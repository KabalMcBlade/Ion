#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "RenderDefs.h"
#include "RenderCommon.h"
#include "RenderCore.h"

#include "../Geometry/BoundingBox.h"

#include "../Scene/Node.h"
#include "../Scene/Entity.h"
#include "../Scene/BaseCamera.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN



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

    void    AddScene(NodeHandle& _root);
    void    AddScene(Node& _root);

	void	Resize();
    void    Prepare();
    void    CoreLoop();

private:
    RenderManager(const RenderManager& _Orig) = delete;
    RenderManager& operator = (const RenderManager&) = delete;

    void Update();
    void Frame();

    void UpdateDrawSurface(const Matrix& _projection, const Matrix& _view, ionSize _nodeCount);

private:
    RenderCore              m_renderCore;

    //////////////////////////////////////////////////////////////////////////
    // Should be inside a "scene graph" class or a "scene" class
    ionSize                 m_nodeCount;
    BoundingBox             m_sceneBoundingBox;
    BaseCameraHandle        m_mainCamera;   // for now only one supported
    eosVector(EntityHandle) m_entityNodes;
    //////////////////////////////////////////////////////////////////////////


    eosVector(DrawSurface)  m_drawSurfaces;


private:
    static RenderManager *s_instance;
};

ION_NAMESPACE_END


#define ionRenderManager() ion::RenderManager::Instance()