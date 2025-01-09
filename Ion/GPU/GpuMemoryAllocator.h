// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\GPU\GpuMemoryAllocator.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include <vulkan/vulkan.h>

#include "GpuMemoryList.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


using GpuNodeListFreeListAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class ION_DLL GpuMemoryAllocator final
{
public:
	static GpuMemoryAllocator& Instance();

	GpuMemoryAllocator();
	~GpuMemoryAllocator();

	void Init(const VkPhysicalDevice& _physicalDevice, const VkDevice& _device, size _granularity);
	void Shutdown();

	GpuMemoryAllocation Alloc(const GpuMemoryCreateInfo& _createInfo);
	void Free(GpuMemoryAllocation& _gpuMemory);


	static uint32 FindMemoryType(const GpuMemoryCreateInfo& _memoryCreateInfo);

private:
	static uint32 CountBitsSet(uint32 _value);

private:
	static GpuNodeListFreeListAllocator* GetAllocator();

	static VkPhysicalDeviceMemoryProperties m_memProperties;

	Vector<GpuMemoryList*, GpuNodeListFreeListAllocator, GetAllocator> m_gpuMemory[VK_MAX_MEMORY_TYPES];
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	size m_granularity;
};

ION_NAMESPACE_END

#define ionGPUMemoryManager() ion::GpuMemoryAllocator::Instance()