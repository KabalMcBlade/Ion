#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Nix/Nix/Nix.h"
#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/CoreDefs.h"


// Actually I don't need this using, because I want to access just to the define in Architecture
NIX_USING_NAMESPACE             
VK_ALLOCATOR_USING_NAMESPACE

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
    EBufferUsage_Static,			// GPU Read
    EBufferUsage_Dynamic,			// GPU Read, CPU Read/Write
};


class BaseBufferObject 
{
public:
    BaseBufferObject();

    ION_INLINE ionSize		GetSize() const { return (m_size & ~ION_BUFFER_OBJECT_MAPPED_FLAG); }
    ION_INLINE ionSize		GetAllocedSize() const { return ((m_size & ~ION_BUFFER_OBJECT_MAPPED_FLAG) + 15) & ~15; }
    ION_INLINE EBufferUsage	GetUsage() const { return m_usage; }
    ION_INLINE VkBuffer		GetObject() const { return m_object; }
    ION_INLINE ionSize		GetOffset() const { return (m_offsetInOtherBuffer & ~ION_BUFFER_OBJECT_MAPPED_FLAG); }

    ION_INLINE ionBool		IsMapped() const { return (m_size & ION_BUFFER_OBJECT_MAPPED_FLAG) != 0; }

protected:
    ION_INLINE void			SetMapped() const { const_cast<ionSize&>(m_size) |= ION_BUFFER_OBJECT_MAPPED_FLAG; }
    ION_INLINE void			SetUnmapped() const { const_cast<ionSize&>(m_size) &= ~ION_BUFFER_OBJECT_MAPPED_FLAG; }
    ION_INLINE ionBool		OwnsBuffer() const { return ((m_offsetInOtherBuffer & ION_BUFFER_OBJECT_MAPPED_FLAG) != 0); }

protected:
    ionSize					m_size;
    ionSize					m_offsetInOtherBuffer;
    EBufferUsage	        m_usage;
    VkDevice                m_device;
    VkBuffer			    m_object;
    vkGpuMemoryAllocation   m_allocation;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if NIX_ARCH & NIX_ARCH_SSE2_FLAG

ION_INLINE void CopyBuffer(ionU8* _dst, const ionU8* _src, ionSize _size)
{
    ionAssertReturnVoid(ION_IS_ALIGNED(_dst, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
    ionAssertReturnVoid(ION_IS_ALIGNED(_src, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");


    ionSize i = 0;
    for (; i + 128 <= _size; i += 128)
    {
        __nixInt4 d0 = _mm_load_si128((__nixInt4 *)&_src[i + 0 * 16]);
        __nixInt4 d1 = _mm_load_si128((__nixInt4 *)&_src[i + 1 * 16]);
        __nixInt4 d2 = _mm_load_si128((__nixInt4 *)&_src[i + 2 * 16]);
        __nixInt4 d3 = _mm_load_si128((__nixInt4 *)&_src[i + 3 * 16]);
        __nixInt4 d4 = _mm_load_si128((__nixInt4 *)&_src[i + 4 * 16]);
        __nixInt4 d5 = _mm_load_si128((__nixInt4 *)&_src[i + 5 * 16]);
        __nixInt4 d6 = _mm_load_si128((__nixInt4 *)&_src[i + 6 * 16]);
        __nixInt4 d7 = _mm_load_si128((__nixInt4 *)&_src[i + 7 * 16]);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 0 * 16], d0);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 1 * 16], d1);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 2 * 16], d2);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 3 * 16], d3);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 4 * 16], d4);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 5 * 16], d5);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 6 * 16], d6);
        _mm_stream_si128((__nixInt4 *)&_dst[i + 7 * 16], d7);
    }
    for (; i + 16 <= _size; i += 16) 
    {
        __nixInt4 d = _mm_load_si128((__nixInt4 *)&_src[i]);
        _mm_stream_si128((__nixInt4 *)&_dst[i], d);
    }
    for (; i + 4 <= _size; i += 4)
    {
        *(ionU32 *)&_dst[i] = *(const ionU32 *)&_src[i];
    }
    for (; i < _size; i++)
    {
        _dst[i] = _src[i];
    }
    _mm_sfence();
}

#else

ION_INLINE void CopyBuffer(ionU8* _dst, const ionU8* _src, ionSize _size)
{
    ionAssertReturnVoid(ION_IS_ALIGNED(_dst, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
    ionAssertReturnVoid(ION_IS_ALIGNED(_src, ION_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
    memcpy(_dst, _src, _size);
}

#endif




ION_NAMESPACE_END