#pragma once

#include <windows.h>

#include "../Core/CoreDefs.h"

ION_NAMESPACE_BEGIN


enum ECursorMode
{
    ECursorMode_NormalWindows,
    ECursorMode_FPS_TPS         // first person or third person game
};


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

ION_NAMESPACE_END