#pragma once

#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemorySettings.h"



EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


using VulkanFreeListAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class ION_DLL VulkanAllocator final : public NoCopyableMoveable
{
public:
	VulkanAllocator() 
	{
		m_callback.pUserData = (void*)this;
		m_callback.pfnAllocation = &VulkanAllocator::Allocation;
		m_callback.pfnReallocation = &VulkanAllocator::Reallocation;
		m_callback.pfnFree = &VulkanAllocator::Free;
		m_callback.pfnInternalAllocation = nullptr;
		m_callback.pfnInternalFree = nullptr;
	}

	~VulkanAllocator() {}

	ION_INLINE const VkAllocationCallbacks& GetCallbacks() const { return m_callback; }


private:
	static void* VKAPI_CALL Allocation(void* _pUserData, size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	static void* VKAPI_CALL Reallocation(void* _pUserData, void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	static void VKAPI_CALL Free(void* _pUserData, void* _pMemory);

	void* Allocation(size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	void* Reallocation(void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	void Free(void* _pMemory);

private:
	VkAllocationCallbacks m_callback;

private:
	static VulkanFreeListAllocator* GetAllocator();
};


ION_NAMESPACE_END