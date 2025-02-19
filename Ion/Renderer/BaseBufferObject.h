// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Renderer\BaseBufferObject.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/CoreDefs.h"

#include "../GPU/GpuDataStructure.h"
#include "../GPU/GpuMemoryAllocator.h"

// Actually I don't need this using, because I want to access just to the define in Architecture
NIX_USING_NAMESPACE             

ION_NAMESPACE_BEGIN


#define ION_BUFFER_OBJECT_MAPPED_FLAG   (1 << ( 4 * 8 - 1 ))        //  0x80000000      // 0b10000000000000000000000000000000 
// ~0x80000000  =   0x7FFFFFFF      // 0b0111 1111 1111 1111 1111 1111 1111 1111
// ~15          =   0xFFFFFFF0      // 0b1111 1111 1111 1111 1111 1111 1111 0000        

enum EBufferMappingType 
{
    EBufferMappingType_Read,
    EBufferMappingType_Write
};

enum EBufferUsage
{
    EBufferUsage_Static,            // GPU Read
    EBufferUsage_Dynamic,            // GPU Read, CPU Read/Write
};


class BaseBufferObject 
{
public:
    BaseBufferObject();

    ION_INLINE ionSize      GetSize() const { return (m_size & ~ION_BUFFER_OBJECT_MAPPED_FLAG); }
    ION_INLINE ionSize      GetAllocedSize() const { return ((m_size & ~ION_BUFFER_OBJECT_MAPPED_FLAG) + 15) & ~15; }
    ION_INLINE EBufferUsage GetUsage() const { return m_usage; }
    ION_INLINE VkBuffer     GetObject() const { return m_object; }
    ION_INLINE ionSize      GetOffset() const { return (m_offsetInOtherBuffer & ~ION_BUFFER_OBJECT_MAPPED_FLAG); }

    ION_INLINE ionBool      IsMapped() const { return (m_size & ION_BUFFER_OBJECT_MAPPED_FLAG) != 0; }

protected:
    ION_INLINE void         SetMapped() const { const_cast<ionSize&>(m_size) |= ION_BUFFER_OBJECT_MAPPED_FLAG; }
    ION_INLINE void         SetUnmapped() const { const_cast<ionSize&>(m_size) &= ~ION_BUFFER_OBJECT_MAPPED_FLAG; }
    ION_INLINE ionBool      OwnsBuffer() const { return ((m_offsetInOtherBuffer & ION_BUFFER_OBJECT_MAPPED_FLAG) != 0); }

protected:
    ionSize                 m_size;
    ionSize                 m_offsetInOtherBuffer;
    EBufferUsage            m_usage;
    VkDevice                m_device;
    VkBuffer                m_object;
    GpuMemoryAllocation   m_allocation;
};


ION_NAMESPACE_END