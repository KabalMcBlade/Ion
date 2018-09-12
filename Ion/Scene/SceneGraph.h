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
class DirectionalLight;
class ION_DLL SceneGraph final
{
public:
    explicit SceneGraph();
    ~SceneGraph();

    const BoundingBox& GetBoundingBox() { return m_sceneBoundingBox; }

    void CreateDirectionalLightToScene();
    void DestroyDirectionalLightToScene();
    ObjectHandler& GetDirectionalLight();
    DirectionalLight* GetDirectionalLightPtr();

    void AddToScene(const ObjectHandler& _node);
    void RemoveFromScene(const ObjectHandler& _node);
    void RemoveAll();

    void UpdateAllCameraAspectRatio(const RenderCore& _renderCore);

    void Prepare();

    void Update(ionFloat _deltaTime);
    void Render(RenderCore& _renderCore, ionU32 _x, ionU32 _y, ionU32 _width, ionU32 _height);

    // Input
    void RegisterToInput(const ObjectHandler& _node);
    void UnregisterFromInput(const ObjectHandler& _node);

    void UpdateMouseInput(const MouseState& _mouseState, ionFloat _deltaTime);
    void UpdateKeyboardInput(const KeyboardState& _keyboardState, ionFloat _deltaTime);

    // Utilities
    ObjectHandler GetObjectByName(const eosString& _name);
    ObjectHandler GetObjectByHash(ionSize _hash);
    ObjectHandler GetObjectByID(ionU32 _id);

    // iterators
    eosVector(ObjectHandler) &GetNodes() { return m_nodes; };

    eosVector(ObjectHandler)::const_iterator GetNodeBegin() { return m_nodes.begin(); }
    eosVector(ObjectHandler)::const_iterator GetNodeEnd() { return m_nodes.end(); }

private:
    SceneGraph(const SceneGraph& _Orig) = delete;
    SceneGraph& operator = (const SceneGraph&) = delete;

    void FillCameraMapTree(const ObjectHandler& _node);
    void GenerateMapTree(const ObjectHandler& _node);

    void SortDrawSurfaces();

    // Utilities
    ObjectHandler GetObjectByHash(ObjectHandler& _node, ionSize _hash);
    ObjectHandler GetObjectByID(ObjectHandler& _node, ionU32 _id);

private:
    BoundingBox                                 m_sceneBoundingBox;
    ObjectHandler                               m_directionalLight;
    eosVector(ObjectHandler)                    m_nodes;
    eosMap(Camera*, eosVector(DrawSurface))     m_drawSurfaces;
    eosVector(ObjectHandler)                    m_registeredInput;
};

ION_NAMESPACE_END