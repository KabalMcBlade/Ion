#include "Window.h"

#define APP_NAME     L"Ion Demo"

ION_NAMESPACE_BEGIN

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    case WM_CLOSE:
        PostMessage(hWnd, WM_USER + 1, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

Window::Window() : m_instance(), m_handle()
{

}

Window::~Window()
{
    if (m_handle)
    {
        DestroyWindow(m_handle);
    }

    if (m_instance)
    {
        UnregisterClass(APP_NAME, m_instance);
    }
}

ionBool Window::Create()
{
    m_instance = GetModuleHandle(nullptr);

    // Register window class
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_instance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = APP_NAME;
    wcex.hIconSm = NULL;

    if (!RegisterClassEx(&wcex))
    {
        return false;
    }

    // Create window
    m_handle = CreateWindow(APP_NAME, APP_NAME, WS_OVERLAPPEDWINDOW, 20, 20, 640, 480, nullptr, nullptr, m_instance, nullptr);
    if (!m_handle)
    {
        return false;
    }

    return m_vulkan.Init(m_instance, m_handle);
}


ionBool Window::Loop()
{
    // Display window
    ShowWindow(m_handle, SW_SHOWNORMAL);
    UpdateWindow(m_handle);

    // Main message loop
    MSG message;
    ionBool loop = true;
    ionBool result = true;

    while (loop)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            // Process events
            switch (message.message)
            {
                // Close
            case WM_USER + 1:
                loop = false;
                break;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            Sleep(100);
        }
    }

    m_vulkan.Shutdown();

    return result;
}

ION_NAMESPACE_END