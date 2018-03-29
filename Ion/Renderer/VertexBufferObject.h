#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "BaseBufferObject.h"



NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN



ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct Vertex
{
public:
    Vector				m_position;		    // 16 bytes
    Vector				m_tangent;		    // 16 bytes
    Vector				m_normal;	        // 16 bytes
    Vector				m_color1;		    // 16 bytes
    Vector				m_color2;		    // 16 bytes: in a next version with skinning, here will be placed the weights
    Vector			    m_textureCoordU;	// 16 bytes: This is just the same value copied across all the vector, It is heavy but performing when computed within math lib
    Vector			    m_textureCoordV;	// 16 bytes: This is just the same value copied across all the vector, It is heavy but performing when computed within math lib
};


class VertexBuffer : public BaseBufferObject 
{
public:
    VertexBuffer();
    ~VertexBuffer();

    ionBool				Alloc(const VkDevice& _device, const void* _data, ionSize _allocSize, EBufferUsage _usage);
    void				Free();

    void				ReferenceTo(const VertexBuffer& _other);
    void				ReferenceTo(const VertexBuffer& _other, ionSize _refOffset, ionSize _refSize);

    void				Update(const void* _data, ionSize _size, ionSize _offset = 0) const;

    void*				MapBuffer(EBufferMappingType _mapType);
    ION_INLINE  Vertex* MapVertexBuffer(EBufferMappingType _mapType) { return static_cast<Vertex*>(MapBuffer(_mapType)); }
    void				UnmapBuffer();

private:
    void				ClearWithoutFreeing();

private:
    VertexBuffer(const VertexBuffer& _Orig) = delete;
    VertexBuffer& operator = (const VertexBuffer&) = delete;
};



ION_NAMESPACE_END