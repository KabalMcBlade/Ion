#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "BaseBufferObject.h"

ION_NAMESPACE_BEGIN

class UniformBuffer : public BaseBufferObject
{
public:
    UniformBuffer();
    ~UniformBuffer();

    ionBool				Alloc(const VkDevice& _device, const void* _data, ionSize _allocSize, EBufferUsage _usage);
    void				Free();

    void				ReferenceTo(const UniformBuffer& _other);
    void				ReferenceTo(const UniformBuffer& _other, ionSize _refOffset, ionSize _refSize);

    void				Update(const void* _data, ionSize _size, ionSize _offset = 0) const;

    void*				MapBuffer(EBufferMappingType _mapType);
    void				UnmapBuffer();

private:
    void				ClearWithoutFreeing();

private:
    UniformBuffer(const UniformBuffer& _Orig) = delete;
    UniformBuffer& operator = (const UniformBuffer&) = delete;
};



ION_NAMESPACE_END