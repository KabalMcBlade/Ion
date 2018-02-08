#pragma once

#include <windows.h>
#include "VulkanWrapper.h"

class Window
{
public:
    Window();
    ~Window();

    bool            Create();
    bool            Loop();

private:
    HINSTANCE       m_instance;
    HWND            m_handle;
    VulkanWrapper   m_vulkan;
};