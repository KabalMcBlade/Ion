#include "RenderManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"

#include "../Utilities/LoaderGLTF.h"

#include "VertexCacheManager.h"
#include "DrawSurface.h"

#include "../Geometry/Mesh.h"

#define SHADOW_MAP_SIZE					1024



EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN



RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager() : m_nodeCount(0)
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
}

void RenderManager::Prepare()
{    
    /*
    const ionSize nodeCount = m_entityNodes.size();
    for (ionSize i = 0; i < nodeCount; ++i)
    {
        VertexCacheHandler vertexCache = ionVertexCacheManager().AllocVertex(m_entityNodes[i]->GetVertexBuffer(0, 0), m_entityNodes[i]->GetVertexBufferSize(0, 0));
        VertexCacheHandler indexCache = ionVertexCacheManager().AllocIndex(m_entityNodes[i]->GetIndexBuffer(0, 0), m_entityNodes[i]->GetIndexBufferSize(0, 0));

        m_vertexCache.push_back(vertexCache);
        m_indexCache.push_back(indexCache);
    }
    */

    //
    // Update entities
    m_nodeCount = m_entityNodes.size();

    // temp: need a proper way to reserve all the amount of surfaces
    if (m_drawSurfaces.capacity() == 0)
    {
        m_drawSurfaces.resize(m_nodeCount);
    }

    // because I want to alternate the buffer, before the first frame I'm preparing the 0 index, which will be presented in the first frame.
    //
    m_mainCamera->Update();

    //
    const Matrix& projection = m_mainCamera->GetPerspectiveProjection();
    const Matrix& view = m_mainCamera->GetView();

    UpdateDrawSurface(projection, view, m_nodeCount);

    //ionVertexCacheManager().BeginFrame();
}

void RenderManager::UpdateDrawSurface(const Matrix& _projection, const Matrix& _view, ionSize _nodeCount)
{
    for (ionSize i = 0; i < _nodeCount; ++i)
    {
        //
        // here we need to update the entity position

        m_entityNodes[i]->GetTransformHandle()->UpdateTransform();
        //m_entityNodes[i]->GetTransformHandle()->UpdateTransformInverse();

        const Matrix& model = m_entityNodes[i]->GetTransformHandle()->GetMatrix();
        //const Matrix& model = m_entityNodes[i]->GetTransformHandle()->GetMatrixInverse();

        // not aligned... just to test
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

        m_drawSurfaces[i].m_indexCount = m_entityNodes[i]->GetIndexBufferSize(0, 0);
        m_drawSurfaces[i].m_vertexCache = ionVertexCacheManager().AllocVertex(m_entityNodes[i]->GetVertexBuffer(0, 0), m_entityNodes[i]->GetVertexBufferSize(0, 0));
        m_drawSurfaces[i].m_indexCache = ionVertexCacheManager().AllocIndex(m_entityNodes[i]->GetIndexBuffer(0, 0), m_entityNodes[i]->GetIndexBufferSize(0, 0));
        m_drawSurfaces[i].m_material = m_entityNodes[i]->GetMaterial(0, 0);
    }

}

void RenderManager::CoreLoop()
{
    Update();
    DrawFrame();
}

void RenderManager::Update()
{
    //
    m_mainCamera->Update();

    //
    const Matrix& projection = m_mainCamera->GetPerspectiveProjection();
    const Matrix& view = m_mainCamera->GetView();

    UpdateDrawSurface(projection, view, m_nodeCount);
}

void RenderManager::DrawFrame()
{
    const ionU32 width = m_renderCore.GetWidth();
    const ionU32 height = m_renderCore.GetHeight();
    const ionSize drawSurfacesCount = m_drawSurfaces.size();

    m_renderCore.BlockingSwapBuffers();
    ionVertexCacheManager().BeginFrame();
    if (m_renderCore.StartFrame())
    {
        m_renderCore.SetViewport(0, 0, width, height);
        m_renderCore.SetScissor(0, 0, width, height);
        m_renderCore.SetState(ECullingMode_Front);
        m_renderCore.SetClear(true, true, true, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 0.0f, 0.0f, 0.0f);
        
        /*
        for (ionSize i = 0; i < drawSurfacesCount; ++i)
        {
            m_renderCore.Draw(m_drawSurfaces[i]);
        }
        */
        m_renderCore.EndFrame();
    }
}


ION_NAMESPACE_END