#include "VulkanWrapper.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"
#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_NAME     "Ion"

VK_ALLOCATOR_USING_NAMESPACE
EOS_USING_NAMESPACE

bool VulkanWrapper::InitPresentationSurface(HINSTANCE _instance, HWND _handle)
{
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.hinstance = _instance;
    surfaceCreateInfo.hwnd = _handle;

    return (vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceCreateInfo, vkMemory, &m_vkSurface) == VK_SUCCESS);
}

bool VulkanWrapper::InitInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = VK_NAME;
    appInfo.pEngineName = VK_NAME;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    eosVector(const char*) enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };   // VK_EXT_DEBUG_REPORT_EXTENSION_NAME

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 2;
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, vkMemory, &m_vkInstance);

    return (result == VK_SUCCESS);
}

bool VulkanWrapper::Init(HINSTANCE _instance, HWND _handle)
{
    if (!InitInstance())
    {
        return false;
    }

    if (!InitPresentationSurface(_instance, _handle))
    {
        return false;
    }

    return true;
}

void VulkanWrapper::Shutdown()
{
    if (m_vkSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, vkMemory);
    }

    if (m_vkInstance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_vkInstance, vkMemory);
    }
}