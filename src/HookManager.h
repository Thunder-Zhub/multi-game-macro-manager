#pragma once
#include "common.h"
#include <vector>
#include <mutex>

class HookManager {
public:
    HookManager();
    ~HookManager();
    
    // Install/uninstall low-level hooks
    bool InstallHooks(HINSTANCE hInst);
    void UninstallHooks();
    
    // Get current bindings list (from active profile)
    void SetBindings(std::vector<Binding>* bindings);
    void SetGlobalEnabled(bool enabled) { m_globalEnabled = enabled; }
    bool IsGlobalEnabled() const { return m_globalEnabled; }
    
    // Get/set current target PID
    void SetTargetPID(DWORD pid) { m_targetPID = pid; }
    DWORD GetTargetPID() const { return m_targetPID; }
    
private:
    HHOOK m_mouseHook;
    HHOOK m_keyboardHook;
    std::vector<Binding>* m_bindings;
    DWORD m_targetPID;
    bool m_globalEnabled;
    int m_nextTimerId;
    std::mutex m_bindingsMutex;
    
    // Static callback wrappers
    static LRESULT CALLBACK StaticMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK StaticKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static void CALLBACK StaticTimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
    
    // Instance callbacks
    LRESULT MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    LRESULT KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    void TimerProc(UINT_PTR id);
    
    void FireAction(Binding& b);
    void StartBindingTimer(HWND hwnd, Binding& b);
    void StopBindingTimer(HWND hwnd, Binding& b);
    
    // Static instance pointer for callbacks
    static HookManager* s_instance;
    friend class MainWindow;
};
