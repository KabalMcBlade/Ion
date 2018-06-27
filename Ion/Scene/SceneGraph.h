#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Node.h"
#include "Entity.h"
#include "Camera.h"

#include "../Geometry/BoundingBox.h"


NIX_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderCore;
class ION_DLL SceneGraph final
{
public:
    explicit SceneGraph();
    ~SceneGraph();

    const BoundingBox& GetBoundingBox() { return m_sceneBoundingBox; }

    void AddToScene(NodeHandle& _node);
    void RemoveFromScene(NodeHandle& _node);

    void UpdateAllCameraAspectRatio(const RenderCore& _renderCore);

    void Prepare();

    void Update(ionFloat _deltaTime);
    void Render(RenderCore& _renderCore, ionU32 _width, ionU32 _height);

    void UpdateMouseActiveCamera(ionFloat _x, ionFloat _y);

private:
    SceneGraph(const SceneGraph& _Orig) = delete;
    SceneGraph& operator = (const SceneGraph&) = delete;

    void FillCameraMapTree(NodeHandle& _node);
    void GenerateMapTree(NodeHandle& _node);

    void UpdateDrawSurface(ionSize _cameraHash, const Matrix& _projection, const Matrix& _view, const EntityHandle& _entity, ionU32 _index);

private:
    BoundingBox                                         m_sceneBoundingBox;
    NodeHandle                                          m_rootHandle;
    eosMap(CameraHandle, eosVector(EntityHandle))   m_treeNodes;
    eosMap(ionSize, eosVector(DrawSurface))             m_drawSurfaces;
    eosMap(ionSize, ionU32)                             m_nodeCountPerCamera;
};

ION_NAMESPACE_END