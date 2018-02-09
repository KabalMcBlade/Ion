#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

ION_NAMESPACE_BEGIN

class ION_DLL VulkanWrapper
{
public:
    VulkanWrapper() : m_vkInstance(VK_NULL_HANDLE), m_vkSurface(VK_NULL_HANDLE) {}

    ionBool    Init(HINSTANCE _instance, HWND _handle);
    void       Shutdown();

private:
    ionBool    InitInstance();
    ionBool    InitPresentationSurface(HINSTANCE _instance, HWND _handle);

private:
    VkSurfaceKHR        m_vkSurface;
    VkInstance          m_vkInstance;
};

ION_NAMESPACE_END