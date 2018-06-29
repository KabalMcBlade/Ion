#include "Window.h"

#include "../Renderer/RenderManager.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


MouseState::MouseState() 
{
    m_buttons[0].IsPressed = false;
    m_buttons[0].WasClicked = false;
    m_buttons[0].WasRelease = false;
    m_buttons[1].IsPressed = false;
    m_buttons[1].WasClicked = false;
    m_buttons[1].WasRelease = false;
    m_position.m_x = 0;
    m_position.m_y = 0;
    m_position.m_delta.m_x = 0;
    m_position.m_delta.m_y = 0;
    m_wheel.m_wasMoved = false;
    m_wheel.m_distance = 0.0f;
}

MouseState::~MouseState() 
{
}


Window::Window() : m_instance(), m_handle(), m_skipNextMouseMove(false)
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

ionBool Window::Create(WNDPROC _wndproc, const eosTString& _name, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _showCursor /*= true*/)
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

    ShowCursor(_showCursor);
    ShowWindow(m_handle, SW_SHOW);  //SW_SHOWNORMAL
    UpdateWindow(m_handle);
    SetForegroundWindow(m_handle);
    SetFocus(m_handle);

    SetInputMode();

    return true;
}

void Window::SetInputMode(ionBool _clipToClient /*= false*/, ionBool _cursorDisabled /*= false*/, HCURSOR _cursor /*= nullptr*/)
{
    if (_cursorDisabled)
    {
        SetCursor(nullptr);
    }
    else
    {
        if (_cursor != nullptr)
        {
            SetCursor(_cursor);
        }
        else
        {
            SetCursor(LoadCursorW(NULL, IDC_ARROW));
        }
    }

    if (_clipToClient)
    {
        RECT clipRect;
        GetClientRect(m_handle, &clipRect);
        ClientToScreen(m_handle, (POINT*)&clipRect.left);
        ClientToScreen(m_handle, (POINT*)&clipRect.right);
        ClipCursor(&clipRect);

        m_cursorMode = ECursorMode_FPS_TPS;
    }
    else
    {
        m_cursorMode = ECursorMode_NormalWindows;

        ClipCursor(nullptr);
    }
}

void Window::GetMousePos(ionFloat& _xpos, ionFloat& _ypos)
{
    POINT pos;

    if (GetCursorPos(&pos))
    {
        ScreenToClient(m_handle, &pos);

        ionS32 midX = m_width >> 1;
        ionS32 midY = m_height >> 1;

        _xpos = static_cast<ionFloat>(pos.x - midX);
        _ypos = static_cast<ionFloat>(pos.y - midY);
    }
}

void Window::CenterCursor()
{
    ionS32 halfWidth = m_width >> 1;
    ionS32 halfHeight = m_height >> 1;

    POINT pos = { halfWidth, halfHeight };
    ClientToScreen(m_handle, &pos);
    SetCursorPos(halfWidth, halfHeight);
}

ionBool Window::Loop()
{
    // Display window
    UpdateWindow(m_handle);

    // Main message loop
    MSG message;
    ionBool loop = true;
    ionBool resize = false;
    ionBool result = true;

    ionFloat mousePosX = 0;
    ionFloat mousePosY = 0;

    CenterCursor();

    m_skipNextMouseMove = false;

    ionRenderManager().Prepare();

    while (loop)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            // Process events
            switch (message.message)
            {
            case ION_WND_RESIZE:
                resize = true;
                break;
            case ION_KEY_ESCAPE:
            case ION_WND_CLOSE:
                loop = false;
                break;
            case ION_MOUSE_MOVE:
                GetMousePos(mousePosX, mousePosY);
                
                if (m_cursorMode == ECursorMode_FPS_TPS)
                {
                    if (m_skipNextMouseMove == false)
                    {
                        MouseMove(mousePosX, mousePosY);
                        CenterCursor();

                        m_skipNextMouseMove = true;

                        GetMousePos(mousePosX, mousePosY);
                        m_mouse.m_position.m_x = mousePosX;
                        m_mouse.m_position.m_y = mousePosY;
                    }
                    else
                    {
                        m_skipNextMouseMove = false;
                    }
                }
                else
                {
                    MouseMove(mousePosX, mousePosY);
                }
           
                break;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            if (resize)
            {
                resize = false;
                ionRenderManager().Resize(m_width, m_height);
            }
            else
            {
                ionRenderManager().CoreLoop();
                MouseReset();
            }
        }
    }

    return result;
}

void Window::MouseClick(ionSize _indexButton, ionBool _state) 
{
    if (_indexButton < 2)
    {
        m_mouse.m_buttons[_indexButton].IsPressed = _state;
        m_mouse.m_buttons[_indexButton].WasClicked = _state;
        m_mouse.m_buttons[_indexButton].WasRelease = !_state;
        
        //ionRenderManager().SetMousePos(LOWORD(message.lParam), HIWORD(message.lParam));
    }
}

void Window::MouseMove(ionFloat _x, ionFloat _y)
{
    m_mouse.m_position.m_delta.m_x = _x - m_mouse.m_position.m_x;
    m_mouse.m_position.m_delta.m_y = m_mouse.m_position.m_y - _y;
    m_mouse.m_position.m_x = _x;
    m_mouse.m_position.m_y = _y;

    ionRenderManager().SetMousePos(m_mouse.m_position.m_delta.m_x, m_mouse.m_position.m_delta.m_y);

    //ionRenderManager().SetMousePos(m_mouse.m_position.m_x, m_mouse.m_position.m_y);
}

void Window::MouseWheel(ionFloat _distance)
{
    m_mouse.m_wheel.m_wasMoved = true;
    m_mouse.m_wheel.m_distance = _distance;
    
    //ionRenderManager().SetMousePos(LOWORD(message.lParam), HIWORD(message.lParam));
}

void Window::MouseReset() 
{
    m_mouse.m_position.m_delta.m_x = 0;
    m_mouse.m_position.m_delta.m_y = 0;
    m_mouse.m_buttons[0].WasClicked = false;
    m_mouse.m_buttons[0].WasRelease = false;
    m_mouse.m_buttons[1].WasClicked = false;
    m_mouse.m_buttons[1].WasRelease = false;
    m_mouse.m_wheel.m_wasMoved = false;
    m_mouse.m_wheel.m_distance = 0.0f;
}

ION_NAMESPACE_END