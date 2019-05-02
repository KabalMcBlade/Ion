#pragma once

#include <windows.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "Mode.h"
#include "CommandLineParser.h"

#define ION_KEY_ESCAPE      WM_USER + 1

#define ION_MOUSE_MOVE      WM_USER + 10
#define ION_MOUSE_WHEEL     WM_USER + 11
#define ION_MOUSE_CLICK     WM_USER + 12

#define ION_KEY_DOWN        WM_USER + 20
#define ION_KEY_UP          WM_USER + 21

#define ION_WND_CLOSE       WM_USER + 30
#define ION_WND_RESIZE      WM_USER + 31


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

class ION_DLL Window
{
public:
    Window();
    ~Window();

    ionBool ParseCommandLine(ionS32 &argc, char **argv);

    ionBool Create(WNDPROC _wndproc, const TCHAR* _name);
    ionBool Loop();

    const HINSTANCE& GetInstance() const { return m_instance; }
    const HWND& GetHandle() const { return m_handle; }

    void SetInputMode(ionBool _clipToClient = false, ionBool _cursorDisabled = false, HCURSOR _cursor = nullptr);

    ionS32 GetWidth() const { return m_width; }
    ionS32 GetHeight() const { return m_height; }
    ionBool IsFullscreen() const { return m_fullScreen; }
    ionBool IsCursorVisible() const { return m_showCursor; }

    CommandLineParser& GetCommandLineParse() { return m_commandLineParse; }

    const eosString& GetMacAddress() const { return m_macAddress; }

    static const char* s_physicalAddress;
    static ionU32 s_physicalAddressNumHigh;
    static ionU32 s_physicalAddressNumLow;

private:
    void MouseClick(ionU32 _indexButton, ionBool _state);
    void MouseMove(ionFloat _x, ionFloat _y);
    void MouseWheel(ionFloat _distance);
    void MouseReset();
    void GetMousePos(ionFloat& _xpos, ionFloat& _ypos);
    void CenterCursor();

    void KeyDown(ionU8 _char);
    void KeyUp(ionU8 _char);
    void KeyboardReset();

private:
    eosString           m_macAddress;
    ionU64              m_macAddressNum;
    MouseState          m_mouse;
    KeyboardState       m_keyboard;
    HINSTANCE           m_instance;
    HWND                m_handle;
    CommandLineParser   m_commandLineParse;
    ionS32              m_width;
    ionS32              m_height;
    ECursorMode         m_cursorMode;
    ionBool             m_fullScreen;
    ionBool             m_showCursor;
    ionBool             m_skipNextMouseMove;
};

ION_NAMESPACE_END