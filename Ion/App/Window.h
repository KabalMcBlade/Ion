#pragma once

#include <windows.h>

#include "../Core/CoreDefs.h"

#include "VulkanWrapper.h"


ION_NAMESPACE_BEGIN

class ION_DLL Window
{
public:
    Window();
    ~Window();

    ionBool        Create();
    ionBool        Loop();

private:
    HINSTANCE       m_instance;
    HWND            m_handle;
    VulkanWrapper   m_vulkan;
};

ION_NAMESPACE_END