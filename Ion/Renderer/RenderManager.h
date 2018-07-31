#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/StandardIncludes.h"
#include "../Core/CoreDefs.h"

#include "RenderDefs.h"
#include "RenderCommon.h"
#include "RenderCore.h"

#include "../Geometry/BoundingBox.h"

#include "../Scene/Node.h"
#include "../Scene/Entity.h"
#include "../Scene/Camera.h"
#include "../Scene/SceneGraph.h"

#include "../App/Mode.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


enum EPrimitiveType
{
    EPrimitiveType_Triangle,
    EPrimitiveType_Quad,
    EPrimitiveType_Cube,
    EPrimitiveType_Sphere
};


class Entity;
class ION_DLL RenderManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static RenderManager& Instance();

    ionBool LoadModelFromFile(const eosString& _fileName, Entity& _entity, ionBool _generateNormalWhenMissing = false, ionBool _generateTangentWhenMissing = false, ionBool _setBitangentSign = false);
    void LoadPrimitive(EVertexLayout _layout, EPrimitiveType _type, Entity& _entity);

    ionBool Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize);
    void    Shutdown();

    RenderManager();
    ~RenderManager();

    void    AddToSceneGraph(NodeHandle _node);

    void    Resize(ionS32& _outNewWidth, ionS32 _outNewHeight);
    void    Prepare();
    void    CoreLoop();

    void    SendMouseInput(const MouseState& _mouseState);
    void    SendKeyboardInput(const KeyboardState& _keyboardState);

    void    RegisterToInput(const NodeHandle& _node);
    void    UnregisterFromInput(const NodeHandle& _node);

    void    Quit();
    ionBool IsRunning();

    // rendering PBR helper functions:
    Texture*    GenerateBRDF();
    Texture*    GetBRDF();

    Texture*    GenerateIrradianceCubemap(const Texture* _environmentCubeMap, EntityHandle _skyboxEntity);
    Texture*    GetIrradianceCubemap();

    Texture*    GeneratePrefilteredEnvironmentCubemap(const Texture* _environmentCubeMap, EntityHandle _skyboxEntity);
    Texture*    GetPrefilteredEnvironmentCubemap();

private:
    RenderManager(const RenderManager& _Orig) = delete;
    RenderManager& operator = (const RenderManager&) = delete;

    void Update(ionFloat _deltaTime);
    void Frame();

private:
    RenderCore  m_renderCore;
    SceneGraph  m_sceneGraph;

    ionFloat    m_deltaTime;

    ionBool     m_running;

private:
    static RenderManager *s_instance;
};

ION_NAMESPACE_END


#define ionRenderManager() ion::RenderManager::Instance()