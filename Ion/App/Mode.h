#pragma once

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>

#include "../Core/CoreDefs.h"

ION_NAMESPACE_BEGIN


enum ECursorMode
{
    ECursorMode_NormalWindows,
    ECursorMode_FPS_TPS         // first person or third person game
};


struct ION_DLL MouseState final
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

enum EKeyboardState
{
    EKeyboardState_Nothing = 0,
    EKeyboardState_Down,
    EKeyboardState_Up
};


enum EKeyboardKey : ionU8
{
    EKeyboardKey_None = 0x00,
    EKeyboardKey_LeftButton = 0x01,
    EKeyboardKey_RightButton = 0x02,
    EKeyboardKey_Cancel = 0x03,
    EKeyboardKey_MiddleButton = 0x04,
    EKeyboardKey_ExtraButton1 = 0x05,
    EKeyboardKey_ExtraButton2 = 0x06,
    EKeyboardKey_Back = 0x08,
    EKeyboardKey_Tab = 0x09,
    EKeyboardKey_Clear = 0x0C,
    EKeyboardKey_Return = 0x0D,
    EKeyboardKey_Shift = 0x10,
    EKeyboardKey_Control = 0x11,
    EKeyboardKey_Menu = 0x12,
    EKeyboardKey_Pause = 0x13,
    EKeyboardKey_CapsLock = 0x14,
    EKeyboardKey_Kana = 0x15,
    EKeyboardKey_Hangeul = 0x15,
    EKeyboardKey_Hangul = 0x15,
    EKeyboardKey_Junja = 0x17,
    EKeyboardKey_Final = 0x18,
    EKeyboardKey_Hanja = 0x19,
    EKeyboardKey_Kanji = 0x19,
    EKeyboardKey_Escape = 0x1B,
    EKeyboardKey_Convert = 0x1C,
    EKeyboardKey_NonConvert = 0x1D,
    EKeyboardKey_Accept = 0x1E,
    EKeyboardKey_ModeChange = 0x1F,
    EKeyboardKey_Space = 0x20,
    EKeyboardKey_Prior = 0x21,
    EKeyboardKey_Next = 0x22,
    EKeyboardKey_End = 0x23,
    EKeyboardKey_Home = 0x24,
    EKeyboardKey_Left = 0x25,
    EKeyboardKey_Up = 0x26,
    EKeyboardKey_Right = 0x27,
    EKeyboardKey_Down = 0x28,
    EKeyboardKey_Select = 0x29,
    EKeyboardKey_Print = 0x2A,
    EKeyboardKey_Execute = 0x2B,
    EKeyboardKey_Snapshot = 0x2C,
    EKeyboardKey_Insert = 0x2D,
    EKeyboardKey_Delete = 0x2E,
    EKeyboardKey_Help = 0x2F,
    EKeyboardKey_N0 = 0x30,
    EKeyboardKey_N1 = 0x31,
    EKeyboardKey_N2 = 0x32,
    EKeyboardKey_N3 = 0x33,
    EKeyboardKey_N4 = 0x34,
    EKeyboardKey_N5 = 0x35,
    EKeyboardKey_N6 = 0x36,
    EKeyboardKey_N7 = 0x37,
    EKeyboardKey_N8 = 0x38,
    EKeyboardKey_N9 = 0x39,
    EKeyboardKey_A = 0x41,
    EKeyboardKey_B = 0x42,
    EKeyboardKey_C = 0x43,
    EKeyboardKey_D = 0x44,
    EKeyboardKey_E = 0x45,
    EKeyboardKey_F = 0x46,
    EKeyboardKey_G = 0x47,
    EKeyboardKey_H = 0x48,
    EKeyboardKey_I = 0x49,
    EKeyboardKey_J = 0x4A,
    EKeyboardKey_K = 0x4B,
    EKeyboardKey_L = 0x4C,
    EKeyboardKey_M = 0x4D,
    EKeyboardKey_N = 0x4E,
    EKeyboardKey_O = 0x4F,
    EKeyboardKey_P = 0x50,
    EKeyboardKey_Q = 0x51,
    EKeyboardKey_R = 0x52,
    EKeyboardKey_S = 0x53,
    EKeyboardKey_T = 0x54,
    EKeyboardKey_U = 0x55,
    EKeyboardKey_V = 0x56,
    EKeyboardKey_W = 0x57,
    EKeyboardKey_X = 0x58,
    EKeyboardKey_Y = 0x59,
    EKeyboardKey_Z = 0x5A,
    EKeyboardKey_LeftWindows = 0x5B,
    EKeyboardKey_RightWindows = 0x5C,
    EKeyboardKey_Application = 0x5D,
    EKeyboardKey_Sleep = 0x5F,
    EKeyboardKey_Numpad0 = 0x60,
    EKeyboardKey_Numpad1 = 0x61,
    EKeyboardKey_Numpad2 = 0x62,
    EKeyboardKey_Numpad3 = 0x63,
    EKeyboardKey_Numpad4 = 0x64,
    EKeyboardKey_Numpad5 = 0x65,
    EKeyboardKey_Numpad6 = 0x66,
    EKeyboardKey_Numpad7 = 0x67,
    EKeyboardKey_Numpad8 = 0x68,
    EKeyboardKey_Numpad9 = 0x69,
    EKeyboardKey_Multiply = 0x6A,
    EKeyboardKey_Add = 0x6B,
    EKeyboardKey_Separator = 0x6C,
    EKeyboardKey_Subtract = 0x6D,
    EKeyboardKey_Decimal = 0x6E,
    EKeyboardKey_Divide = 0x6F,
    EKeyboardKey_F1 = 0x70,
    EKeyboardKey_F2 = 0x71,
    EKeyboardKey_F3 = 0x72,
    EKeyboardKey_F4 = 0x73,
    EKeyboardKey_F5 = 0x74,
    EKeyboardKey_F6 = 0x75,
    EKeyboardKey_F7 = 0x76,
    EKeyboardKey_F8 = 0x77,
    EKeyboardKey_F9 = 0x78,
    EKeyboardKey_F10 = 0x79,
    EKeyboardKey_F11 = 0x7A,
    EKeyboardKey_F12 = 0x7B,
    EKeyboardKey_F13 = 0x7C,
    EKeyboardKey_F14 = 0x7D,
    EKeyboardKey_F15 = 0x7E,
    EKeyboardKey_F16 = 0x7F,
    EKeyboardKey_F17 = 0x80,
    EKeyboardKey_F18 = 0x81,
    EKeyboardKey_F19 = 0x82,
    EKeyboardKey_F20 = 0x83,
    EKeyboardKey_F21 = 0x84,
    EKeyboardKey_F22 = 0x85,
    EKeyboardKey_F23 = 0x86,
    EKeyboardKey_F24 = 0x87,
    EKeyboardKey_NumLock = 0x90,
    EKeyboardKey_ScrollLock = 0x91,
    EKeyboardKey_NEC_Equal = 0x92,
    EKeyboardKey_Fujitsu_Jisho = 0x92,
    EKeyboardKey_Fujitsu_Masshou = 0x93,
    EKeyboardKey_Fujitsu_Touroku = 0x94,
    EKeyboardKey_Fujitsu_Loya = 0x95,
    EKeyboardKey_Fujitsu_Roya = 0x96,
    EKeyboardKey_LeftShift = 0xA0,
    EKeyboardKey_RightShift = 0xA1,
    EKeyboardKey_LeftControl = 0xA2,
    EKeyboardKey_RightControl = 0xA3,
    EKeyboardKey_LeftMenu = 0xA4,
    EKeyboardKey_RightMenu = 0xA5,
    EKeyboardKey_BrowserBack = 0xA6,
    EKeyboardKey_BrowserForward = 0xA7,
    EKeyboardKey_BrowserRefresh = 0xA8,
    EKeyboardKey_BrowserStop = 0xA9,
    EKeyboardKey_BrowserSearch = 0xAA,
    EKeyboardKey_BrowserFavorites = 0xAB,
    EKeyboardKey_BrowserHome = 0xAC,
    EKeyboardKey_VolumeMute = 0xAD,
    EKeyboardKey_VolumeDown = 0xAE,
    EKeyboardKey_VolumeUp = 0xAF,
    EKeyboardKey_MediaNextTrack = 0xB0,
    EKeyboardKey_MediaPrevTrack = 0xB1,
    EKeyboardKey_MediaStop = 0xB2,
    EKeyboardKey_MediaPlayPause = 0xB3,
    EKeyboardKey_LaunchMail = 0xB4,
    EKeyboardKey_LaunchMediaSelect = 0xB5,
    EKeyboardKey_LaunchApplication1 = 0xB6,
    EKeyboardKey_LaunchApplication2 = 0xB7,
    EKeyboardKey_OEM1 = 0xBA,
    EKeyboardKey_OEMPlus = 0xBB,
    EKeyboardKey_OEMComma = 0xBC,
    EKeyboardKey_OEMMinus = 0xBD,
    EKeyboardKey_OEMPeriod = 0xBE,
    EKeyboardKey_OEM2 = 0xBF,
    EKeyboardKey_OEM3 = 0xC0,
    EKeyboardKey_OEM4 = 0xDB,
    EKeyboardKey_OEM5 = 0xDC,
    EKeyboardKey_OEM6 = 0xDD,
    EKeyboardKey_OEM7 = 0xDE,
    EKeyboardKey_OEM8 = 0xDF,
    EKeyboardKey_OEMAX = 0xE1,
    EKeyboardKey_OEM102 = 0xE2,
    EKeyboardKey_ICOHelp = 0xE3,
    EKeyboardKey_ICO00 = 0xE4,
    EKeyboardKey_ProcessKey = 0xE5,
    EKeyboardKey_ICOClear = 0xE6,
    EKeyboardKey_Packet = 0xE7,
    EKeyboardKey_OEMReset = 0xE9,
    EKeyboardKey_OEMJump = 0xEA,
    EKeyboardKey_OEMPA1 = 0xEB,
    EKeyboardKey_OEMPA2 = 0xEC,
    EKeyboardKey_OEMPA3 = 0xED,
    EKeyboardKey_OEMWSCtrl = 0xEE,
    EKeyboardKey_OEMCUSel = 0xEF,
    EKeyboardKey_OEMATTN = 0xF0,
    EKeyboardKey_OEMFinish = 0xF1,
    EKeyboardKey_OEMCopy = 0xF2,
    EKeyboardKey_OEMAuto = 0xF3,
    EKeyboardKey_OEMENLW = 0xF4,
    EKeyboardKey_OEMBackTab = 0xF5,
    EKeyboardKey_ATTN = 0xF6,
    EKeyboardKey_CRSel = 0xF7,
    EKeyboardKey_EXSel = 0xF8,
    EKeyboardKey_EREOF = 0xF9,
    EKeyboardKey_Play = 0xFA,
    EKeyboardKey_Zoom = 0xFB,
    EKeyboardKey_Noname = 0xFC,
    EKeyboardKey_PA1 = 0xFD,
    EKeyboardKey_OEMClear = 0xFE
};


struct ION_DLL KeyboardState final
{
    EKeyboardState m_state;
    EKeyboardKey m_key;

    KeyboardState();
    ~KeyboardState();
};

ION_NAMESPACE_END