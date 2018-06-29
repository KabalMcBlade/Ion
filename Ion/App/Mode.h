#pragma once

#include <windows.h>

#include "../Core/CoreDefs.h"

ION_NAMESPACE_BEGIN


enum ECursorMode
{
    ECursorMode_NormalWindows,
    ECursorMode_FPS_TPS         // first person or third person game
};


struct MouseState
{
    struct ButtonsState
    {
        ionBool IsPressed;
        ionBool WasClicked;
        ionBool WasRelease;
    } m_buttons[2];

    struct PositionState
    {
        ionFloat m_x;
        ionFloat m_y;
        struct DeltaState
        {
            ionFloat m_x;
            ionFloat m_y;
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


enum EKeyEvent : ionU32
{
    EKeyEvent_Down = WM_KEYDOWN,
    EKeyEvent_Up = WM_KEYUP
};
/*
enum EKeyCode : ionU32
{
    EKeyCode_Space = VK_SPACE,
    EKeyCode_End = VK_END,
    EKeyCode_Home = VK_END,

    EKeyCode_Left = VK_LEFT,
    EKeyCode_Up = VK_UP,
    EKeyCode_Right = VK_RIGHT,
    EKeyCode_Down = VK_DOWN,

    EKeyCode_Print = VK_PRINT,
    EKeyCode_Insert = VK_INSERT,
    EKeyCode_Delete = VK_DELETE,

    EKeyCode_NumPad0 = VK_NUMPAD0,
    EKeyCode_NumPad1 = VK_NUMPAD1,
    EKeyCode_NumPad2 = VK_NUMPAD2,
    EKeyCode_NumPad3 = VK_NUMPAD3,
    EKeyCode_NumPad4 = VK_NUMPAD4,
    EKeyCode_NumPad5 = VK_NUMPAD5,
    EKeyCode_NumPad6 = VK_NUMPAD6,
    EKeyCode_NumPad7 = VK_NUMPAD7,
    EKeyCode_NumPad8 = VK_NUMPAD8,
    EKeyCode_NumPad9 = VK_NUMPAD9,
    EKeyCode_Mul = VK_MULTIPLY,
    EKeyCode_Add = VK_ADD,
    EKeyCode_Separator = VK_SEPARATOR,
    EKeyCode_Min = VK_SUBTRACT,
    EKeyCode_Decimal = VK_DECIMAL,
    EKeyCode_Div = VK_DIVIDE,
    EKeyCode_F1 = VK_F1,
    EKeyCode_F2 = VK_F2,
    EKeyCode_F3 = VK_F3,
    EKeyCode_F4 = VK_F4,
    EKeyCode_F5 = VK_F5,
    EKeyCode_F6 = VK_F6,
    EKeyCode_F7 = VK_F7,
    EKeyCode_F8 = VK_F8,
    EKeyCode_F9 = VK_F9,
    EKeyCode_F10 = VK_F10,
    EKeyCode_F11 = VK_F11,
    EKeyCode_F12 = VK_F12,
};
*/



struct KeyboardState
{
    EKeyEvent m_keyEvent;
    //EKeyCode m_keyCode;
};

ION_NAMESPACE_END