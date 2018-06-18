#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

struct GPU final
{
    GPU();
    ~GPU();

    ionBool Set(const VkInstance& _vkInstance, const VkSurfaceKHR& _vkSurface, const VkPhysicalDevice& _vkDevice);

    VkPhysicalDevice                    m_vkPhysicalDevice;
    VkPhysicalDeviceProperties          m_vkPhysicalDeviceProps;
    VkPhysicalDeviceMemoryProperties    m_vkPhysicalDeviceMemoryProperties;
    VkPhysicalDeviceFeatures            m_vkPhysicalDevFeatures;
    VkSurfaceCapabilitiesKHR            m_vkSurfaceCaps;
    eosVector(VkSurfaceFormatKHR)        m_vkSurfaceFormats;
    eosVector(VkPresentModeKHR)            m_vkPresentModes;
    eosVector(VkQueueFamilyProperties)    m_vkQueueFamilyProps;
    eosVector(VkExtensionProperties)    m_vkExtensionProps;
};

ION_NAMESPACE_END