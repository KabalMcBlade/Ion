#include "GpuMemoryAllocator.h"

EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


VkPhysicalDeviceMemoryProperties GpuMemoryAllocator::m_memProperties;


GpuNodeListFreeListAllocator* GpuMemoryAllocator::GetAllocator()
{
	static HeapArea<Settings::kGPUListNodeAllocatorSize> memoryArea;
	static GpuNodeListFreeListAllocator memoryAllocator(memoryArea, "GPUNodeListFreeListAllocator");

	return &memoryAllocator;
}

GpuMemoryAllocator& GpuMemoryAllocator::Instance()
{
	static GpuMemoryAllocator instance;
	return instance;
}


GpuMemoryAllocator::GpuMemoryAllocator()
{
}

GpuMemoryAllocator::~GpuMemoryAllocator()
{
}

void GpuMemoryAllocator::Init(const VkPhysicalDevice& _physicalDevice, const VkDevice& _device, size _granularity)
{
	m_physicalDevice = _physicalDevice;
	m_device = _device;

	m_granularity = _granularity;

	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memProperties);
}

void GpuMemoryAllocator::Shutdown()
{
	for (uint32 i = 0; i < VK_MAX_MEMORY_TYPES; ++i)
	{
		Vector<GpuMemoryList*, GpuNodeListFreeListAllocator, GetAllocator>& memoryList = m_gpuMemory[i];
		size listSize = memoryList.size();
		for (size j = 0; j < listSize; ++j)
		{
			eosDelete(memoryList[j], GetAllocator());
		}

		memoryList.clear();
	}
}


GpuMemoryAllocation GpuMemoryAllocator::Alloc(const GpuMemoryCreateInfo& _createInfo)
{
	GpuMemoryAllocation allocation;
	allocation.m_result = VK_ERROR_INITIALIZATION_FAILED;

	uint32 memoryTypeIndex = FindMemoryType(_createInfo);

	eosAssertReturnValue(memoryTypeIndex != UINT32_MAX, allocation, "Unable to allocate memory for the create info passed.");

	Vector<GpuMemoryList*, GpuNodeListFreeListAllocator, GetAllocator>& memoryList = m_gpuMemory[memoryTypeIndex];

	size count = memoryList.size();
	for (size i = 0; i < count; ++i)
	{
		GpuMemoryList* node = memoryList[i];
		
		// this class is friend of GpuMemoryList!
		if (node->m_memoryTypeIndex != memoryTypeIndex)
		{
			continue;
		}

		if (node->Alloc(_createInfo.m_size, _createInfo.m_align, m_granularity, _createInfo.m_type, allocation))
		{
			return allocation;
		}
	}

	VkDeviceSize logicSize = (_createInfo.m_usage == EMemoryUsage_GPU) ? Settings::kGpuDeviceLocalSize : Settings::kGpuHostVisibleSize;

	GpuMemoryList* list = eosNew(GpuMemoryList, GetAllocator(), m_device, memoryTypeIndex, logicSize, _createInfo.m_usage);

	memoryList.push_back(list);
	list->Alloc(_createInfo.m_size, _createInfo.m_align, m_granularity, _createInfo.m_type, allocation);

	return allocation;
}

void GpuMemoryAllocator::Free(GpuMemoryAllocation& _gpuMemory)
{
	_gpuMemory.m_owner->Free(_gpuMemory);

	if (_gpuMemory.m_owner->m_allocated == 0)
	{
		Vector<GpuMemoryList*, GpuNodeListFreeListAllocator, GetAllocator>& memoryList = m_gpuMemory[_gpuMemory.m_owner->m_memoryTypeIndex];
		memoryList.erase(std::remove(memoryList.begin(), memoryList.end(), _gpuMemory.m_owner), memoryList.end());

		eosDelete(_gpuMemory.m_owner, GetAllocator());

		_gpuMemory.m_owner = nullptr;
	}
}

uint32 GpuMemoryAllocator::FindMemoryType(const GpuMemoryCreateInfo& _memoryCreateInfo)
{
	uint32 memoryTypeIndex = UINT32_MAX;

	uint32 requiredFlags = 0;
	uint32 preferredFlags = 0;

	switch (_memoryCreateInfo.m_usage)
	{
	case EMemoryUsage_Unknown:
		break;
	case EMemoryUsage_GPU:
		preferredFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case EMemoryUsage_CPU:
		requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case EMemoryUsage_CPU_to_GPU:
		requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferredFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case EMemoryUsage_GPU_to_CPU:
		requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		break;
	}

	uint32 minBitCost = UINT32_MAX;
	for (uint32 memTypeIndex = 0, memTypeBit = 1; memTypeIndex < m_memProperties.memoryTypeCount; ++memTypeIndex, memTypeBit <<= 1)
	{
		if ((memTypeBit & _memoryCreateInfo.m_memoryTypeBits) != 0)
		{
			const VkMemoryPropertyFlags currFlags = m_memProperties.memoryTypes[memTypeIndex].propertyFlags;

			if ((requiredFlags & ~currFlags) == 0)
			{
				uint32 bitCost = CountBitsSet(preferredFlags & ~currFlags);

				if (bitCost < minBitCost)
				{
					memoryTypeIndex = memTypeIndex;
					if (bitCost == 0)
					{
						break;
					}
					minBitCost = bitCost;
				}
			}
		}
	}
	return memoryTypeIndex;
}

uint32 GpuMemoryAllocator::CountBitsSet(uint32 _value)
{
	uint32 bit = _value - ((_value >> 1) & 0x55555555);
	bit = ((bit >> 2) & 0x33333333) + (bit & 0x33333333);
	bit = ((bit >> 4) + bit) & 0x0F0F0F0F;
	bit = ((bit >> 8) + bit) & 0x00FF00FF;
	bit = ((bit >> 16) + bit) & 0x0000FFFF;
	return bit;
}


ION_NAMESPACE_END