#include "Managers.h"

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