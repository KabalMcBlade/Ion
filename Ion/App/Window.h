#pragma once

#include <windows.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

class ION_DLL Window
{
public:
    Window();
    ~Window();

    ionBool        Create(WNDPROC _wndproc, const eosTString& _name, ionU32 _width, ionU32 _height, ionBool _fullScreen);
    ionBool        Loop();

private:
    eosTString      m_name;
    HINSTANCE       m_instance;
    HWND            m_handle;
    ionU32          m_width;
    ionU32          m_height;
    ionBool         m_fullScreen;
};

ION_NAMESPACE_END