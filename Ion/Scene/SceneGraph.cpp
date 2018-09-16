#include "SceneGraph.h"

#include "../Renderer/RenderCore.h"
#include "../Renderer/VertexCacheManager.h"

#include "../Renderer/RenderCommon.h"

#include "../Renderer/RenderManager.h"

#include "../Scene/DirectionalLight.h"

NIX_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

SceneGraph::SceneGraph()
{
    
}

SceneGraph::~SceneGraph()
{
    DestroyDirectionalLightToScene();

    for (eosMap(Camera*, eosVector(DrawSurface))::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        eosVector(DrawSurface)& drawSurfaces = iter->second;
        drawSurfaces.clear();
    }
    m_drawSurfaces.clear();

    m_registeredInput.clear();
}

void SceneGraph::CreateDirectionalLightToScene()
{
    if (!m_directionalLight.IsValid())
    {
        m_directionalLight = eosNew(DirectionalLight, ION_MEMORY_ALIGNMENT_SIZE, "MainDirectionalLight");
    }
}

void SceneGraph::DestroyDirectionalLightToScene()
{
    if (m_directionalLight.IsValid())
    {
        m_directionalLight.Release();
    }
}

ObjectHandler& SceneGraph::GetDirectionalLight()
{
    return m_directionalLight;
}

DirectionalLight* SceneGraph::GetDirectionalLightPtr()
{
    return dynamic_cast<DirectionalLight*>(m_directionalLight.GetPtr());
}

void SceneGraph::AddToScene(const ObjectHandler& _node)
{
    m_nodes.push_back(_node);
}

void SceneGraph::RemoveFromScene(const ObjectHandler& _node)
{
    m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), _node), m_nodes.end());
}

void SceneGraph::RemoveAll()
{
    m_nodes.clear();
}

void SceneGraph::UpdateAllCameraAspectRatio(const RenderCore& _renderCore)
{
    for (eosMap(Camera*, eosVector(DrawSurface))::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        Camera* cam = iter->first;

        cam->UpdateAspectRatio((ionFloat)_renderCore.GetWidth() / (ionFloat)_renderCore.GetHeight());
    }
}

void SceneGraph::FillCameraMapTree(const ObjectHandler& _node)
{
    if (_node->GetNodeType() == ENodeType_Camera)
    {
        Camera* cam = dynamic_cast<Camera*>(_node.GetPtr());

        if (m_drawSurfaces.find(cam) == m_drawSurfaces.end())
        {
            m_drawSurfaces.insert(std::pair<Camera*, eosVector(DrawSurface)>(cam, eosVector(DrawSurface)()));
        }
    }

    if (_node->GetChildren().empty())
    {
        return;
    }

    const eosVector(ObjectHandler)& children = _node->GetChildren();
    eosVector(ObjectHandler)::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;
    for (; it != end; ++it)
    {
        ObjectHandler nh = (*it);
        FillCameraMapTree(nh);
    }
}

void SceneGraph::GenerateMapTree(const ObjectHandler& _node)
{
    if (_node->GetNodeType() == ENodeType_Entity)
    {
        for (eosMap(Camera*, eosVector(DrawSurface))::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
        {
            Camera* cam = iter->first;

            if (_node->IsInRenderLayer(cam->GetRenderLayer()))
            {
                ObjectHandler entity = _node;

                // is the root of the mesh (because just the "root" has the mesh renderer)
                const BaseMeshRenderer* renderer = entity->GetMeshRenderer();
                if (renderer != nullptr)
                {
                    DrawSurface drawSurface;

                    drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(renderer->GetVertexData(), renderer->GetVertexDataCount(), renderer->GetSizeOfVertex());
                    drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(renderer->GetIndexData(), renderer->GetIndexDataCount());
                    
                    m_drawSurfaces[cam].push_back(drawSurface);
                }

                const ionU32 meshCount = entity->GetMeshCount();
                if (meshCount > 0)
                {
                    DrawSurface* drawSurface = nullptr;
                    if (!m_isMeshGeneratedFirstTime)
                    {
                        drawSurface = &m_drawSurfaces[cam].back();
                        m_isMeshGeneratedFirstTime = true;
                    }
                    else
                    {
                        const DrawSurface& drawSurfacePrev = m_drawSurfaces[cam].back();

                        DrawSurface drawSurfaceTmp;
                        drawSurfaceTmp.m_vertexCache = drawSurfacePrev.m_vertexCache;
                        drawSurfaceTmp.m_indexCache = drawSurfacePrev.m_indexCache;

                        m_drawSurfaces[cam].push_back(drawSurfaceTmp);

                        drawSurface = &m_drawSurfaces[cam].back();
                    }

                    drawSurface->m_nodeRef = entity.GetPtr();
                    drawSurface->m_visible = entity->IsVisible();    // this one is updated x frame, just set for the beginning

                    BoundingBox* bb = entity->GetBoundingBox();
                    m_sceneBoundingBox.Expande(bb->GetTransformed(entity->GetTransform().GetMatrix()));

                    for (ionU32 i = 0; i < meshCount; i++)
                    {
                        drawSurface->m_meshIndexRef = i;
                        drawSurface->m_indexStart = entity->GetMesh(i)->GetIndexStart();
                        drawSurface->m_indexCount = entity->GetMesh(i)->GetIndexCount();
                        drawSurface->m_material = entity->GetMesh(i)->GetMaterial();
                        drawSurface->m_sortingIndex = static_cast<ionU8>(drawSurface->m_material->GetAlphaMode());
                    }
                }
            }
        }
    }

    if (_node->GetChildren().empty())
    {
        return;
    }

    const eosVector(ObjectHandler)& children = _node->GetChildren();
    eosVector(ObjectHandler)::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;
    for (; it != end; ++it)
    {
        ObjectHandler nh = (*it);
        GenerateMapTree(nh);
    }
}


void SceneGraph::Prepare()
{
    // Generate the plain map recursively
    // I don't care about the speed here, is just once before start
    eosVector(ObjectHandler)::const_iterator begin = m_nodes.cbegin(), end = m_nodes.cend(), it = begin;
    for (; it != end; ++it)
    {
        const ObjectHandler& node = (*it);
        FillCameraMapTree(node);

        m_isMeshGeneratedFirstTime = false;
        GenerateMapTree(node);
    }
    m_isMeshGeneratedFirstTime = false;

    SortDrawSurfaces();
}

void SceneGraph::SortDrawSurfaces()
{
    for (eosMap(Camera*, eosVector(DrawSurface))::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        eosVector(DrawSurface)& drawSurfaces = iter->second;

        eosVector(DrawSurface)::size_type miniPos;
        for (eosVector(DrawSurface)::size_type i = 0; i < drawSurfaces.size(); ++i)
        {
            miniPos = i;
            for (eosVector(DrawSurface)::size_type j = i + 1; j < drawSurfaces.size(); ++j)
            {
                if (drawSurfaces[j] < drawSurfaces[miniPos])
                {
                    miniPos = j;
                }
            }

            DrawSurface temp = drawSurfaces[miniPos];
            drawSurfaces[miniPos] = drawSurfaces[i];
            drawSurfaces[i] = temp;
        }
    }
}

void SceneGraph::Update(ionFloat _deltaTime)
{
    // update
    eosVector(ObjectHandler)::const_iterator begin = m_nodes.cbegin(), end = m_nodes.cend(), it = begin;
    for (; it != end; ++it)
    {
        const ObjectHandler& node = (*it);
        node->Update(_deltaTime);
    }

    // mapping
    //ionVertexCacheManager().BeginMapping();
    for (eosMap(Camera*, eosVector(DrawSurface))::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        Camera* cam = iter->first;

        cam->UpdateView();  // here is updated the skybox either

        const Matrix& projection = cam->GetPerspectiveProjection();
        const Matrix& view = cam->GetView();

        const Vector& cameraPos = cam->GetTransform().GetPosition();

        eosVector(DrawSurface)& drawSurfaces = iter->second;
        eosVector(DrawSurface)::iterator beginDS = drawSurfaces.begin(), endDS = drawSurfaces.end(), itDS = beginDS;
        for (; itDS != endDS; ++itDS)
        {
            DrawSurface& drawSurface = (*itDS);

            _mm_storeu_ps(&drawSurface.m_viewMatrix[0], view[0]);
            _mm_storeu_ps(&drawSurface.m_viewMatrix[4], view[1]);
            _mm_storeu_ps(&drawSurface.m_viewMatrix[8], view[2]);
            _mm_storeu_ps(&drawSurface.m_viewMatrix[12], view[3]);

            _mm_storeu_ps(&drawSurface.m_projectionMatrix[0], projection[0]);
            _mm_storeu_ps(&drawSurface.m_projectionMatrix[4], projection[1]);
            _mm_storeu_ps(&drawSurface.m_projectionMatrix[8], projection[2]);
            _mm_storeu_ps(&drawSurface.m_projectionMatrix[12], projection[3]);

            _mm_storeu_ps(&drawSurface.m_mainCameraPos[0], cameraPos);
            if (m_directionalLight.IsValid())
            {
                _mm_storeu_ps(&drawSurface.m_directionalLight[0], GetDirectionalLightPtr()->GetLightDirection());
                _mm_storeu_ps(&drawSurface.m_directionalLightColor[0], GetDirectionalLightPtr()->GetColor());
            }

            drawSurface.m_exposure = ionRenderManager().m_exposure;
            drawSurface.m_gamma = ionRenderManager().m_gamma;
            drawSurface.m_prefilteredCubeMipLevels = ionRenderManager().m_prefilteredCubeMipLevels;


            // relative to the nodes
            const Matrix& model = drawSurface.m_nodeRef->GetTransform().GetMatrixWS();

            drawSurface.m_visible = drawSurface.m_nodeRef->IsVisible();

            _mm_storeu_ps(&drawSurface.m_modelMatrix[0], model[0]);
            _mm_storeu_ps(&drawSurface.m_modelMatrix[4], model[1]);
            _mm_storeu_ps(&drawSurface.m_modelMatrix[8], model[2]);
            _mm_storeu_ps(&drawSurface.m_modelMatrix[12], model[3]);
        }
    }
    //ionVertexCacheManager().EndMapping();
}

void SceneGraph::Render(RenderCore& _renderCore, ionU32 _x, ionU32 _y, ionU32 _width, ionU32 _height)
{
    for (eosMap(Camera*, eosVector(DrawSurface))::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        Camera* cam = iter->first;

        cam->ConputeRenderAreaViewportScissor(_x, _y, _width, _height);
        cam->StartRenderPass(_renderCore);

        cam->SetViewport(_renderCore);
        cam->SetScissor(_renderCore);

        cam->RenderSkybox(_renderCore);

        const eosVector(DrawSurface)& surfaces = iter->second;

        eosVector(DrawSurface)::const_iterator begin = surfaces.cbegin(), end = surfaces.cend(), it = begin;
        for (; it != end; ++it)
        {
            const DrawSurface& drawSurface = (*it);
            
            if (drawSurface.m_visible)
            {
                _renderCore.SetState(drawSurface.m_material->GetState().GetStateBits());
                _renderCore.Draw(cam->GetRenderPass(), drawSurface);
            }
        }

        cam->EndRenderPass(_renderCore);
    }
}

void SceneGraph::RegisterToInput(const ObjectHandler& _node)
{
    m_registeredInput.push_back(_node);
}

void SceneGraph::UnregisterFromInput(const ObjectHandler& _node)
{
    //m_registeredInput.erase(std::remove(m_registeredInput.begin(), m_registeredInput.end(), _node), m_registeredInput.end());
    std::remove(m_registeredInput.begin(), m_registeredInput.end(), _node);
}

void SceneGraph::UpdateMouseInput(const MouseState& _mouseState, ionFloat _deltaTime)
{
    eosVector(ObjectHandler)::const_iterator begin = m_registeredInput.cbegin(), end = m_registeredInput.cend(), it = begin;
    for (; it != end; ++it)
    {
        const ObjectHandler& node = (*it);
        node->OnMouseInput(_mouseState, _deltaTime);
    }
}

void SceneGraph::UpdateKeyboardInput(const KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    eosVector(ObjectHandler)::const_iterator begin = m_registeredInput.cbegin(), end = m_registeredInput.cend(), it = begin;
    for (; it != end; ++it)
    {
        const ObjectHandler& node = (*it);
        node->OnKeyboardInput(_keyboardState, _deltaTime);
    }
}

ObjectHandler SceneGraph::GetObjectByHash(ObjectHandler& _node, ionSize _hash)
{
    if (_node->GetHash() == _hash)
    {
        return _node;
    }

    eosVector(ObjectHandler)::iterator begin = _node->GetChildIteratorBegin(), end = _node->GetChildIteratorEnd(), it = begin;
    for (; it != end; ++it)
    {
        ObjectHandler& node = *(it);
        return GetObjectByHash(node, _hash);
    }

    static ObjectHandler empty;
    return empty;
}

ObjectHandler SceneGraph::GetObjectByID(ObjectHandler& _node, ionU32 _id)
{
    if (_node->GetNodeIndex() == _id)
    {
        return _node;
    }

    eosVector(ObjectHandler)::iterator begin = _node->GetChildIteratorBegin(), end = _node->GetChildIteratorEnd(), it = begin;
    for (; it != end; ++it)
    {
        ObjectHandler& node = *(it);
        return GetObjectByID(node, _id);
    }

    static ObjectHandler empty;
    return empty;
}

ObjectHandler SceneGraph::GetObjectByName(const eosString& _name)
{
    const ionSize hash = std::hash<eosString>{}(_name);
    return GetObjectByHash(hash);
}

ObjectHandler SceneGraph::GetObjectByHash(ionSize _hash)
{
    eosVector(ObjectHandler)::const_iterator begin = GetNodeBegin(), end = GetNodeEnd(), it = begin;
    for (; it != end; ++it)
    {
        ObjectHandler node = *(it);

        ObjectHandler findNode = GetObjectByHash(node, _hash);
        if (findNode.IsValid())
        {
            return findNode;
        }
    }

    static ObjectHandler empty;
    return empty;
}

ObjectHandler SceneGraph::GetObjectByID(ionU32 _id)
{
    eosVector(ObjectHandler)::const_iterator begin = GetNodeBegin(), end = GetNodeEnd(), it = begin;
    for (; it != end; ++it)
    {
        ObjectHandler node = *(it);

        ObjectHandler findNode = GetObjectByID(node, _id);
        if (findNode.IsValid())
        {
            return findNode;
        }
    }

    static ObjectHandler empty;
    return empty;
}

ION_NAMESPACE_END