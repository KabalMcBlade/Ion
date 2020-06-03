#include "SceneGraph.h"

#include "../Renderer/RenderCore.h"
#include "../Renderer/VertexCacheManager.h"

#include "../Renderer/RenderCommon.h"

#include "../Renderer/RenderManager.h"

#include "../Scene/DirectionalLight.h"

#include "../Animation/AnimationRenderer.h"

#include "../Core/UUID.h"

NIX_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


SceneGraphAllocator* SceneGraph::GetAllocator()
{
	static HeapArea<Settings::kSceneGraphAllocatorSize> memoryArea;
	static SceneGraphAllocator memoryAllocator(memoryArea, "SceneGraphFreeListAllocator");

	return &memoryAllocator;
}


SceneGraph::SceneGraph()
{
    m_root = CreateNode(Node, "ION_SCENEGRAPH_ROOT");
}

SceneGraph::~SceneGraph()
{
    DestroyDirectionalLightToScene();

    for (ionMap<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>, SceneGraphAllocator, GetAllocator>::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>& drawSurfaces = iter->second;
        drawSurfaces.clear();
    }
    m_drawSurfaces.clear();

    m_registeredInput.clear();

	DestroyNode(m_root);
}

void SceneGraph::CreateDirectionalLightToScene()
{
    if (m_directionalLight == nullptr)
    {
        m_directionalLight = ionNew(DirectionalLight, GetAllocator(), "MainDirectionalLight");
    }
}

void SceneGraph::DestroyDirectionalLightToScene()
{
    if (m_directionalLight != nullptr)
    {
        ionDelete(m_directionalLight, GetAllocator());
		m_directionalLight = nullptr;
    }
}
DirectionalLight* SceneGraph::GetDirectionalLight()
{
    return m_directionalLight;
}

void SceneGraph::AddToScene(Node* _node)
{
    _node->AttachToParent(m_root);
}

void SceneGraph::RemoveFromScene(Node* _node)
{
    _node->DetachFromParent();
}

void SceneGraph::RemoveAll(const std::function< void(Node* _node) >& _lambda /*= nullptr*/)
{
    ionVector<Node*, NodeAllocator, Node::GetAllocator> myCopy = m_root->GetChildren();
	ionVector<Node*, NodeAllocator, Node::GetAllocator>::const_iterator it = myCopy.begin();
    while (it != myCopy.end())
    {
        if (_lambda != nullptr)
        {
            _lambda((*it));
        }

        ++it;
    }

    // do nothing here: maybe add later a "detach all" ?
}

void SceneGraph::UpdateAllCameraAspectRatio(RenderCore& _renderCore)
{
    for (ionMap<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>, SceneGraphAllocator, GetAllocator>::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        Camera* cam = iter->first;

        cam->RecreateRenderPassAndFrameBuffers(_renderCore);
        cam->UpdateAspectRatio((ionFloat)_renderCore.GetWidth() / (ionFloat)_renderCore.GetHeight());
    }
}

void SceneGraph::Begin()
{
    // I do 2 iterations for clearness

    // first camera
    m_root->IterateAll(
        [&](Node* _node)
    {
        if (_node->GetNodeType() == ENodeType_Camera)
        {
            Camera* cam = dynamic_cast<Camera*>(_node);
            if (m_drawSurfaces.find(cam) == m_drawSurfaces.end())
            {
				ionVector<DrawSurface, SceneGraphAllocator, GetAllocator> drawSurfacesVector;
				std::pair<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>> pairCamPerDrawsurfaces(cam, drawSurfacesVector);
				m_drawSurfaces.insert(pairCamPerDrawsurfaces);
				//m_drawSurfaces->insert(std::pair<Camera*, ionVector<DrawSurface>>(cam, ionVector<DrawSurface>()));
            }
        }
    }
    );

    // second objects
    m_root->IterateAll(
        [&](Node* _node)
    {
        if (_node->GetNodeType() == ENodeType_Entity)
        {
            for (ionMap<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>, SceneGraphAllocator, GetAllocator>::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
            {
                Camera* cam = iter->first;

                if (_node->IsInRenderLayer(cam->GetRenderLayer()))
                {
                    // is the root of the mesh (because just the "root" has the mesh renderer)
                    const BaseMeshRenderer* renderer = _node->GetMeshRenderer();
                    if (renderer != nullptr)
                    {
                        m_isMeshGeneratedFirstTime = false;

                        DrawSurface drawSurface;

                        drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(renderer->GetVertexData(), renderer->GetVertexDataCount(), renderer->GetSizeOfVertex());
                        drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(renderer->GetIndexData(), renderer->GetIndexDataCount(), renderer->GetSizeOfIndex());

                        m_drawSurfaces[cam].push_back(drawSurface);
                    }

                    const ionU32 meshCount = _node->GetMeshCount();
                    if (meshCount > 0)
                    {
                        for (ionU32 i = 0; i < meshCount; i++)
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


                            drawSurface->m_nodeRef = _node;
                            drawSurface->m_visible = _node->IsVisible();    // this one is updated x frame, just set for the beginning
                            drawSurface->m_meshIndexRef = i;
                            drawSurface->m_indexStart = _node->GetMesh(i)->GetIndexStart();
                            drawSurface->m_indexCount = _node->GetMesh(i)->GetIndexCount();
                            drawSurface->m_material = _node->GetMesh(i)->GetMaterial();
                            drawSurface->m_sortingIndex = static_cast<ionU8>(drawSurface->m_material->GetAlphaMode());

                            BoundingBox* bb = _node->GetBoundingBox();
                            m_sceneBoundingBox.Expande(bb->GetTransformed(_node->GetTransform().GetMatrix()));
                        }
                    }
                }
            }
        }
    }
    );
    m_isMeshGeneratedFirstTime = false;

    SortDrawSurfaces();

    m_root->IterateAll(
        [&](Node* _node)
    {
        _node->OnBegin();
    }
    );
}

void SceneGraph::End()
{  
    m_root->IterateAll(
        [&](Node* _node)
    {
        _node->OnEnd();
    }
    );
}

void SceneGraph::SortDrawSurfaces()
{
    for (ionMap<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>, SceneGraphAllocator, GetAllocator>::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>& drawSurfaces = iter->second;

        ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>::size_type miniPos;
        for (ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>::size_type i = 0; i < drawSurfaces.size(); ++i)
        {
            miniPos = i;
            for (ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>::size_type j = i + 1; j < drawSurfaces.size(); ++j)
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
    m_root->Update(_deltaTime);

    // mapping
    //ionVertexCacheManager().BeginMapping();
    for (ionMap<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>, SceneGraphAllocator, GetAllocator>::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        Camera* cam = iter->first;

        cam->UpdateView();  // here is updated the sky box either

        const Matrix4x4& projection = cam->GetPerspectiveProjection();
        const Matrix4x4& view = cam->GetView();

        const Vector4& cameraPos = cam->GetTransform().GetPosition();

        ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>& drawSurfaces = iter->second;
        ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>::iterator beginDS = drawSurfaces.begin(), endDS = drawSurfaces.end(), itDS = beginDS;
        for (; itDS != endDS; ++itDS)
        {
            DrawSurface& drawSurface = (*itDS);

            drawSurface.m_viewMatrix = view;
            drawSurface.m_projectionMatrix = projection;
            drawSurface.m_mainCameraPos = cameraPos;

            if (m_directionalLight != nullptr)
            {
                drawSurface.m_directionalLight = m_directionalLight->GetLightDirection();
                drawSurface.m_directionalLightColor = m_directionalLight->GetColor();
            }

            drawSurface.m_exposure = ionRenderManager().m_exposure;
            drawSurface.m_gamma = ionRenderManager().m_gamma;
            drawSurface.m_prefilteredCubeMipLevels = ionRenderManager().m_prefilteredCubeMipLevels;


            // relative to the nodes
            drawSurface.m_modelMatrix = drawSurface.m_nodeRef->GetTransform().GetMatrixWS();

            drawSurface.m_visible = drawSurface.m_nodeRef->IsVisible();
        }
    }
    //ionVertexCacheManager().EndMapping();
}

void SceneGraph::Render(RenderCore& _renderCore, ionU32 _x, ionU32 _y, ionU32 _width, ionU32 _height)
{
    for (ionMap<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>, SceneGraphAllocator, GetAllocator>::iterator iter = m_drawSurfaces.begin(); iter != m_drawSurfaces.end(); ++iter)
    {
        Camera* cam = iter->first;

        cam->ConputeRenderAreaViewportScissor(_x, _y, _width, _height);
        cam->StartRenderPass(_renderCore);

        cam->SetViewport(_renderCore);
        cam->SetScissor(_renderCore);

        cam->RenderSkybox(_renderCore);

        const ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>& surfaces = iter->second;

        ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>::const_iterator begin = surfaces.cbegin(), end = surfaces.cend(), it = begin;
        for (; it != end; ++it)
        {
            const DrawSurface& drawSurface = (*it);
            
            if (drawSurface.m_visible)
            {
                AnimationRenderer* animationRenderer = drawSurface.m_nodeRef->GetAnimationRenderer();
                if (animationRenderer != nullptr)
                {
                    if (animationRenderer->IsEnabled())
                    {
                        animationRenderer->Draw(drawSurface.m_nodeRef);
                    }
                }

                _renderCore.SetState(drawSurface.m_material->GetState().GetStateBits());
                _renderCore.Draw(cam->GetRenderPass(), drawSurface);
            }
        }

        cam->EndRenderPass(_renderCore);
    }
}

void SceneGraph::RegisterToInput(Node*_node)
{
    m_registeredInput.push_back(_node);
}

void SceneGraph::UnregisterFromInput(Node* _node)
{
    //m_registeredInput->erase(std::remove(m_registeredInput->begin(), m_registeredInput->end(), _node), m_registeredInput->end());
    std::remove(m_registeredInput.begin(), m_registeredInput.end(), _node);
}

void SceneGraph::UpdateMouseInput(const MouseState& _mouseState, ionFloat _deltaTime)
{
    ionVector<Node*, SceneGraphAllocator, GetAllocator>::const_iterator begin = m_registeredInput.cbegin(), end = m_registeredInput.cend(), it = begin;
    for (; it != end; ++it)
    {
		(*it)->OnMouseInput(_mouseState, _deltaTime);
    }
}

void SceneGraph::UpdateKeyboardInput(const KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    ionVector<Node*, SceneGraphAllocator, GetAllocator>::const_iterator begin = m_registeredInput.cbegin(), end = m_registeredInput.cend(), it = begin;
    for (; it != end; ++it)
    {
		(*it)->OnKeyboardInput(_keyboardState, _deltaTime);
    }
}

Node* SceneGraph::GetObjectByName(const ionString& _name)
{
	Node* nodeToFind = nullptr;   // default is empty/invalid

    m_root->IterateAll(
        [&](Node* _node)
    {
        if (_node->GetName() == _name)
        {
            nodeToFind = _node;
            return;
        }
    }
    );

    return nodeToFind;
}

Node* SceneGraph::GetObjectByUUID(const UUID& _uuid)
{
	Node* nodeToFind = nullptr;   // default is empty/invalid

    m_root->IterateAll(
        [&](Node* _node)
    {
        if (_node->GetUUID() == _uuid)
        {
            nodeToFind = _node;
            return;
        }
    }
    );

    return nodeToFind;
}


ION_NAMESPACE_END