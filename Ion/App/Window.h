#pragma once

#include <windows.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"


#include "VulkanWrapper.h"

EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

class ION_DLL Window
{
public:
    Window();
    ~Window();

    ionBool        Create(WNDPROC _wndproc, const eosTString& _name);
    ionBool        Loop();

private:
    eosTString      m_name;
    HINSTANCE       m_instance;
    HWND            m_handle;
    VulkanWrapper   m_vulkan;
};

ION_NAMESPACE_END