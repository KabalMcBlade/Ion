#include "Memory.h"

#include "../Core/FileSystemManager.h"
#include "../Renderer/GPUMemoryManager.h"
#include "../Renderer/StagingBufferManager.h"
#include "../Renderer/VertexCacheManager.h"
#include "../Shader/ShaderProgramHelper.h"
#include "../Shader/ShaderProgramManager.h"
#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"
#include "../Renderer/RenderManager.h"


ION_NAMESPACE_BEGIN


ION_DLL void InitializeAllocators(eosSize _uiHeapSize, eosSize _uiLinearSize, eosSize _uiStackBlockSize, eosSize _uiStackBlockCount)
{
    MemoryManager::Instance().GetHeapAllocator().Init(_uiHeapSize);
    MemoryManager::Instance().GetLinearAllocator().Init(_uiLinearSize);
    MemoryManager::Instance().GetStackAllocator().Init(_uiStackBlockSize, _uiStackBlockCount);
}

ION_DLL void ShutdownAllocators()
{
    MemoryManager::Instance().GetHeapAllocator().Shutdown();
    MemoryManager::Instance().GetLinearAllocator().Shutdown();
    MemoryManager::Instance().GetStackAllocator().Shutdown();
}


ION_DLL void InitializeVulkanAllocators(vkaSize _uiSizeCommand, vkaSize _uiSizeObject, vkaSize _uiSizeCache, vkaSize _uiSizeDevice, vkaSize _uiSizeInstace, vkaSize _uiGpuMaxMemoryBlocks)
{
    vkMemoryInit(_uiSizeCommand, _uiSizeObject, _uiSizeCache, _uiSizeDevice, _uiSizeInstace);
    vkGpuMemoryInit(_uiGpuMaxMemoryBlocks); 
}

ION_DLL void ShutdownVulkanAllocators()
{
    vkGpuMemoryShutdown();
    vkMemoryShutdown();
}

ION_DLL void InitializeManagers()
{
    FileSystemManager::Create();
    GPUMemoryManager::Create();
    VertexCacheManager::Create();
    StagingBufferManager::Create();
    ShaderProgramHelper::Create();
    ShaderProgramManager::Create();
    TextureManager::Create();
    MaterialManager::Create();
    RenderManager::Create();
}

ION_DLL void ShutdownManagers()
{
    RenderManager::Destroy();
    MaterialManager::Destroy();
    TextureManager::Destroy();
    ShaderProgramManager::Destroy();
    ShaderProgramHelper::Destroy();
    StagingBufferManager::Destroy();
    VertexCacheManager::Destroy();
    GPUMemoryManager::Destroy();
    FileSystemManager::Destroy();
}


ION_NAMESPACE_END