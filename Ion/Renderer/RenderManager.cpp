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


RenderManager::RenderManager() : m_nodeCount(0), m_time(0.0f), m_deltaTime(0.0f), m_lastTime(0.0f)
{

}

RenderManager::~RenderManager()
{

}

ionBool RenderManager::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    return m_renderCore.Init(_instance, _handle, _width, _height, _fullScreen, _enableValidationLayer, _vkDeviceLocalSize, _vkHostVisibleSize, _vkStagingBufferSize);
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

void RenderManager::AddScene(NodeHandle& _root)
{
    m_root = _root;

    // simplify version for now, just to have something to render soon
    const eosVector(NodeHandle)& children = m_root->GetChildren();

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

void RenderManager::Resize()
{
    m_renderCore.Recreate();
    m_mainCamera->UpdateAspectRatio((ionFloat)m_renderCore.GetWidth() / (ionFloat)m_renderCore.GetHeight());
}


void RenderManager::Prepare()
{    
    //
    // Update entities
    m_nodeCount = m_entityNodes.size();

    m_drawSurfaces.resize(m_nodeCount);
}

void RenderManager::UpdateDrawSurface(const Matrix& _projection, const Matrix& _view, ionSize _nodeCount)
{
    for (ionSize i = 0; i < _nodeCount; ++i)
    {
        const Matrix& model = m_entityNodes[i]->GetTransformHandle()->GetMatrix();

        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[0], model[0]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[4], model[1]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[8], model[2]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[12], model[3]);

        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[0], _view[0]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[4], _view[1]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[8], _view[2]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[12], _view[3]);

        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[0], _projection[0]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[4], _projection[1]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[8], _projection[2]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[12], _projection[3]);

        m_drawSurfaces[i].m_indexStart = m_entityNodes[i]->GetMesh(0)->GetIndexStart();
        m_drawSurfaces[i].m_indexCount = m_entityNodes[i]->GetMesh(0)->GetIndexCount();
        m_drawSurfaces[i].m_vertexCache = ionVertexCacheManager().AllocVertex(m_entityNodes[i]->GetMesh(0)->GetVertexData(), m_entityNodes[i]->GetMesh(0)->GetVertexSize());
        m_drawSurfaces[i].m_indexCache = ionVertexCacheManager().AllocIndex(m_entityNodes[i]->GetMesh(0)->GetIndexData(), m_entityNodes[i]->GetMesh(0)->GetIndexSize());
        m_drawSurfaces[i].m_material = m_entityNodes[i]->GetMesh(0)->GetMaterial();
    }

}

void RenderManager::CoreLoop()
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

void RenderManager::Update(ionFloat _deltaTime)
{
    //
    m_root->Update(_deltaTime);

    //
    m_mainCamera->Update();

    //
    const Matrix& projection = m_mainCamera->GetPerspectiveProjection();
    const Matrix& view = m_mainCamera->GetView();
     
    ionVertexCacheManager().BeginMapping();
    UpdateDrawSurface(projection, view, m_nodeCount);
    ionVertexCacheManager().EndMapping();
}

void RenderManager::Frame()
{
    const ionU32 width = m_renderCore.GetWidth();
    const ionU32 height = m_renderCore.GetHeight();

    if (m_renderCore.StartFrame())
    {
        m_mainCamera->StartRenderPass(m_renderCore, 1.0f, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 1.0f, 1.0f);
        m_mainCamera->SetViewport(m_renderCore, 0, 0, width, height, 1.0f, 0.0f, 1.0f);
        m_mainCamera->SetScissor(m_renderCore, 0, 0, width, height, 1.0f);

        for (ionSize i = 0; i < m_nodeCount; ++i)
        {
            m_renderCore.SetState(m_drawSurfaces[i].m_material->GetState().GetStateBits());
            m_renderCore.Draw(m_drawSurfaces[i]);
        }

        m_mainCamera->EndRenderPass(m_renderCore);

        m_renderCore.EndFrame();
    }
}


ION_NAMESPACE_END