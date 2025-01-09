// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\GPU\GpuMemoryList.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include <vulkan/vulkan.h>

#include "GpuDataStructure.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


ION_MEMORY_ALIGNED class GpuMemoryList final
{
public:
	GpuMemoryList(const VkDevice _device, uint32 _memorytypeIndex, VkDeviceSize _size, EMemoryUsage _usage);
	~GpuMemoryList();

	bool Alloc(size _size, size _align, VkDeviceSize _granularity, EGpuMemoryType _gpuType, GpuMemoryAllocation& _gpuMemory);
	void Free(GpuMemoryAllocation& _gpuMemory);

	ION_INLINE bool IsHostVisible() const { return m_usage != EMemoryUsage_GPU; }

private:
	friend class GpuMemoryAllocator;

	// for granularity check link: https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html
	// and search for: Buffer-Image Granularity
	bool HasGranularityConflict(EGpuMemoryType _memoryType0, VkDeviceSize _offset0, VkDeviceSize _size0, EGpuMemoryType _memoryType1, VkDeviceSize _offset1, VkDeviceSize _pageSize);


	ION_MEMORY_ALIGNED struct GpuMemoryNode final
	{
		uint32              m_index;
		VkDeviceSize        m_size;
		VkDeviceSize        m_offset;
		EGpuMemoryType      m_gpuMemoryType;
	};

	using MemoryNode = typename ION_MEMORY_ALIGNED DoublyLinkedList<GpuMemoryNode>::Node;

	VkDevice m_device;
	DoublyLinkedList<GpuMemoryNode> m_nodeList;
	VkDeviceMemory m_deviceMemory;
	VkDeviceSize m_totalSize;
	VkDeviceSize m_allocated;
	EMemoryUsage m_usage;
	uint32 m_memoryTypeIndex;
	uint32 m_nextNodeIndex;
	uint8* m_memoryBuffer;
};


ION_NAMESPACE_END