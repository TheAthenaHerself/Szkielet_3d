#include <windows.h>
#include <winuser.h>
#include <stdexcept>
#include <iostream>
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
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = CLASS_NAME;
    wc.hIconSm = nullptr;

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowExW(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Basic Template",              // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        nullptr,       // Parent window    
        nullptr,       // Menu
        hInstance,  // Instance handle
        nullptr        // Additional application data
    );

    if (hwnd == nullptr)
    {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    int a = 0;
    while (a = GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (a == -1) {
        return 1;
    }

    return 0;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    try {
        switch (uMsg)
        {
        case WM_CREATE:
            OnInit(hwnd);
            InitTimer(hwnd);
            return 0;
        case WM_TIMER:
            OnUpdate();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_PAINT:
            OnRender();
            ValidateRect(hwnd, nullptr);
            return 0;
        case WM_DESTROY:
            OnDestroy();
            PostQuitMessage(0);
            DestroyWindow(hwnd);
            ReleaseTimer(hwnd);
            return 0;
        }
    }
    catch (std::runtime_error &error) {
        std::string str(error.what());
        str = "\n" + str + "\n";
        std::wstring wsTmp(str.begin(), str.end());
        OutputDebugStringW(wsTmp.c_str());
        exit(1);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}