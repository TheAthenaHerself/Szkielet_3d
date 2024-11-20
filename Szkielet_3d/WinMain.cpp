#include <windows.h>
#include <winuser.h>
#include "Direct3d.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(
    _In_ [[maybe_unused]] HINSTANCE hInstance,
    _In_opt_ [[maybe_unused]] HINSTANCE hPrevInstance,
    _In_ [[maybe_unused]] PWSTR pCmdLine,
    _In_ [[maybe_unused]] INT nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Basic Template";

    WNDCLASSEX wc = { };

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = CLASS_NAME;
    wc.hIconSm = NULL;

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowExW(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Basic Template",              // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        initD2(hwnd);
        InitTimer(hwnd);
        return 0;
    case WM_TIMER:
        OnTimer();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    case WM_PAINT:
        paintD2(hwnd);
        ValidateRect(hwnd, nullptr);
        return 0;
    case WM_DESTROY:
        destroyD2();
        PostQuitMessage(0);
        DestroyWindow(hwnd);
        ReleaseTimer(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}