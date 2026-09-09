#pragma once
#include "common.h"
#include "ProfileManager.h"
#include "HookManager.h"
#include <vector>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    
    bool Create(HINSTANCE hInst);
    void Show(int nCmdShow);
    int Run();
    
private:
    HWND m_hWnd;
    HWND m_hProfileTab;
    HWND m_hTargetLabel;
    HWND m_hListView;
    HWND m_hStatusBar;
    HWND m_hGlobalToggle;
    
    ProfileManager m_profileMgr;
    HookManager m_hookMgr;
    
    int m_currentProfileIndex;
    
    // Message handlers
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT OnMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
    // UI Methods
    void CreateControls();
    void RefreshProfileTabs();
    void RefreshBindingList();
    void OnSelectTarget();
    void OnAddBinding();
    void OnEditBinding(int index);
    void OnDeleteBinding(int index);
    void OnProfileChanged(int profileIndex);
    void OnGlobalToggle();
    void UpdateStatusBar();
    
    // Static instance pointer
    static MainWindow* s_instance;
    friend class DialogHelpers;
};
