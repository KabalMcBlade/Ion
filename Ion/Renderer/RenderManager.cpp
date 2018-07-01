#include "RenderManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"

#include "../Utilities/LoaderGLTF.h"

#include "VertexCacheManager.h"

#include "../Geometry/Mesh.h"

#include "../Texture/TextureManager.h"

#include "../Geometry/PrimitiveFactory.h"

//#define SHADOW_MAP_SIZE                    1024

#define ION_CACHE_LINE_SIZE        128
#define ION_MAX_FRAME_MEMORY    67305472    //64 * 1024 * 1024


#define ION_FPS_LIMIT  0.01666666666666666666666666666667f     // 1.0 / 60.0f



EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager() : m_time(0.0f), m_deltaTime(0.0f), m_lastTime(0.0f), m_running(false)
{

}

RenderManager::~RenderManager()
{

}

ionBool RenderManager::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    if (m_renderCore.Init(_instance, _handle, _width, _height, _fullScreen, _enableValidationLayer, _vkDeviceLocalSize, _vkHostVisibleSize, _vkStagingBufferSize))
    {
        m_running = true;
        return true;
    }
    else
    {
        return false;
    }
}

void RenderManager::Shutdown()
{
    m_renderCore.Shutdown();
}

void RenderManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(RenderManager, ION_MEMORY_ALIGNMENT_SIZE);
    }
}

void RenderManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

RenderManager& RenderManager::Instance()
{
    return *s_instance;
}

ionBool RenderManager::LoadModelFromFile(const eosString& _fileName, Entity& _entity, ionBool _generateNormalWhenMissing /*= false*/, ionBool _generateTangentWhenMissing /*= false*/, ionBool _setBitangentSign /*= false*/)
{
    LoaderGLTF loader;
    return loader.Load(_fileName, _entity, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);
}

void RenderManager::LoadPrimitive(EVertexLayout _layout, EPrimitiveType _type, Entity& _entity)
{
    switch(_type)
    {
    case EPrimitiveType_Triangle:
        PrimitiveFactory::GenerateTriangle(_layout, _entity);
        break;
    case EPrimitiveType_Quad:
        PrimitiveFactory::GenerateQuad(_layout, _entity);
        break;
    case EPrimitiveType_Cube:
        PrimitiveFactory::GenerateCube(_layout, _entity);
        break;
    case EPrimitiveType_Sphere:
        PrimitiveFactory::GenerateSphere(_layout, _entity);
        break;
    }
}

void RenderManager::AddToSceneGraph(NodeHandle _node)
{
    m_sceneGraph.AddToScene(_node);
}

void RenderManager::Resize(ionS32& _outNewWidth, ionS32 _outNewHeight)
{
    m_renderCore.Recreate();
    m_sceneGraph.UpdateAllCameraAspectRatio(m_renderCore);


    _outNewWidth = m_renderCore.GetWidth();
    _outNewHeight = m_renderCore.GetHeight();
}

void RenderManager::Prepare()
{
    m_sceneGraph.Prepare();
}

void RenderManager::CoreLoop()
{
    if (m_running)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        m_time = std::chrono::duration<ionFloat, std::chrono::seconds::period>(currentTime - startTime).count();

        m_deltaTime = m_time - m_lastTime;

        while (m_time - m_lastTime > ION_FPS_LIMIT)
        {
            Update(m_deltaTime);
            Frame();

            m_lastTime = m_time;
        }
    }
}

void RenderManager::Update(ionFloat _deltaTime)
{
    m_sceneGraph.Update(_deltaTime);
}

void RenderManager::Frame()
{
    const ionU32 width = m_renderCore.GetWidth();
    const ionU32 height = m_renderCore.GetHeight();

    if (m_renderCore.StartFrame())
    {
        m_sceneGraph.Render(m_renderCore, width, height);

        m_renderCore.EndFrame();
    }
}

void RenderManager::SetMouseInput(const MouseState& _mouseState)
{
    m_sceneGraph.UpdateMouseInput(_mouseState, m_deltaTime);
}

void RenderManager::SetKeyboardInput(const KeyboardState& _keyboardState)
{
    m_sceneGraph.UpdateKeyboardInput(_keyboardState, m_deltaTime);
}

void RenderManager::SetActiveInputNode(const NodeHandle& _node)
{
    m_sceneGraph.SetActiveInputNode(_node);
}

void RenderManager::SetActiveInputNode(ionSize _nodeHash)
{
    m_sceneGraph.SetActiveInputNode(_nodeHash);
}

void RenderManager::Quit()
{
    m_running = false;
}

ionBool RenderManager::IsRunning()
{
    return m_running;
}

ION_NAMESPACE_END