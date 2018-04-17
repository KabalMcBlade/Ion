#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "RenderCommon.h"


ION_NAMESPACE_BEGIN

class UniformBuffer;

class VertexCacheManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static VertexCacheManager& Instance();

    ionBool Init(VkDeviceSize _uniformBufferOffsetAlignment);
    void    Shutdown();

    ionBool	GetJointBuffer(VertexCacheHandler _handler, UniformBuffer *_jointBuffer);

    VertexCacheManager();
    ~VertexCacheManager();

private:
    VertexCacheManager(const VertexCacheManager& _Orig) = delete;
    VertexCacheManager& operator = (const VertexCacheManager&) = delete;

private:

    static VertexCacheManager *s_instance;
};

ION_NAMESPACE_END


#define ionVertexCacheManager() ion::VertexCacheManager::Instance()