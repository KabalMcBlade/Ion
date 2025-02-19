// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Renderer\StagingBufferManager.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


struct StagingBuffer 
{
    StagingBuffer() :
        m_submitted(false),
        m_vkCommandBuffer(VK_NULL_HANDLE),
        m_vkBuffer(VK_NULL_HANDLE),
        m_vkFence(VK_NULL_HANDLE),
        m_vkOffset(0),
        m_data(nullptr) {}

    ionBool                m_submitted;
    VkCommandBuffer        m_vkCommandBuffer;
    VkBuffer            m_vkBuffer;
    VkFence                m_vkFence;
    VkDeviceSize        m_vkOffset;
    ionU8*                m_data;
};


class StagingBufferManager final
{
public:
    static StagingBufferManager& Instance();

    ionBool Init(VkDevice _vkDevice, VkQueue _vkGraphicsQueue, ionS32 _vkGraphicsFamilyIndex);
    void    Shutdown();

    StagingBufferManager();
    ~StagingBufferManager();

    ionU8*  Stage(ionSize _size, ionSize _alignment, VkCommandBuffer& _outVkCommandBuffer, VkBuffer& _outVkBuffer, ionSize& _outVkBufferOffset);
    void    Submit();

private:
    StagingBufferManager(const StagingBufferManager& _Orig) = delete;
    StagingBufferManager& operator = (const StagingBufferManager&) = delete;

private:
    VkDevice        m_vkDevice;
    VkQueue         m_vkGraphicsQueue;
    ionS32          m_vkGraphicsFamilyIndex;
    ionU8*            m_mappedData;
    VkDeviceMemory    m_vkMemory;
    VkCommandPool    m_vkCommandPool;
    StagingBuffer   m_buffer;
};

ION_NAMESPACE_END


#define ionStagingBufferManager() ion::StagingBufferManager::Instance()