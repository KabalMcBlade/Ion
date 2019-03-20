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

#include "../Utilities/LoaderGLTF.h"

#include "../App/Mode.h"


#define ION_PBR_SHADER_NAME    "PBR"
#define ION_PBR_MORPH_SHADER_NAME    "PBRMorph"
#define ION_DIFFUSE_LIGHT_SHADER_NAME    "DiffuseLight"
#define ION_DIFFUSE_LIGHT_MORPH_SHADER_NAME    "DiffuseLightMorph"
#define ION_UNLIT_SHADER_NAME    "Unlit"
#define ION_UNLIT_MORPH_SHADER_NAME    "UnlitMorph"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


enum EPrimitiveType
{
    EPrimitiveType_Triangle,
    EPrimitiveType_Quad,
    EPrimitiveType_Cube,
    EPrimitiveType_Sphere,
    EPrimitiveType_Pyramid
};

class UUID;
class Entity;
class DirectionalLight;
class ION_DLL RenderManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static RenderManager& Instance();

    ionBool LoadModelFromFile(const eosString& _filePath, Camera* _camToUpdate, ObjectHandler& _entity);
    void DumpModelToFile(const eosString& _filePath, const ObjectHandler& _entity, LoaderGLTF::ESerializationLevel _level = LoaderGLTF::ESerializationLevel_Normal);

    void GeneratePrimitive(EVertexLayout _layout, EPrimitiveType _type, ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);

    void LoadColoredTriangle(ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
    void LoadColoredQuad(ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
    void LoadColoredCube(ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
    void LoadColoredSphere(ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
    void LoadColoredPyramid(ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);

    ionBool Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize);
    void    Shutdown();

    RenderManager();
    ~RenderManager();

    ION_INLINE ionFloat GetDeltaTime() const { return m_deltaTime; }

    void    AddDirectionalLight();
    void    RemoveDirectionalLight();
    DirectionalLight*   GetDirectionalLight();

    // NOTE: for now call the following 3 functions just BEFORE start your application and after.
    // This because the scene graph at the moment is not updated at runtime, so will take not effect (or worst side effect)
    void    AddToSceneGraph(ObjectHandler _node);
    void    RemoveFromSceneGraph(ObjectHandler _node);
    void    RemoveAllSceneGraph();

    void    PrepareToShutDown();

    void    Resize(ionS32& _outNewWidth, ionS32 _outNewHeight);
    void    Begin();
    void    End();
    void    CoreLoop();

    void    SendMouseInput(const MouseState& _mouseState);
    void    SendKeyboardInput(const KeyboardState& _keyboardState);

    void    RegisterToInput(const ObjectHandler& _node);
    void    UnregisterFromInput(const ObjectHandler& _node);

    ObjectHandler GetObjectByName(const eosString& _name);
    ObjectHandler GetObjectByUUID(const UUID& _uuid);

    void    Quit();
    ionBool IsRunning();

    // rendering PBR helper functions:
    const Texture*  GenerateBRDF(ObjectHandler _camera);
    const Texture*  GetBRDF() const;

    const Texture*  GenerateIrradianceCubemap(ObjectHandler _camera);
    const Texture*  GetIrradianceCubemap() const;

    const Texture*  GeneratePrefilteredEnvironmentCubemap(ObjectHandler _camera);
    const Texture*  GetPrefilteredEnvironmentCubemap() const;

    const Texture*  GenerateNullTexture();
    const Texture*  GetNullTexure() const;

    // render core interface for outside user (very minimal)
    VkCommandBuffer InstantiateCommandBuffer(VkCommandBufferLevel _level);
    void            ShutdownCommandBuffer(VkCommandBuffer _commandBuffer);

    //////////////////////////////////////////////////////////////////////////

    ionFloat m_exposure;
    ionFloat m_gamma;
    ionFloat m_prefilteredCubeMipLevels;

private:
    RenderManager(const RenderManager& _Orig) = delete;
    RenderManager& operator = (const RenderManager&) = delete;

    void Update(ionFloat _deltaTime);
    void Frame();

    void Recreate();

    void LoadCommonMaterialForIntegratedPrimitive(ObjectHandler& _entity, Material* _material);

private:
    RenderCore  m_renderCore;
    SceneGraph  m_sceneGraph;
    LoaderGLTF  m_loader;

    ionFloat    m_deltaTime;

    ionBool     m_running;

private:
    static RenderManager *s_instance;
};

ION_NAMESPACE_END


#define ionRenderManager() ion::RenderManager::Instance()