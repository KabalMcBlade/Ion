#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/CoreDefs.h"

VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class GPUMemoryManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static GPUMemoryManager& Instance();

    ION_INLINE vkGpuMemoryAllocator& GetAllocator() { return m_gpuAllocator; }
    ION_INLINE const vkGpuMemoryAllocator& GetAllocator() const { return m_gpuAllocator; }

    GPUMemoryManager();
    ~GPUMemoryManager();

private:
    GPUMemoryManager(const GPUMemoryManager& _Orig) = delete;
    GPUMemoryManager& operator = (const GPUMemoryManager&) = delete;

private:
    vkGpuMemoryAllocator m_gpuAllocator;

    static GPUMemoryManager *s_instance;
};

ION_NAMESPACE_END

#define ionGPUMemoryManager() ion::GPUMemoryManager::Instance().GetAllocator()