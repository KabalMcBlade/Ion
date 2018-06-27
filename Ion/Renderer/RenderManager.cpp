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


RenderManager::RenderManager() : m_time(0.0f), m_deltaTime(0.0f), m_lastTime(0.0f)
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
}
/*
void RenderManager::AddScene(Node& _root)
{
    SmartPointer<Node> rootHandle(&_root);
    AddScene(rootHandle);
}
*/
void RenderManager::Resize()
{
    m_renderCore.Recreate();

    for (eosMap(BaseCameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        BaseCameraHandle cam = iter->first;

        cam->UpdateAspectRatio((ionFloat)m_renderCore.GetWidth() / (ionFloat)m_renderCore.GetHeight());
    }
}

void RenderManager::FillCameraMapTree(NodeHandle& _node)
{
    if (_node->GetNodeType() == ENodeType_Camera)
    {
        if (m_treeNodes.find(_node) == m_treeNodes.end())
        {
            m_treeNodes.insert(std::pair<BaseCameraHandle, eosVector(EntityHandle)>(_node, eosVector(EntityHandle)()));
        }
    }

    if (_node->GetChildren().empty())
    {
        return;
    }

    const eosVector(NodeHandle)& children = _node->GetChildren();
    eosVector(NodeHandle)::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;
    for (; it != end; ++it)
    {
        NodeHandle nh = (*it);
        FillCameraMapTree(nh);
    }
}

void RenderManager::GenerateMapTree(NodeHandle& _node)
{
    if (_node->GetNodeType() == ENodeType_Entity)
    {
        for (eosMap(BaseCameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
        {
            const BaseCameraHandle& cam = iter->first;

            if (_node->IsInRenderLayer(cam->GetRenderLayer()))
            {
                m_nodeCountPerCamera[cam->GetHash()]++;
                m_treeNodes[cam].push_back(_node);
            }
        }
    }

    if (_node->GetChildren().empty())
    {
        return;
    }

    const eosVector(NodeHandle)& children = _node->GetChildren();
    eosVector(NodeHandle)::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;
    for (; it != end; ++it)
    {
        NodeHandle nh = (*it);
        GenerateMapTree(nh);
    }
}

void RenderManager::Prepare()
{
    // Generate the plain map recursively
    // I don't care about the speed here, is just once before start
    FillCameraMapTree(m_root);
    GenerateMapTree(m_root);

    //
    // Update entities
    for (eosMap(BaseCameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        const BaseCameraHandle& cam = iter->first;
        m_drawSurfaces[cam->GetHash()].resize(m_nodeCountPerCamera[cam->GetHash()]);
    }
}

void RenderManager::UpdateDrawSurface(ionSize _cameraHash, const Matrix& _projection, const Matrix& _view, const EntityHandle& _entity, ionU32 _index)
{
    const Matrix& model = _entity->GetTransformHandle()->GetMatrixWS();

    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_modelMatrix[0], model[0]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_modelMatrix[4], model[1]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_modelMatrix[8], model[2]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_modelMatrix[12], model[3]);

    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_viewMatrix[0], _view[0]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_viewMatrix[4], _view[1]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_viewMatrix[8], _view[2]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_viewMatrix[12], _view[3]);

    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_projectionMatrix[0], _projection[0]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_projectionMatrix[4], _projection[1]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_projectionMatrix[8], _projection[2]);
    _mm_storeu_ps(&m_drawSurfaces[_cameraHash][_index].m_projectionMatrix[12], _projection[3]);

    m_drawSurfaces[_cameraHash][_index].m_indexStart = _entity->GetMesh(0)->GetIndexStart();
    m_drawSurfaces[_cameraHash][_index].m_indexCount = _entity->GetMesh(0)->GetIndexCount();
    m_drawSurfaces[_cameraHash][_index].m_vertexCache = ionVertexCacheManager().AllocVertex(_entity->GetMesh(0)->GetVertexData(), _entity->GetMesh(0)->GetVertexSize());
    m_drawSurfaces[_cameraHash][_index].m_indexCache = ionVertexCacheManager().AllocIndex(_entity->GetMesh(0)->GetIndexData(), _entity->GetMesh(0)->GetIndexSize());
    m_drawSurfaces[_cameraHash][_index].m_material = _entity->GetMesh(0)->GetMaterial();
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

    ionVertexCacheManager().BeginMapping();

    ionU32 index = 0;
    for (eosMap(BaseCameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        const BaseCameraHandle& cam = iter->first;

        cam->UpdateView();

        const Matrix& projection = cam->GetPerspectiveProjection();
        const Matrix& view = cam->GetView();

        const eosVector(EntityHandle)& entities = iter->second;
        eosVector(EntityHandle)::const_iterator begin = entities.cbegin(), end = entities.cend(), it = begin;
        for (; it != end; ++it)
        {
            const EntityHandle& entity = (*it);
            UpdateDrawSurface(cam->GetHash(), projection, view, entity, index);
            ++index;
        }
    }

    ionVertexCacheManager().EndMapping();
}

void RenderManager::Frame()
{
    const ionU32 width = m_renderCore.GetWidth();
    const ionU32 height = m_renderCore.GetHeight();

    if (m_renderCore.StartFrame())
    {
        for (eosMap(BaseCameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
        {
            const BaseCameraHandle& cam = iter->first;

            cam->StartRenderPass(m_renderCore, 1.0f, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 1.0f, 1.0f);
            cam->SetViewport(m_renderCore, 0, 0, width, height, 1.0f, 0.0f, 1.0f);
            cam->SetScissor(m_renderCore, 0, 0, width, height, 1.0f);

            const eosVector(DrawSurface)& surfaces = m_drawSurfaces[cam->GetHash()];

            eosVector(DrawSurface)::const_iterator begin = surfaces.cbegin(), end = surfaces.cend(), it = begin;
            for (; it != end; ++it)
            {
                const DrawSurface& drawSuraface = (*it);
                
                m_renderCore.SetState(drawSuraface.m_material->GetState().GetStateBits());
                m_renderCore.Draw(drawSuraface);
            }

            cam->EndRenderPass(m_renderCore);
        }

        m_renderCore.EndFrame();
    }
}


ION_NAMESPACE_END