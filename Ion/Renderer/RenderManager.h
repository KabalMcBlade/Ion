#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"
//#include "../Scene/Node.h"

#include "RenderCommon.h"

#include "RenderCore.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

//class Node;
class BaseCamera;
class Entity;

class ION_DLL RenderManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static RenderManager& Instance();

    //const RenderCore& GetRenderer() const { return m_renderCore; }
    ionBool LoadModelFromFile(const eosString& _fileName, Entity& _entity);

    ionBool Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize);
    void    Shutdown();

    RenderManager();
    ~RenderManager();

    void AddCamera(BaseCamera& _camera);
    void AddEntity(Entity& _entity);

    void PreRender();
    void Render();

private:
    RenderManager(const RenderManager& _Orig) = delete;
    RenderManager& operator = (const RenderManager&) = delete;

private:
    RenderCore  m_renderCore;

    //eosVector(Node*)        m_nodeList;
    eosVector(BaseCamera*)  m_cameraList;
    eosVector(Entity*)      m_entityList;

    eosVector(VertexCacheHandler)   m_vertexHandlers;
    eosVector(VertexCacheHandler)   m_indexHandlers;

private:
    static RenderManager *s_instance;
};

ION_NAMESPACE_END


#define ionRenderManager() ion::RenderManager::Instance()