#include <windows.h>
#include "MainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES };
    InitCommonControlsEx(&icc);
    
    MainWindow window;
    if (!window.Create(hInstance)) {
        MessageBoxW(NULL, L"Failed to create main window!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    window.Show(nCmdShow);
    return window.Run();
}
