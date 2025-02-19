// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Renderer\VertexBufferObject.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "BaseBufferObject.h"

#include "RenderCommon.h"

NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class VertexBuffer : public BaseBufferObject 
{
public:
    VertexBuffer();
    ~VertexBuffer();

    ionBool             Alloc(const VkDevice& _device, const void* _data, ionSize _allocSize, EBufferUsage _usage);
    void                Free();

    void                ReferenceTo(const VertexBuffer& _other);
    void                ReferenceTo(const VertexBuffer& _other, ionSize _refOffset, ionSize _refSize);

    void                Update(const void* _data, ionSize _size, ionSize _offset = 0) const;

    void*                MapBuffer(EBufferMappingType _mapType);
    ION_INLINE  Vertex* MapVertexBuffer(EBufferMappingType _mapType) { return static_cast<Vertex*>(MapBuffer(_mapType)); }
    void                UnmapBuffer();

private:
    void                ClearWithoutFreeing();
};



ION_NAMESPACE_END