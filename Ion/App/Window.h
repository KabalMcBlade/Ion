#pragma once

#include <windows.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"


#define ION_KEY_ESCAPE  WM_USER + 1

#define ION_MOUSE_MOVE  WM_USER + 100

#define ION_WND_CLOSE   WM_USER + 200
#define ION_WND_RESIZE  WM_USER + 201


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

class MouseState 
{
public:
    struct ButtonsState 
    {
        ionBool IsPressed;
        ionBool WasClicked;
        ionBool WasRelease;
    } m_buttons[2];

    struct PositionState 
    {
        ionS32 m_x;
        ionS32 m_y;
        struct DeltaState
        {
            ionS32 m_x;
            ionS32 m_y;
        } m_delta;
    } m_position;

    struct WheelState 
    {
        ionBool  m_wasMoved;
        ionFloat m_distance;
    } m_wheel;

    MouseState();
    ~MouseState();
};

class ION_DLL Window
{
public:
    Window();
    ~Window();

    ionBool        Create(WNDPROC _wndproc, const eosTString& _name, ionU32 _width, ionU32 _height, ionBool _fullScreen);
    ionBool        Loop();

    const HINSTANCE& GetInstance() const { return m_instance; }
    const HWND& GetHandle() const { return m_handle; }

private:
    void MouseClick(ionSize _indexButton, ionBool _state);
    void MouseMove(ionS32 _x, ionS32 _y);
    void MouseWheel(ionFloat _distance);
    void MouseReset();

private:
    eosTString      m_name;
    MouseState      m_mouse;
    HINSTANCE       m_instance;
    HWND            m_handle;
    ionU32          m_width;
    ionU32          m_height;
    ionBool         m_fullScreen;
};

ION_NAMESPACE_END