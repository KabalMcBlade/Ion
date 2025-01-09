// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\GPU\VulkanAllocator.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "VulkanAllocator.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


VulkanFreeListAllocator* VulkanAllocator::GetAllocator()
{
	static HeapArea<Settings::kVulkanAllocatorSize> memoryArea;
	static VulkanFreeListAllocator memoryAllocator(memoryArea, "VulkanFreeListAllocator");

	return &memoryAllocator;
}


void* VulkanAllocator::Allocation(size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return eosNewAlignedRaw(_size, GetAllocator(), _alignment);
}

void* VKAPI_CALL VulkanAllocator::Allocation(void* _pUserData, size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Allocation(_size, _alignment, _allocationScope);
}

void* VulkanAllocator::Reallocation(void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return eosReallocAlignedRaw(_pOriginal, _size, GetAllocator(), _alignment);
}

void* VKAPI_CALL VulkanAllocator::Reallocation(void* _pUserData, void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Reallocation(_pOriginal, _size, _alignment, _allocationScope);
}

void VulkanAllocator::Free(void* _pMemory)
{
	if (_pMemory == nullptr) // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/PFN_vkFreeFunction.html (may be null and it would be safe anyway!)
	{
		return;
	}

	eosDeleteRaw(_pMemory, GetAllocator());
}

void VKAPI_CALL VulkanAllocator::Free(void* _pUserData, void* _pMemory)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Free(_pMemory);
}


ION_NAMESPACE_END