#include "Window.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


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
        UnregisterClass(m_name.c_str(), m_instance);
    }
}

ionBool Window::Create(WNDPROC _wndproc, const eosTString& _name)
{
    m_name = _name;

    m_instance = GetModuleHandle(nullptr);

    // Register window class
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = _wndproc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_instance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = m_name.c_str();
    wcex.hIconSm = NULL;

    if (!RegisterClassEx(&wcex))
    {
        return false;
    }

    // Create window
    m_handle = CreateWindow(m_name.c_str(), m_name.c_str(), WS_OVERLAPPEDWINDOW, 20, 20, 640, 480, nullptr, nullptr, m_instance, nullptr);
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