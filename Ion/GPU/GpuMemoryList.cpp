#include "GpuMemoryList.h"

#include "GpuMemoryManager.h"

EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


GpuMemoryList::GpuMemoryList(const VkDevice _device, uint32 _memorytypeIndex, VkDeviceSize _size, EMemoryUsage _usage) :
	m_device(_device),
	m_totalSize(_size),
	m_allocated(0),
	m_memoryTypeIndex(_memorytypeIndex),
	m_usage(_usage),
	m_deviceMemory(VK_NULL_HANDLE),
	m_memoryBuffer(nullptr),
	m_nextNodeIndex(0)
{
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = m_totalSize;
	memoryAllocateInfo.memoryTypeIndex = m_memoryTypeIndex;

	VkResult result = vkAllocateMemory(m_device, &memoryAllocateInfo, GpuMemoryManager::Instance().GetVK(), &m_deviceMemory);
	ionAssertReturnVoid(result == VK_SUCCESS, "Vulkan Cannot allocate memory!");
	ionAssertReturnVoid(m_deviceMemory != VK_NULL_HANDLE, "Vulkan Device Memory not created!");

	if (IsHostVisible())
	{
		void* pData = nullptr;
		VkResult result = vkMapMemory(m_device, m_deviceMemory, 0, m_totalSize, 0, &pData);
		ionAssertReturnVoid(result == VK_SUCCESS, "Vulkan Cannot map memory!");

		m_memoryBuffer = static_cast<uint8*>(pData);
		ionAssertReturnVoid(m_memoryBuffer != nullptr, "Host Visible: Memory buffer not created!");
	}

	MemoryNode* head = eosNew(MemoryNode, GpuMemoryManager::GetAllocator());

	head->m_data.m_index = m_nextNodeIndex++;
	head->m_data.m_size = m_totalSize;
	head->m_data.m_offset = 0;
	head->m_data.m_gpuMemoryType = EGpuMemoryType_Free;

	head->m_prev = nullptr;
	head->m_next = nullptr;

	m_nodeList.SetHead(nullptr);
	m_nodeList.Push(head);
}


GpuMemoryList::~GpuMemoryList()
{
	if (IsHostVisible())
	{
		vkUnmapMemory(m_device, m_deviceMemory);
	}

	vkFreeMemory(m_device, m_deviceMemory, GpuMemoryManager::Instance().GetVK());
	m_deviceMemory = VK_NULL_HANDLE;

	
	MemoryNode* temp = nullptr;
	MemoryNode* current = m_nodeList.GetHead();

	while (current != nullptr)
	{
		temp = current->m_next;

		if (current->m_next != nullptr)
		{
			current->m_next->m_prev = current->m_prev;
		}

		if (current->m_prev != nullptr)
		{
			current->m_prev->m_next = current->m_next;
		}

		eosDelete(current, GpuMemoryManager::GetAllocator());

		current = temp;
	}
	
	m_nodeList.SetHead(nullptr);
}

bool GpuMemoryList::Alloc(size _size, size _align, VkDeviceSize _granularity, EGpuMemoryType _gpuType, GpuMemoryAllocation& _gpuMemory)
{
	const VkDeviceSize freeSize = m_totalSize - m_allocated;
	if (freeSize < _size)
	{
		_gpuMemory.m_result = VK_ERROR_OUT_OF_DEVICE_MEMORY;
		return false;
	}

	MemoryNode* head = m_nodeList.GetHead();
	MemoryNode* current = nullptr;
	MemoryNode* best = nullptr;
	MemoryNode* previous = nullptr;

	VkDeviceSize padding = 0;
	VkDeviceSize offset = 0;
	VkDeviceSize alignedSize = 0;

	for (current = head; current != nullptr; previous = current, current = current->m_next)
	{
		if (current->m_data.m_gpuMemoryType != EGpuMemoryType_Free)
		{
			continue;
		}

		if (_size > current->m_data.m_size)
		{
			continue;
		}


		size uiMask = _align - 1;
		offset = (current->m_data.m_offset + uiMask) & ~uiMask;

		if (previous != nullptr && _granularity > 1)
		{
			if (HasGranularityConflict(previous->m_data.m_gpuMemoryType, previous->m_data.m_offset, previous->m_data.m_size, _gpuType, offset, _granularity))
			{
				VkDeviceSize uiMask = _granularity - 1;
				offset = (offset + uiMask) & ~uiMask;
			}
		}

		padding = offset - current->m_data.m_offset;
		alignedSize = padding + _size;

		if (alignedSize > current->m_data.m_size)
		{
			continue;
		}

		if (alignedSize + m_allocated >= m_totalSize)
		{
			_gpuMemory.m_result = VK_ERROR_OUT_OF_DEVICE_MEMORY;
			return false;
		}

		if (_granularity > 1 && current->m_next != nullptr)
		{
			MemoryNode* next = current->m_next;
			if (HasGranularityConflict(_gpuType, offset, _size, next->m_data.m_gpuMemoryType, next->m_data.m_offset, _granularity))
			{
				continue;
			}
		}

		best = current;
		break;
	}

	if (best == nullptr)
	{
		_gpuMemory.m_result = VK_ERROR_INITIALIZATION_FAILED;
		return false;
	}

	if (best->m_data.m_size > _size)
	{
		MemoryNode* node = eosNew(MemoryNode, GpuMemoryManager::GetAllocator());
		MemoryNode* next = best->m_next;

		node->m_data.m_index = m_nextNodeIndex++;
		node->m_prev = best;
		best->m_next = node;

		node->m_next = next;
		if (next)
		{
			next->m_prev = node;
		}

		node->m_data.m_size = best->m_data.m_size - alignedSize;
		node->m_data.m_offset = offset + _size;
		node->m_data.m_gpuMemoryType = EGpuMemoryType_Free;
	}

	best->m_data.m_gpuMemoryType = _gpuType;
	best->m_data.m_size = _size;

	m_allocated += alignedSize;

	_gpuMemory.m_size = best->m_data.m_size;
	_gpuMemory.m_index = best->m_data.m_index;
	_gpuMemory.m_memory = m_deviceMemory;

	if (IsHostVisible())
	{
		_gpuMemory.m_mappedData = m_memoryBuffer + offset;
	}

	_gpuMemory.m_offset = offset;
	_gpuMemory.m_owner = this;
	_gpuMemory.m_result = VK_SUCCESS;

	return true;
}

void GpuMemoryList::Free(GpuMemoryAllocation& _gpuMemory)
{
	MemoryNode* head = m_nodeList.GetHead();
	MemoryNode* current = nullptr;
	for (current = head; current != nullptr; current = current->m_next)
	{
		if (current->m_data.m_index == _gpuMemory.m_index)
		{
			break;
		}
	}

	if (current == nullptr)
	{
		ionAssertReturnVoid(false, "Tried to free an unknown allocation");
		return;
	}

	current->m_data.m_gpuMemoryType = EGpuMemoryType_Free;

	if (current->m_prev && current->m_prev->m_data.m_gpuMemoryType == EGpuMemoryType_Free)
	{
		MemoryNode * prev = current->m_prev;

		prev->m_next = current->m_next;
		if (current->m_next)
		{
			current->m_next->m_prev = prev;
		}

		prev->m_data.m_size += current->m_data.m_size;

		eosDelete(current, GpuMemoryManager::GetAllocator());

		current = prev;
	}

	if (current->m_next && current->m_next->m_data.m_gpuMemoryType == EGpuMemoryType_Free)
	{
		MemoryNode* next = current->m_next;

		if (next->m_next)
		{
			next->m_next->m_prev = current;
		}

		current->m_next = next->m_next;

		current->m_data.m_size += next->m_data.m_size;

		eosDelete(next, GpuMemoryManager::GetAllocator());
	}

	m_allocated -= _gpuMemory.m_size;
}

bool GpuMemoryList::HasGranularityConflict(EGpuMemoryType _memoryType0, VkDeviceSize _offset0, VkDeviceSize _size0, EGpuMemoryType _memoryType1, VkDeviceSize _offset1, VkDeviceSize _pageSize)
{
	ionAssertReturnValue(_offset0 + _size0 <= _offset1 && _size0 > 0 && _pageSize > 0, false, "Out of bound!");

	VkDeviceSize end0 = _offset0 + _size0 - 1;
	VkDeviceSize endPage0 = end0 & ~(_pageSize - 1);
	VkDeviceSize start1 = _offset1;
	VkDeviceSize startPage1 = start1 & ~(_pageSize - 1);

	if (endPage0 == startPage1)
	{
		if (_memoryType0 > _memoryType1)
		{
			EGpuMemoryType c = _memoryType0;
			_memoryType0 = _memoryType1;
			_memoryType1 = c;
		}

		switch (_memoryType0)
		{
		case EGpuMemoryType_Free:
			return false;
		case EGpuMemoryType_Buffer:
			return    _memoryType1 == EGpuMemoryType_Image || _memoryType1 == EGpuMemoryType_ImageOptimal;
		case EGpuMemoryType_Image:
			return  _memoryType1 == EGpuMemoryType_Image || _memoryType1 == EGpuMemoryType_ImageLinear || _memoryType1 == EGpuMemoryType_ImageOptimal;
		case EGpuMemoryType_ImageLinear:
			return _memoryType1 == EGpuMemoryType_ImageOptimal;
		case EGpuMemoryType_ImageOptimal:
			return false;
		default:
			eosAssertReturnValue(false, false, "Format %d not recognized!", _memoryType0);
			return true;
		}
	}
	else
	{
		return false;
	}
}

ION_NAMESPACE_END