#include "HookManager.h"
#include <map>

HookManager* HookManager::s_instance = nullptr;

HookManager::HookManager() 
    : m_mouseHook(NULL), m_keyboardHook(NULL), m_bindings(nullptr), 
      m_targetPID(0), m_globalEnabled(true), m_nextTimerId(1) {
    s_instance = this;
}

HookManager::~HookManager() {
    UninstallHooks();
}

bool HookManager::InstallHooks(HINSTANCE hInst) {
    m_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, StaticMouseProc, hInst, 0);
    m_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, StaticKeyboardProc, hInst, 0);
    return (m_mouseHook != NULL && m_keyboardHook != NULL);
}

void HookManager::UninstallHooks() {
    if (m_mouseHook) {
        UnhookWindowsHookEx(m_mouseHook);
        m_mouseHook = NULL;
    }
    if (m_keyboardHook) {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = NULL;
    }
}

void HookManager::SetBindings(std::vector<Binding>* bindings) {
    std::lock_guard<std::mutex> lock(m_bindingsMutex);
    m_bindings = bindings;
}

LRESULT CALLBACK HookManager::StaticMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (s_instance) return s_instance->MouseProc(nCode, wParam, lParam);
    return 0;
}

LRESULT CALLBACK HookManager::StaticKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (s_instance) return s_instance->KeyboardProc(nCode, wParam, lParam);
    return 0;
}

void CALLBACK HookManager::StaticTimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time) {
    if (s_instance) s_instance->TimerProc(id);
}

LRESULT HookManager::MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && m_globalEnabled && m_targetPID > 0 && IsTargetForeground(m_targetPID)) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
        WORD xButton = HIWORD(pMouse->mouseData);
        
        std::lock_guard<std::mutex> lock(m_bindingsMutex);
        if (!m_bindings) return CallNextHookEx(m_mouseHook, nCode, wParam, lParam);
        
        for (auto& b : *m_bindings) {
            if (b.triggerType != TRIG_MOUSE_BTN) continue;
            
            bool isDown = false, isUp = false;
            if (b.triggerCode == XBUTTON1) {
                isDown = (wParam == WM_XBUTTONDOWN && xButton == XBUTTON1);
                isUp = (wParam == WM_XBUTTONUP && xButton == XBUTTON1);
            } else if (b.triggerCode == XBUTTON2) {
                isDown = (wParam == WM_XBUTTONDOWN && xButton == XBUTTON2);
                isUp = (wParam == WM_XBUTTONUP && xButton == XBUTTON2);
            }
            
            if (isDown) {
                b.isHeld = true;
                // StartBindingTimer will be called from main thread
            } else if (isUp) {
                b.isHeld = false;
            }
        }
    }
    return CallNextHookEx(m_mouseHook, nCode, wParam, lParam);
}

LRESULT HookManager::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && m_globalEnabled && m_targetPID > 0 && IsTargetForeground(m_targetPID)) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        
        std::lock_guard<std::mutex> lock(m_bindingsMutex);
        if (!m_bindings) return CallNextHookEx(m_keyboardHook, nCode, wParam, lParam);
        
        for (auto& b : *m_bindings) {
            if (b.triggerType != TRIG_KEY) continue;
            if ((int)pKey->vkCode != b.triggerCode) continue;
            
            if (wParam == WM_KEYDOWN) {
                b.isHeld = true;
            } else if (wParam == WM_KEYUP) {
                b.isHeld = false;
            }
        }
    }
    return CallNextHookEx(m_keyboardHook, nCode, wParam, lParam);
}

void HookManager::TimerProc(UINT_PTR id) {
    std::lock_guard<std::mutex> lock(m_bindingsMutex);
    if (!m_bindings || !m_globalEnabled || m_targetPID == 0) return;
    
    for (auto& b : *m_bindings) {
        if (b.timerId == id && b.isHeld && IsTargetForeground(m_targetPID)) {
            FireAction(b);
        }
    }
}

void HookManager::FireAction(Binding& b) {
    INPUT inputs[2] = {};
    
    if (b.actionType == ACT_LEFT_CLICK) {
        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs[1].type = INPUT_MOUSE;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(2, inputs, sizeof(INPUT));
    } else if (b.actionType == ACT_RIGHT_CLICK) {
        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        inputs[1].type = INPUT_MOUSE;
        inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        SendInput(2, inputs, sizeof(INPUT));
    } else if (b.actionType == ACT_KEY_PRESS) {
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = b.actionCode;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = b.actionCode;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, inputs, sizeof(INPUT));
    }
}

void HookManager::StartBindingTimer(HWND hwnd, Binding& b) {
    if (b.timerId == 0) {
        b.timerId = m_nextTimerId++;
        int intervalMs = 1000 / (b.cps > 0 ? b.cps : 1);
        SetTimer(hwnd, b.timerId, intervalMs, StaticTimerProc);
    }
}

void HookManager::StopBindingTimer(HWND hwnd, Binding& b) {
    if (b.timerId != 0) {
        KillTimer(hwnd, b.timerId);
        b.timerId = 0;
    }
}
