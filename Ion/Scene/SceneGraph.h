#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Node.h"
#include "Entity.h"
#include "Camera.h"

#include "../Geometry/BoundingBox.h"

#include "../App/Mode.h"

NIX_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Camera;
class RenderCore;
class ION_DLL SceneGraph final
{
public:
    explicit SceneGraph();
    ~SceneGraph();

    const BoundingBox& GetBoundingBox() { return m_sceneBoundingBox; }

    void AddToScene(ObjectHandler& _node);
    void RemoveFromScene(ObjectHandler& _node);

    void UpdateAllCameraAspectRatio(const RenderCore& _renderCore);

    void Prepare();
    void PrepareSurfaces();

    void Update(ionFloat _deltaTime);
    void Render(RenderCore& _renderCore, ionU32 _x, ionU32 _y, ionU32 _width, ionU32 _height);

    // Input
    void RegisterToInput(const ObjectHandler& _node);
    void UnregisterFromInput(const ObjectHandler& _node);

    void UpdateMouseInput(const MouseState& _mouseState, ionFloat _deltaTime);
    void UpdateKeyboardInput(const KeyboardState& _keyboardState, ionFloat _deltaTime);

private:
    SceneGraph(const SceneGraph& _Orig) = delete;
    SceneGraph& operator = (const SceneGraph&) = delete;

    void FillCameraMapTree(ObjectHandler& _node);
    void GenerateMapTree(ObjectHandler& _node);

    void UpdateDrawSurface(ionSize _cameraHash, ionU32 _index, const ObjectHandler& _entity);
    void UpdateUniformBuffer(Camera* _camera, ionU32 _index, const Matrix& _projection, const Matrix& _view, const ObjectHandler& _entity);

private:
    BoundingBox                             m_sceneBoundingBox;
    ObjectHandler                              m_rootHandle;
    eosMap(Camera*, eosVector(ObjectHandler))  m_treeNodes;
    eosMap(ionSize, eosVector(DrawSurface)) m_drawSurfaces;
    eosMap(ionSize, ionU32)                 m_nodeCountPerCamera;
    eosVector(ObjectHandler)                   m_registeredInput;
};

ION_NAMESPACE_END