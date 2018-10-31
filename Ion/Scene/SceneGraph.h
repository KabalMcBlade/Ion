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
    void RemoveAll(const std::function< void(const ObjectHandler& _node) >& _lambda = nullptr);

    void UpdateAllCameraAspectRatio(RenderCore& _renderCore);

    void Begin();
    void End();

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

private:
    SceneGraph(const SceneGraph& _Orig) = delete;
    SceneGraph& operator = (const SceneGraph&) = delete;

    void SortDrawSurfaces();

private:
    BoundingBox                                 m_sceneBoundingBox;
    ObjectHandler                               m_directionalLight;
    ObjectHandler                               m_root;
    eosMap(Camera*, eosVector(DrawSurface))     m_drawSurfaces;
    eosVector(ObjectHandler)                    m_registeredInput;
    ionBool                                     m_isMeshGeneratedFirstTime;  // is an helper
};

ION_NAMESPACE_END