// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\GPU\GpuAllocator.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "GPUMemoryManager.h"


#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemorySettings.h"

EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


ION_INLINE VkDeviceMemory vkGpuAllocateMemory(const VkDevice& _vkDevice, const VkMemoryAllocateInfo& _allocateInfo)
{
	VkDeviceMemory deviceMemoryAllocated = VK_NULL_HANDLE;

	VkResult result = vkAllocateMemory(_vkDevice, &_allocateInfo, vkMemory, &deviceMemoryAllocated);
	ionAssertReturnValue(result == VK_SUCCESS, "Cannot allocate memory!", VK_NULL_HANDLE);

	return deviceMemoryAllocated;
}

ION_INLINE void* vkGpuMapMemory(const VkDevice& _vkDevice, const VkDeviceMemory& _vkDeviceMemoryAllocated, VkDeviceSize _vkOffset, VkDeviceSize _vkSize, VkMemoryMapFlags _vkFlags)
{
	void* pData = nullptr;

	VkResult result = vkMapMemory(_vkDevice, _vkDeviceMemoryAllocated, _vkOffset, _vkSize, _vkFlags, &pData);
	ionAssertReturnValue(result == VK_SUCCESS, "Cannot map memory!", nullptr);

	return pData;
}

ION_INLINE void vkGpuUnmapMemory(const VkDevice& _vkDevice, const VkDeviceMemory& _vkDeviceMemoryAllocated)
{
	vkUnmapMemory(_vkDevice, _vkDeviceMemoryAllocated);
}


ION_INLINE void vkGpuFreeMemory(const VkDevice& _vkDevice, VkDeviceMemory& _vkDeviceMemoryAllocated)
{
	vkFreeMemory(_vkDevice, _vkDeviceMemoryAllocated, vkMemory);
	_vkDeviceMemoryAllocated = VK_NULL_HANDLE;
}


ION_NAMESPACE_END