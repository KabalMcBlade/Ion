// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\GPU\GpuMemoryManager.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include <vulkan/vulkan.h>

#include "VulkanAllocator.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

using GpuFreeListAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class ION_DLL GpuMemoryManager final : public NoCopyableMoveable
{
public:
	enum EMemoryType
	{
		EMemoryType_Vulkan,
		EMemoryType_GPU
	};

	static GpuMemoryManager& Instance();

	ION_INLINE const VkAllocationCallbacks* GetVK() const { return &m_allocator.GetCallbacks(); }

private:
	friend class GpuMemoryList;
	static GpuFreeListAllocator* GetAllocator();

	GpuMemoryManager() {}
	~GpuMemoryManager() {}

private:
	VulkanAllocator m_allocator;
};

#ifndef vkMemory
#define vkMemory GpuMemoryManager::Instance().GetVK()
#endif


ION_NAMESPACE_END