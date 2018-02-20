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

ionBool Window::Create(WNDPROC _wndproc, const eosTString& _name, ionU32 _width, ionU32 _height, ionBool _fullScreen)
{
    m_name = _name;
    m_width = _width;
    m_height = _height;
    m_fullScreen = _fullScreen;

    m_instance = GetModuleHandle(nullptr);

    // Register window class
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = _wndproc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_instance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//(HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = m_name.c_str();
    wcex.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

    if (!RegisterClassEx(&wcex))
    {
        return false;
    }

    ionS32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
    ionS32 screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (m_fullScreen)
    {
        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = screenWidth;
        dmScreenSettings.dmPelsHeight = screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if ((m_width != (ionU32)screenWidth) && (m_height != (ionU32)screenHeight))
        {
            if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
            {
                m_fullScreen = false;
            }
        }

    }

    DWORD dwExStyle;
    DWORD dwStyle;

    if (m_fullScreen)
    {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    }

    RECT windowRect;
    windowRect.left = 0L;
    windowRect.top = 0L;
    windowRect.right = m_fullScreen ? (long)screenWidth : (long)m_width;
    windowRect.bottom = m_fullScreen ? (long)screenHeight : (long)m_height;

    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    // Create window
    m_handle = CreateWindowEx(0,
        m_name.c_str(),
        m_name.c_str(),
        dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0,
        0,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        m_instance,
        nullptr);

    if (!m_handle)
    {
        std::cout << "Could not create window!" << std::endl;
        return false;
    }

    ShowWindow(m_handle, SW_SHOW);  //SW_SHOWNORMAL
    SetForegroundWindow(m_handle);
    SetFocus(m_handle);

    return m_vulkan.Init(m_instance, m_handle);
}


ionBool Window::Loop()
{
    // Display window
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
            //m_vulkan.RenderFrame();
            Sleep(100);
        }
    }

    m_vulkan.Shutdown();

    return result;
}

ION_NAMESPACE_END