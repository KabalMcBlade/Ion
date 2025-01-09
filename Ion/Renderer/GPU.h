// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Renderer\GPU.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"
#include "../Core/MemoryWrapper.h"

#include "../Core/MemoryWrapper.h"

#include "../Core/MemorySettings.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using GPUAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


struct GPU final
{
	static GPUAllocator* GetAllocator();

    GPU();
    ~GPU();

    ionBool Set(const VkInstance& _vkInstance, const VkSurfaceKHR& _vkSurface, const VkPhysicalDevice& _vkDevice);

    VkPhysicalDevice                    m_vkPhysicalDevice;
    VkPhysicalDeviceProperties          m_vkPhysicalDeviceProps;
    VkPhysicalDeviceMemoryProperties    m_vkPhysicalDeviceMemoryProperties;
    VkPhysicalDeviceFeatures            m_vkPhysicalDevFeatures;
    VkSurfaceCapabilitiesKHR            m_vkSurfaceCaps;
    ionVector<VkSurfaceFormatKHR, GPUAllocator, GetAllocator>        m_vkSurfaceFormats;
    ionVector<VkPresentModeKHR, GPUAllocator, GetAllocator>            m_vkPresentModes;
    ionVector<VkQueueFamilyProperties, GPUAllocator, GetAllocator>    m_vkQueueFamilyProps;
    ionVector<VkExtensionProperties, GPUAllocator, GetAllocator>    m_vkExtensionProps;
};

ION_NAMESPACE_END