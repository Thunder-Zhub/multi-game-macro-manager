#include "MainWindow.h"
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

MainWindow* MainWindow::s_instance = nullptr;

MainWindow::MainWindow() : m_hWnd(NULL), m_currentProfileIndex(0) {
    s_instance = this;
}

MainWindow::~MainWindow() {
    if (m_hWnd) DestroyWindow(m_hWnd);
}

bool MainWindow::Create(HINSTANCE hInst) {
    m_profileMgr.LoadConfig();
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"MacroManagerMain";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);
    
    m_hWnd = CreateWindowW(L"MacroManagerMain",
        L"Multi-Game Macro Manager v2.0",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 520,
        NULL, NULL, hInst, this);
    
    if (!m_hWnd) return false;
    
    CreateControls();
    
    if (!m_hookMgr.InstallHooks(hInst)) {
        MessageBoxW(m_hWnd, L"Failed to install hooks. Run as Administrator!",
            L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    
    return true;
}

void MainWindow::Show(int nCmdShow) {
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
    RefreshProfileTabs();
    RefreshBindingList();
}

int MainWindow::Run() {
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = nullptr;
    
    if (msg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<MainWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (pThis) return pThis->OnMessage(msg, wParam, lParam);
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT MainWindow::OnMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        CreateControls();
        SetTimer(m_hWnd, 1, 300, NULL);
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == 100) OnSelectTarget();
        else if (id == 101) OnAddBinding();
        else if (id == 102) {
            int sel = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
            if (sel >= 0) OnEditBinding(sel);
        }
        else if (id == 103) {
            int sel = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
            if (sel >= 0) OnDeleteBinding(sel);
        }
        else if (id == 104) OnGlobalToggle();
        break;
    }
    case WM_NOTIFY: {
        LPNMHDR pnmhdr = (LPNMHDR)lParam;
        if (pnmhdr->idFrom == 200) { // Profile tab
            if (pnmhdr->code == TCN_SELCHANGE) {
                int idx = TabCtrl_GetCurSel(m_hProfileTab);
                OnProfileChanged(idx);
            }
        }
        break;
    }
    case WM_TIMER:
        RefreshBindingList();
        UpdateStatusBar();
        break;
    case WM_DESTROY:
        KillTimer(m_hWnd, 1);
        m_hookMgr.UninstallHooks();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

void MainWindow::CreateControls() {
    // Global toggle button
    m_hGlobalToggle = CreateWindowW(L"BUTTON", L"🔴 DISABLE ALL (F10)",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 10, 150, 30, m_hWnd, (HMENU)104, NULL, NULL);
    
    // Target selection
    CreateWindowW(L"BUTTON", L"Select Target Game",
        WS_CHILD | WS_VISIBLE, 170, 10, 140, 30,
        m_hWnd, (HMENU)100, NULL, NULL);
    
    m_hTargetLabel = CreateWindowW(L"STATIC", L"Target: (None)",
        WS_CHILD | WS_VISIBLE, 320, 17, 360, 20,
        m_hWnd, NULL, NULL, NULL);
    
    // Profile tab control
    m_hProfileTab = CreateWindowW(WC_TABCONTROLW, NULL,
        WS_CHILD | WS_VISIBLE | TCS_TABS,
        10, 50, 680, 30, m_hWnd, (HMENU)200, NULL, NULL);
    
    // Binding ListView
    m_hListView = CreateWindowExW(0, WC_LISTVIEWW, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        10, 90, 680, 300, m_hWnd, (HMENU)201, NULL, NULL);
    ListView_SetExtendedListViewStyle(m_hListView,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    
    // Setup ListView columns
    LVCOLUMNW col = {};
    col.mask = LVCF_TEXT | LVCF_WIDTH;
    col.pszText = (LPWSTR)L"Trigger"; col.cx = 100;
    ListView_InsertColumn(m_hListView, 0, &col);
    col.pszText = (LPWSTR)L"Action"; col.cx = 150;
    ListView_InsertColumn(m_hListView, 1, &col);
    col.pszText = (LPWSTR)L"CPS"; col.cx = 60;
    ListView_InsertColumn(m_hListView, 2, &col);
    col.pszText = (LPWSTR)L"Status"; col.cx = 100;
    ListView_InsertColumn(m_hListView, 3, &col);
    
    // Buttons
    CreateWindowW(L"BUTTON", L"+ Add Binding",
        WS_CHILD | WS_VISIBLE, 10, 400, 120, 30,
        m_hWnd, (HMENU)101, NULL, NULL);
    CreateWindowW(L"BUTTON", L"Edit",
        WS_CHILD | WS_VISIBLE, 140, 400, 80, 30,
        m_hWnd, (HMENU)102, NULL, NULL);
    CreateWindowW(L"BUTTON", L"Delete",
        WS_CHILD | WS_VISIBLE, 230, 400, 80, 30,
        m_hWnd, (HMENU)103, NULL, NULL);
    
    // Status bar
    m_hStatusBar = CreateWindowW(L"STATIC",
        L"Status: Ready | Global: ENABLED (F10 to toggle)",
        WS_CHILD | WS_VISIBLE,
        10, 440, 680, 40, m_hWnd, NULL, NULL, NULL);
}

void MainWindow::RefreshProfileTabs() {
    TabCtrl_DeleteAllItems(m_hProfileTab);
    
    const auto& config = m_profileMgr.GetConfig();
    for (size_t i = 0; i < config.profiles.size(); i++) {
        TCITEMW item = {};
        item.mask = TCIF_TEXT;
        item.pszText = (LPWSTR)config.profiles[i].name.c_str();
        TabCtrl_InsertItem(m_hProfileTab, (int)i, &item);
    }
    
    if (config.profiles.size() > 0) {
        TabCtrl_SetCurSel(m_hProfileTab, 0);
        OnProfileChanged(0);
    }
}

void MainWindow::RefreshBindingList() {
    Profile* profile = m_profileMgr.GetCurrentProfile();
    if (!profile) {
        ListView_DeleteAllItems(m_hListView);
        return;
    }
    
    ListView_DeleteAllItems(m_hListView);
    m_hookMgr.SetBindings(&profile->bindings);
    m_hookMgr.SetTargetPID(profile->targetPID);
    
    for (size_t i = 0; i < profile->bindings.size(); i++) {
        const auto& b = profile->bindings[i];
        
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = (int)i;
        
        std::wstring triggerStr = VkToString(b.triggerCode);
        item.pszText = (LPWSTR)triggerStr.c_str();
        ListView_InsertItem(m_hListView, &item);
        
        std::wstring actionStr = ActionToString(b);
        ListView_SetItemText(m_hListView, (int)i, 1, (LPWSTR)actionStr.c_str());
        
        std::wstring cpsStr = std::to_wstring(b.cps);
        ListView_SetItemText(m_hListView, (int)i, 2, (LPWSTR)cpsStr.c_str());
        
        std::wstring statusStr = b.isHeld ? L"● Firing" : L"○ Ready";
        ListView_SetItemText(m_hListView, (int)i, 3, (LPWSTR)statusStr.c_str());
    }
}

void MainWindow::OnSelectTarget() {
    Profile* profile = m_profileMgr.GetCurrentProfile();
    if (!profile) {
        MessageBoxW(m_hWnd, L"Create a profile first!", L"Info", MB_OK);
        return;
    }
    
    auto processes = m_profileMgr.EnumRunningProcesses();
    
    // Simple dialog to select process
    HWND hDlg = CreateWindowW(L"#32770", L"Select Target Game",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 400, 300, m_hWnd, NULL, NULL, NULL);
    
    // This is simplified - in real impl, would use proper dialog
    HWND hList = CreateWindowW(L"LISTBOX", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
        10, 10, 380, 230, hDlg, (HMENU)1, NULL, NULL);
    
    for (const auto& p : processes) {
        std::wstring label = p.exeName + L" (PID: " + std::to_wstring(p.pid) + L")";
        SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)label.c_str());
    }
    
    CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE,
        10, 250, 90, 30, hDlg, (HMENU)IDOK, NULL, NULL);
    CreateWindowW(L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE,
        110, 250, 90, 30, hDlg, (HMENU)IDCANCEL, NULL, NULL);
    
    // Note: This is simplified demo. Use proper dialog template in production
}

void MainWindow::OnAddBinding() {
    Profile* profile = m_profileMgr.GetCurrentProfile();
    if (!profile) {
        MessageBoxW(m_hWnd, L"Create a profile first!", L"Info", MB_OK);
        return;
    }
    
    if (profile->bindings.size() >= 5) {
        MessageBoxW(m_hWnd, L"Maximum 5 bindings per profile!", L"Limit", MB_OK | MB_ICONWARNING);
        return;
    }
    
    // Dialog for adding binding
    MessageBoxW(m_hWnd, L"Binding dialog - TODO: Implement", L"Info", MB_OK);
}

void MainWindow::OnEditBinding(int index) {
    Profile* profile = m_profileMgr.GetCurrentProfile();
    if (!profile || index < 0 || index >= (int)profile->bindings.size()) return;
    
    MessageBoxW(m_hWnd, L"Edit binding dialog - TODO: Implement", L"Info", MB_OK);
}

void MainWindow::OnDeleteBinding(int index) {
    Profile* profile = m_profileMgr.GetCurrentProfile();
    if (!profile || index < 0 || index >= (int)profile->bindings.size()) return;
    
    profile->bindings.erase(profile->bindings.begin() + index);
    m_profileMgr.UpdateProfile(m_currentProfileIndex, *profile);
    RefreshBindingList();
}

void MainWindow::OnProfileChanged(int profileIndex) {
    m_currentProfileIndex = profileIndex;
    m_profileMgr.SetCurrentProfile(profileIndex);
    RefreshBindingList();
    UpdateStatusBar();
}

void MainWindow::OnGlobalToggle() {
    bool enabled = m_hookMgr.IsGlobalEnabled();
    m_hookMgr.SetGlobalEnabled(!enabled);
    m_profileMgr.GetConfig().globalEnabled = !enabled;
    m_profileMgr.SaveConfig();
    UpdateStatusBar();
}

void MainWindow::UpdateStatusBar() {
    Profile* profile = m_profileMgr.GetCurrentProfile();
    bool globalEnabled = m_hookMgr.IsGlobalEnabled();
    
    std::wstring text = L"Profile: ";
    if (profile) text += profile->name;
    else text += L"(None)";
    
    text += L" | Global: ";
    text += globalEnabled ? L"ENABLED" : L"DISABLED";
    text += L" (F10 to toggle)";
    
    SetWindowTextW(m_hStatusBar, text.c_str());
    
    // Update button text
    SetWindowTextW(m_hGlobalToggle,
        globalEnabled ? L"🟢 ENABLED (F10)" : L"🔴 DISABLED (F10)");
}
