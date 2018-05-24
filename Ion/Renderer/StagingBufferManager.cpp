#include "StagingBufferManager.h"


#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN


StagingBufferManager *StagingBufferManager::s_instance = nullptr;


StagingBufferManager::StagingBufferManager() :
    m_maxBufferSize(0),
    m_currentBuffer(0),
    m_vkGraphicsFamilyIndex(-1),
    m_mappedData(nullptr),
    m_vkDevice(VK_NULL_HANDLE),
    m_vkGraphicsQueue(VK_NULL_HANDLE),
    m_vkMemory(VK_NULL_HANDLE),
    m_vkCommandPool(VK_NULL_HANDLE)
{
}

StagingBufferManager::~StagingBufferManager()
{
    m_buffers.clear();
}

ionBool StagingBufferManager::Init(ionSize _vkMaxBufferSize, VkDevice _vkDevice, VkQueue _vkGraphicsQueue, ionS32 _vkGraphicsFamilyIndex)
{
    m_maxBufferSize = _vkMaxBufferSize;
    m_vkDevice = _vkDevice;
    m_vkGraphicsQueue = _vkGraphicsQueue;
    m_vkGraphicsFamilyIndex = _vkGraphicsFamilyIndex;
    m_buffers.resize(ION_FRAME_DATA_COUNT);

    VkResult result;

    {
        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = m_maxBufferSize;
        createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        for (ionU32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
        {
            m_buffers[i].m_vkOffset = 0;

            result = vkCreateBuffer(m_vkDevice, &createInfo, vkMemory, &m_buffers[i].m_vkBuffer);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create buffer for staging!", false);
        }
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_vkDevice, m_buffers[0].m_vkBuffer, &memoryRequirements);

    VkDeviceSize uiMask = memoryRequirements.alignment - 1;
    VkDeviceSize uiSize = (memoryRequirements.size + uiMask) & ~uiMask;

    vkGpuMemoryCreateInfo createInfo = {};
    createInfo.m_size = memoryRequirements.size;
    createInfo.m_align = memoryRequirements.alignment;
    createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
    createInfo.m_usage = EMemoryUsage_CPU_to_GPU;
    createInfo.m_type = EGpuMemoryType_Buffer;

    {
        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = uiSize * ION_FRAME_DATA_COUNT;
        allocateInfo.memoryTypeIndex = vkGpuMemoryAllocator::FindMemoryType(createInfo);

        m_vkMemory = vkGpuAllocateMemory(m_vkDevice, allocateInfo);
    }

    if (m_vkMemory == VK_NULL_HANDLE)
    {
        return false;
    }

    for (ionU32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
    {
        result = vkBindBufferMemory(m_vkDevice, m_buffers[i].m_vkBuffer, m_vkMemory, i * uiSize);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot bind buffer for staging!", false);
    }

    m_mappedData = static_cast<vkaU8*>(vkGpuMapMemory(m_vkDevice, m_vkMemory, 0, uiSize * ION_FRAME_DATA_COUNT, 0));

    if (m_mappedData == nullptr)
    {
        return false;
    }

    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = m_vkGraphicsFamilyIndex;

        result = vkCreateCommandPool(m_vkDevice, &createInfo, vkMemory, &m_vkCommandPool);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create command pool for staging!", false);
    }

    {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = m_vkCommandPool;
        allocateInfo.commandBufferCount = 1;

        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        for (ionU32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
        {
            result = vkAllocateCommandBuffers(m_vkDevice, &allocateInfo, &m_buffers[i].m_vkCommandBuffer);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot allocate command buffer for staging!", false);

            result = vkCreateFence(m_vkDevice, &createInfo, vkMemory, &m_buffers[i].m_vkFence);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create fence for staging!", false);

            result = vkBeginCommandBuffer(m_buffers[i].m_vkCommandBuffer, &commandBufferBeginInfo);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot begin command buffer for staging!", false);

            m_buffers[i].m_data = (ionU8*)m_mappedData + (i * uiSize);
        }
    }
   
    return true;
}

void StagingBufferManager::Shutdown()
{
    vkGpuUnmapMemory(m_vkDevice, m_vkMemory);
    vkGpuFreeMemory(m_vkDevice, m_vkMemory);

    m_mappedData = NULL;

    for (ionU32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
    {
        vkDestroyFence(m_vkDevice, m_buffers[i].m_vkFence, vkMemory);
        vkDestroyBuffer(m_vkDevice, m_buffers[i].m_vkBuffer, vkMemory);

        vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, 1, &m_buffers[i].m_vkCommandBuffer);
    }

    vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, vkMemory);

    m_buffers.clear();

    m_maxBufferSize = 0;
    m_currentBuffer = 0;
}

void StagingBufferManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(StagingBufferManager, EOS_MEMORY_ALIGNMENT_SIZE);
    }
}

void StagingBufferManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

StagingBufferManager& StagingBufferManager::Instance()
{
    return *s_instance;
}


ionU8* StagingBufferManager::Stage(ionSize _size, ionSize _alignment, VkCommandBuffer& _outVkCommandBuffer, VkBuffer& _outVkBuffer, ionSize& _outVkBufferOffset)
{
    ionAssertReturnValue(_size < m_maxBufferSize, "Size is outbound of total memory!", nullptr);

    StagingBuffer& stagingBuffer = m_buffers[m_currentBuffer];

    VkDeviceSize uiMask = _alignment - 1;
    VkDeviceSize uiSize = (stagingBuffer.m_vkOffset + uiMask) & ~uiMask;

    stagingBuffer.m_vkOffset = uiSize;

    if ((stagingBuffer.m_vkOffset + _size) >= (m_maxBufferSize) && !stagingBuffer.m_submitted)
    {
        Submit();
    }

    stagingBuffer = m_buffers[m_currentBuffer];
    if (stagingBuffer.m_submitted) 
    {
        VkResult result = vkWaitForFences(m_vkDevice, 1, &stagingBuffer.m_vkFence, VK_TRUE, UINT64_MAX);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot wait fences for staging!", nullptr);

        result = vkResetFences(m_vkDevice, 1, &stagingBuffer.m_vkFence);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot reset fences for staging!", nullptr);

        stagingBuffer.m_vkOffset = 0;
        stagingBuffer.m_submitted = false;

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        result = vkBeginCommandBuffer(stagingBuffer.m_vkCommandBuffer, &commandBufferBeginInfo);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot begin command buffer for staging!", nullptr);
    }

    _outVkCommandBuffer = stagingBuffer.m_vkCommandBuffer;
    _outVkBuffer = stagingBuffer.m_vkBuffer;
    _outVkBufferOffset = stagingBuffer.m_vkOffset;

    ionU8* data = stagingBuffer.m_data + stagingBuffer.m_vkOffset;
    stagingBuffer.m_vkOffset += _size;

    return data;
}

void StagingBufferManager::Submit()
{
    StagingBuffer& stagingBuffer = m_buffers[m_currentBuffer];

    if (stagingBuffer.m_submitted || stagingBuffer.m_vkOffset == 0) 
    {
        return;
    }

    VkMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
    vkCmdPipelineBarrier(stagingBuffer.m_vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);

    vkEndCommandBuffer(stagingBuffer.m_vkCommandBuffer);

    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.memory = m_vkMemory;
    memoryRange.size = VK_WHOLE_SIZE;
    vkFlushMappedMemoryRanges(m_vkDevice, 1, &memoryRange);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &stagingBuffer.m_vkCommandBuffer;

    vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, stagingBuffer.m_vkFence);

    stagingBuffer.m_submitted = true;

    m_currentBuffer = (m_currentBuffer + 1) % ION_FRAME_DATA_COUNT;
}

ION_NAMESPACE_END