#include "UniformBufferObject.h"

#include "GPUMemoryManager.h"
#include "StagingBufferManager.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

ION_NAMESPACE_BEGIN


UniformBuffer::UniformBuffer()
{
    //m_usage = EBufferUsage_Dynamic;
    SetUnmapped();
}

UniformBuffer::~UniformBuffer()
{
    Free(); //  need it in this case ?
}

ionBool UniformBuffer::Alloc(const VkDevice& _device, const void* _data, ionSize _allocSize, EBufferUsage _usage)
{
    ionAssertReturnValue(m_object == VK_NULL_HANDLE, "Buffer already allocated", false);
    ionAssertReturnValue(ION_IS_ALIGNED(_data, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16", false);
    ionAssertReturnValue(_allocSize > 0, "Size must be greeter than 0", false);

    m_size = _allocSize;
    m_usage = _usage;
    m_device = _device;

    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.size = GetAllocedSize();
    createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_usage == EBufferUsage_Static)
    {
        createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    VkResult result = vkCreateBuffer(m_device, &createInfo, vkMemory, &m_object);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_device, m_object, &memoryRequirements);

    {
        vkGpuMemoryCreateInfo createInfo = {};
        createInfo.m_size = memoryRequirements.size;
        createInfo.m_align = memoryRequirements.alignment;
        createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
        createInfo.m_usage = (m_usage == EBufferUsage_Static) ? EMemoryUsage_GPU : EMemoryUsage_CPU_to_GPU;
        createInfo.m_type = EGpuMemoryType_Buffer;

        m_allocation = ionGPUMemoryManager().Alloc(createInfo);
        ionAssertReturnValue(m_allocation.m_result == VK_SUCCESS, "Cannot Allocate memory!", false);

        result = vkBindBufferMemory(m_device, m_object, m_allocation.m_memory, m_allocation.m_offset);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot bind the image memory!", false);
    }

    if (_data != nullptr)
    {
        Update(_data, _allocSize);
    }

    return true;
}

void UniformBuffer::Free()
{
    if (IsMapped())
    {
        UnmapBuffer();
    }

    if (!OwnsBuffer())
    {
        ClearWithoutFreeing();
        return;
    }

    if (m_object == VK_NULL_HANDLE)
    {
        return;
    }

    if (m_object != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(m_device, m_object, vkMemory);
        ionGPUMemoryManager().Free(m_allocation);

        m_object = VK_NULL_HANDLE;
        m_allocation = vkGpuMemoryAllocation();
    }

    ClearWithoutFreeing();
}

void UniformBuffer::ReferenceTo(const UniformBuffer& _other)
{
    ionAssertReturnVoid(!IsMapped(), "Buffer must be not mapped");
    ionAssertReturnVoid(_other.GetSize() > 0, "The other buffer must greater than 0");

    Free();
    m_size = _other.GetSize();
    m_offsetInOtherBuffer = _other.GetOffset();
    m_device = _other.m_device;
    m_usage = _other.m_usage;
    m_object = _other.m_object;
    m_allocation = _other.m_allocation;

    ionAssertReturnVoid(!OwnsBuffer(), "Should not own this buffer now!");
}

void UniformBuffer::ReferenceTo(const UniformBuffer& _other, ionSize _refOffset, ionSize _refSize)
{
    ionAssertReturnVoid(!IsMapped(), "Buffer must be not mapped");
    ionAssertReturnVoid(_refOffset >= 0, "Offset must be greater or equal to 0");
    ionAssertReturnVoid(_refSize >= 0, "Size must be greater or equal to 0");
    ionAssertReturnVoid(_refOffset + _refSize <= _other.GetSize(), "Size plus offset must be less or equal to other size");

    Free();
    m_size = _refSize;
    m_offsetInOtherBuffer = _other.GetOffset() + _refOffset;
    m_device = _other.m_device;
    m_usage = _other.m_usage;
    m_object = _other.m_object;
    m_allocation = _other.m_allocation;

    ionAssertReturnVoid(!OwnsBuffer(), "Should not own this buffer now!");
}

void UniformBuffer::Update(const void* _data, ionSize _size, ionSize _offset /*= 0*/) const
{
    ionAssertReturnVoid(m_object != VK_NULL_HANDLE, "Buffer must be allocated");
    ionAssertReturnVoid(ION_IS_ALIGNED(_data, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
    ionAssertReturnVoid((GetOffset() & 15) == 0, "Offset not aligned to 16");
    ionAssertReturnVoid(_size < GetSize(), "Size must be less of the total size than 0");

    if (m_usage == EBufferUsage_Dynamic)
    {
        ionAssertReturnVoid(ION_IS_ALIGNED(m_allocation.m_mappedData + GetOffset() + _offset, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
        ionAssertReturnVoid(ION_IS_ALIGNED((const ionU8*)_data, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
        CopyBuffer(m_allocation.m_mappedData + GetOffset() + _offset, (const ionU8*)_data, _size);
    }
    else
    {
        VkBuffer stageBuffer;
        VkCommandBuffer commandBuffer;
        ionSize stageOffset = 0;
        ionU8* stageData = ionStagingBufferManager().Stage(_size, ION_MEMORY_ALIGNMENT_SIZE, commandBuffer, stageBuffer, stageOffset);

        CopyBuffer(stageData, (const ionU8*)_data, _size);

        VkBufferCopy bufferCopy = {};
        bufferCopy.srcOffset = stageOffset;
        bufferCopy.dstOffset = GetOffset() + _offset;
        bufferCopy.size = _size;

        vkCmdCopyBuffer(commandBuffer, stageBuffer, m_object, 1, &bufferCopy);
    }
}

void* UniformBuffer::MapBuffer(EBufferMappingType _mapType)
{
    return MapBuffer(_mapType, 0);
}

void* UniformBuffer::MapBuffer(EBufferMappingType _mapType, ionSize _offset)
{
    ionAssertReturnValue(m_object != VK_NULL_HANDLE, "Buffer must be allocated", nullptr);
    ionAssertReturnValue(m_usage != EBufferUsage_Static, "Cannot map static buffer", nullptr);

    void* buffer = m_allocation.m_mappedData + GetOffset() + _offset;

    SetMapped();

    ionAssertReturnValue(buffer != nullptr, "Failed to map the buffer", nullptr);

    return buffer;
}

void UniformBuffer::UnmapBuffer()
{
    ionAssertReturnVoid(m_object != VK_NULL_HANDLE, "Buffer must be allocated");
    ionAssertReturnVoid(m_usage != EBufferUsage_Static, "Cannot unmap static buffer");

    SetUnmapped();
}

void UniformBuffer::ClearWithoutFreeing()
{
    m_size = 0;
    m_offsetInOtherBuffer = ION_BUFFER_OBJECT_MAPPED_FLAG;
    m_object = VK_NULL_HANDLE;
    m_allocation.m_memory = VK_NULL_HANDLE;
}


ION_NAMESPACE_END
