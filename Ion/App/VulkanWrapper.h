#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


class VulkanWrapper
{
public:
    VulkanWrapper() : m_vkInstance(VK_NULL_HANDLE), m_vkSurface(VK_NULL_HANDLE) {}

    bool    Init(HINSTANCE _instance, HWND _handle);
    void    Shutdown();

private:
    bool    InitInstance();
    bool    InitPresentationSurface(HINSTANCE _instance, HWND _handle);

private:
    VkSurfaceKHR        m_vkSurface;
    VkInstance          m_vkInstance;
};