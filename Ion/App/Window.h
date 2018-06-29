#pragma once

#include <windows.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "Mode.h"

#define ION_KEY_ESCAPE  WM_USER + 1

#define ION_MOUSE_MOVE  WM_USER + 100

#define ION_WND_CLOSE   WM_USER + 200
#define ION_WND_RESIZE  WM_USER + 201


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

class ION_DLL Window
{
public:
    Window();
    ~Window();

    ionBool        Create(WNDPROC _wndproc, const eosTString& _name, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _showCursor /*= true*/);
    ionBool        Loop();

    const HINSTANCE& GetInstance() const { return m_instance; }
    const HWND& GetHandle() const { return m_handle; }

    void SetInputMode(ionBool _clipToClient = false, ionBool _cursorDisabled = false, HCURSOR _cursor = nullptr);

private:
    void MouseClick(ionSize _indexButton, ionBool _state);
    void MouseMove(ionFloat _x, ionFloat _y);
    void MouseWheel(ionFloat _distance);
    void MouseReset();

    void GetMousePos(ionFloat& _xpos, ionFloat& _ypos);

    void CenterCursor();

private:
    eosTString      m_name;
    MouseState      m_mouse;
    HINSTANCE       m_instance;
    HWND            m_handle;
    ionS32          m_width;
    ionS32          m_height;
    ECursorMode     m_cursorMode;
    ionBool         m_fullScreen;
    ionBool         m_skipNextMouseMove;
};

ION_NAMESPACE_END