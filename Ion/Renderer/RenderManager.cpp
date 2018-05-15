#include "RenderManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"

#include "../Utilities/LoaderGLTF.h"

#include "VertexCacheManager.h"

#include "../Geometry/Mesh.h"

#define SHADOW_MAP_SIZE					1024



EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN



RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{

}

ionBool RenderManager::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    return m_renderCore.Init(_instance, _handle, _width, _height, _fullScreen, _enableValidationLayer, _shaderFolderPath, _vkDeviceLocalSize, _vkHostVisibleSize, _vkStagingBufferSize);
}

void RenderManager::Shutdown()
{
    m_renderCore.Shutdown();
}

void RenderManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(RenderManager, EOS_MEMORY_ALIGNMENT_SIZE);
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

ionBool RenderManager::LoadModelFromFile(const eosString& _fileName, Entity& _entity)
{
    LoaderGLTF loader;
    return loader.Load(_fileName, m_renderCore.GetDevice(), _entity);
}

void RenderManager::AddScene(NodeHandle& _root)
{
    // simplify version for now, just to have something to render soon
    m_rootMatrix = _root->GetTransformHandle()->GetMatrix();

    const eosVector(NodeHandle)& children = _root->GetChildren();

    eosVector(NodeHandle)::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;

    for (; it != end; ++it)
    {
        NodeHandle nh = (*it);

        switch(nh->GetNodeType())
        {
        case ENodeType_Camera:
            if (!m_mainCamera.IsValid())
            {
                m_mainCamera.SetFromOther<Node>(nh);
            }
            break;
        case ENodeType_DirectionalLight:
            if (!m_sceneLight.IsValid())
            {
                m_sceneLight.SetFromOther<Node>(nh);
            }
            break;
        case ENodeType_Entity:
            EntityHandle handle;
            handle.SetFromOther<Node>(nh);
            m_sceneBoundingBox.Expande(handle->GetTransformedBoundingBox());
            m_entityNodes.push_back(handle);
            break;
        }
    }
}

void RenderManager::AddScene(Node& _root)
{
    SmartPointer<Node> rootHandle(&_root);
    AddScene(rootHandle);
}

void RenderManager::CoreLoop()
{
    Update();
    DrawFrame();
}

void RenderManager::Update()
{
    //
    // compute view projection matrix
    const Matrix projection = m_mainCamera->GetPerspectiveProjection();
    const Matrix view = m_mainCamera->GetView();

    m_viewProjection = projection * view;

    //
    // Update lights here when (not yet implemented)


    //
    // Update entities
    /*
    eosVector(EntityHandle)::const_iterator begin = m_entityNodes.cbegin(), end = m_entityNodes.cend(), it = begin;

    for (; it != end; ++it)
    {
        EntityHandle entityHandle = (*it);
        entityHandle->GetTransformHandle()->UpdateTransform(m_rootMatrix);
        entityHandle->GetTransformHandle()->UpdateTransformInverse(m_rootMatrix);
    }
    */

    //
    // Update shadow light and shadow map here
    //m_sceneLight->ComputeScaleAndOffset(*m_mainCamera, m_sceneBoundingBox, SHADOW_MAP_SIZE);
    //m_shadowLightViewProjection = m_sceneLight->GetLightViewProjMatrix();
}

void RenderManager::DrawFrame()
{
    //m_renderCore.StartFrame();


    //m_renderCore.EndFrame();
}

ION_NAMESPACE_END