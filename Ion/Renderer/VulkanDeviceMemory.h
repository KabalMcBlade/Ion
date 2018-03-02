#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/CoreDefs.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN


enum EVulkanDeviceMemoryLocation
{
    EVulkanDeviceMemoryLocation_DeviceLocal,
    EVulkanDeviceMemoryLocation_HostVisible
};


enum EVulkanDeviceMemoryType
{
    EVulkanDeviceMemoryType_StaticImages,
    EVulkanDeviceMemoryType_StaticBuffers,
    EVulkanDeviceMemoryType_StaticStagingBuffers,

    EVulkanDeviceMemoryType_ResolutionDependentImages,
    EVulkanDeviceMemoryType_ResolutionDependentBuffers,
    EVulkanDeviceMemoryType_ResolutionDependentStagingBuffers,

    EVulkanDeviceMemoryType_VolatileStagingBuffers,

    EVulkanDeviceMemoryType_Count,

    EVulkanDeviceMemoryType_RangeStartStatic = EVulkanDeviceMemoryType_StaticImages,
    EVulkanDeviceMemoryType_RangeEndStatic = EVulkanDeviceMemoryType_StaticStagingBuffers,
    EVulkanDeviceMemoryType_RangeStartResolutionDependent = EVulkanDeviceMemoryType_ResolutionDependentImages,
    EVulkanDeviceMemoryType_RangeEndResolutionDependent = EVulkanDeviceMemoryType_ResolutionDependentStagingBuffers,
    EVulkanDeviceMemoryType_RangeStartVolatile = EVulkanDeviceMemoryType_VolatileStagingBuffers,
    EVulkanDeviceMemoryType_RangeEndVolatile = EVulkanDeviceMemoryType_VolatileStagingBuffers
};


class ION_DLL VulkanDeviceMemory
{
public:
    ION_INLINE ionU8* GetHostVisibleMemoryForOffset(ionU32 _offset)
    {
        return &m_mappedHostVisibleMemory[_offset];
    }

    ION_INLINE ionU8* GetMappedHostVisibleMemoryBuffer()
    {
        return m_mappedHostVisibleMemory;
    }

    ION_INLINE void** GetMappapleHostVisibleMemoryBuffer()
    {
        return reinterpret_cast<void**>(&m_mappedHostVisibleMemory);
    }

    ION_INLINE VkDeviceMemory& GetDeviceLocalMemory()
    {
        return m_vkDeviceLocalMemory;
    }

    ION_INLINE VkDeviceMemory& GetHostVisibleMemory()
    {
        return m_vkHostVisibleMemory;
    }

    ION_INLINE ionU32 GetMemoryPoolSizesInBytes(EVulkanDeviceMemoryType _type) const
    {
        return m_memoryPoolSizesInBytes[_type];
    }
    
    ION_INLINE EVulkanDeviceMemoryLocation GetMemoryPoolUsages(EVulkanDeviceMemoryType _type) const
    {
        return m_memoryPoolUsages[_type];
    }

    ION_INLINE VkDeviceMemory GetDeviceMemory(EVulkanDeviceMemoryType _type)
    {
        return m_memoryPoolUsages[_type] == EVulkanDeviceMemoryLocation_DeviceLocal ? m_vkDeviceLocalMemory : m_vkHostVisibleMemory;
    }

    //////////////////////////////////////////////////////////////////////////
    








    ION_INLINE void InitDeviceOffsetMemory(EVulkanDeviceMemoryType _type, ionU32 _size, ionU32 _initialOffset = 0)
    {
        m_deviceMemoryAllocators[_type].Init(_size, _initialOffset);
    }

    ION_INLINE void ResetDeviceOffsetMemory(EVulkanDeviceMemoryType _type)
    {
        m_deviceMemoryAllocators[_type].Reset();
    }

    ION_INLINE ionU32 AllocateDeviceOffsetMemory(EVulkanDeviceMemoryType _type, ionU32 _size, ionU32 _alignment)
    {
        return m_deviceMemoryAllocators[_type].Allocate(_size, _alignment);
    }

    ION_INLINE ionU32 GetSizeDeviceOffsetMemory(EVulkanDeviceMemoryType _type) const
    {
        return m_deviceMemoryAllocators[_type].Size();
    }

    ION_INLINE ionU32 GetCurrentOffsetDeviceOffsetMemory(EVulkanDeviceMemoryType _type) const
    {
        return m_deviceMemoryAllocators[_type].CurrentOffset();
    }
    
    ION_INLINE ionU32 CalcAvailableMemoryInBytesDeviceOffsetMemory(EVulkanDeviceMemoryType _type) const
    {
        return m_deviceMemoryAllocators[_type].CalcAvailableMemoryInBytes();
    }
  
private:
    struct OffsetAllocator
    {
        ION_INLINE OffsetAllocator() : m_sizeInBytes(0), m_currentOffsetInBytes(0), m_initialOffset(0)
        {

        }

        ION_INLINE void Init(ionU32 _size, ionU32 _initialOffset = 0)
        {
            m_initialOffset = _initialOffset;
            m_currentOffsetInBytes = m_initialOffset;
            m_sizeInBytes = _size;
        }

        ION_INLINE ionU32 Allocate(ionU32 _size, ionU32 _alignment)
        {
            eosSize uiMask = _alignment - 1;
            eosSize newOffset = (m_currentOffsetInBytes + uiMask) & ~uiMask;
            m_currentOffsetInBytes = newOffset + _size;
            ionAssertReturnValue((m_currentOffsetInBytes - m_initialOffset) <= m_sizeInBytes, "Out of memory", 0);
            return newOffset;
        }

        ION_INLINE void Reset()
        {
            m_currentOffsetInBytes = m_initialOffset;
        }

        ION_INLINE ionU32 Size() const
        {
            return m_sizeInBytes;
        }

        ION_INLINE ionU32 CurrentOffset() const
        {
            return m_currentOffsetInBytes;
        }

        ION_INLINE ionU32 CalcAvailableMemoryInBytes() const
        {
            return m_sizeInBytes - (m_currentOffsetInBytes - m_initialOffset);
        }

    private:
        ionU32 m_initialOffset;
        ionU32 m_sizeInBytes;
        ionU32 m_currentOffsetInBytes;
    };
    OffsetAllocator m_deviceMemoryAllocators[EVulkanDeviceMemoryType_Count];


    VkDeviceMemory m_vkDeviceLocalMemory;
    VkDeviceMemory m_vkHostVisibleMemory;

    ionU8* m_mappedHostVisibleMemory;

    ionU32 m_memoryPoolSizesInBytes[EVulkanDeviceMemoryType_Count];
    EVulkanDeviceMemoryLocation m_memoryPoolUsages[EVulkanDeviceMemoryType_Count];
    //////////////////////////////////////////////////////////////////////////


private:
    VulkanDeviceMemory();
    ~VulkanDeviceMemory();

    //eosHeapAllocator        m_heapAllocator;

};


ION_NAMESPACE_END