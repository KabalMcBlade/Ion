#include "SceneGraph.h"

#include "../Renderer/RenderCore.h"
#include "../Renderer/VertexCacheManager.h"

#include "../Renderer/RenderCommon.h"

NIX_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

SceneGraph::SceneGraph() : m_activeInputHashNode(0)
{
    m_rootHandle = eosNew(Node, ION_MEMORY_ALIGNMENT_SIZE);
    m_rootHandle->GetTransformHandle()->SetPosition(VectorHelper::Get0001());
    m_rootHandle->GetTransformHandle()->SetRotation(VectorHelper::Get0001());
}

SceneGraph::~SceneGraph()
{
    for (eosMap(CameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        eosVector(EntityHandle)& entities = iter->second;
        entities.clear();
    }
    m_treeNodes.clear();

    for (eosMap(ionSize, eosVector(DrawSurface))::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        eosVector(DrawSurface)& drawSurfaces = iter->second;
        drawSurfaces.clear();
    }
    m_drawSurfaces.clear();

    m_nodeCountPerCamera.clear();
    m_hashToNodeMap.clear();
}

void SceneGraph::AddToScene(NodeHandle& _node)
{
    m_hashToNodeMap.insert(std::pair<ionSize, NodeHandle>(_node->GetHash(), _node));

    _node->AttachToParent(m_rootHandle);
}

void SceneGraph::RemoveFromScene(NodeHandle& _node)
{
    //eosMap(ionSize, NodeHandle)::iterator itErase = m_hashToNodeMap.find(_node->GetHash()); 
    //m_hashToNodeMap.erase(itErase);
    m_hashToNodeMap.erase(_node->GetHash());

    _node->DetachFromParent();
}

void SceneGraph::UpdateAllCameraAspectRatio(const RenderCore& _renderCore)
{
    for (eosMap(CameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        CameraHandle cam = iter->first;

        cam->UpdateAspectRatio((ionFloat)_renderCore.GetWidth() / (ionFloat)_renderCore.GetHeight());
    }
}

void SceneGraph::FillCameraMapTree(NodeHandle& _node)
{
    if (_node->GetNodeType() == ENodeType_Camera)
    {
        if (m_treeNodes.find(_node) == m_treeNodes.end())
        {
            m_treeNodes.insert(std::pair<CameraHandle, eosVector(EntityHandle)>(_node, eosVector(EntityHandle)()));
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

void SceneGraph::GenerateMapTree(NodeHandle& _node)
{
    if (_node->GetNodeType() == ENodeType_Entity)
    {
        for (eosMap(CameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
        {
            const CameraHandle& cam = iter->first;

            if (_node->IsInRenderLayer(cam->GetRenderLayer()))
            {
                EntityHandle entity = _node;
                m_sceneBoundingBox.Expande(entity->GetTransformedBoundingBox());

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

void SceneGraph::Prepare()
{
    // Generate the plain map recursively
    // I don't care about the speed here, is just once before start
    FillCameraMapTree(m_rootHandle);
    GenerateMapTree(m_rootHandle);

    // Update entities
    for (eosMap(CameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        const CameraHandle& cam = iter->first;
        m_drawSurfaces[cam->GetHash()].resize(m_nodeCountPerCamera[cam->GetHash()]);
    }
}

void SceneGraph::UpdateDrawSurface(ionSize _cameraHash, const Matrix& _projection, const Matrix& _view, const EntityHandle& _entity, ionU32 _index)
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

void SceneGraph::Update(ionFloat _deltaTime)
{
    // update
    m_rootHandle->Update(_deltaTime);

    // mapping
    ionVertexCacheManager().BeginMapping();
    ionU32 index = 0;
    for (eosMap(CameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        const CameraHandle& cam = iter->first;

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

void SceneGraph::Render(RenderCore& _renderCore, ionU32 _width, ionU32 _height)
{
    for (eosMap(CameraHandle, eosVector(EntityHandle))::iterator iter = m_treeNodes.begin(); iter != m_treeNodes.end(); ++iter)
    {
        const CameraHandle& cam = iter->first;

        cam->StartRenderPass(_renderCore, 1.0f, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 1.0f, 1.0f);
        cam->SetViewport(_renderCore, 0, 0, _width, _height, 1.0f, 0.0f, 1.0f);
        cam->SetScissor(_renderCore, 0, 0, _width, _height, 1.0f);

        const eosVector(DrawSurface)& surfaces = m_drawSurfaces[cam->GetHash()];

        eosVector(DrawSurface)::const_iterator begin = surfaces.cbegin(), end = surfaces.cend(), it = begin;
        for (; it != end; ++it)
        {
            const DrawSurface& drawSuraface = (*it);

            _renderCore.SetState(drawSuraface.m_material->GetState().GetStateBits());
            _renderCore.Draw(drawSuraface);
        }

        cam->EndRenderPass(_renderCore);
    }
}

void SceneGraph::SetActiveInputNode(const NodeHandle& _node)
{
    SetActiveInputNode(_node->GetHash());
}

void SceneGraph::SetActiveInputNode(ionSize _nodeHash)
{
    eosMap(ionSize, NodeHandle)::iterator itSelected = m_hashToNodeMap.find(_nodeHash);
    if (itSelected != m_hashToNodeMap.end())
    {
        m_activeInputHashNode = _nodeHash;
        return;
    }
}

void SceneGraph::UpdateMouseInputActiveNode(ionFloat _x, ionFloat _y)
{
    ionAssertReturnVoid(m_activeInputHashNode != 0, "There is no active camera object");
    m_hashToNodeMap[m_activeInputHashNode]->ProcessMouseMovement(_x, _y);
}

ION_NAMESPACE_END