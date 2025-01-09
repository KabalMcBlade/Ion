// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\SceneGraph.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Node.h"
#include "Entity.h"
#include "Camera.h"

#include "../Geometry/BoundingBox.h"

#include "../App/Mode.h"

#include "../Core/MemorySettings.h"


NIX_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using SceneGraphAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class UUID;
class RenderCore;
class DirectionalLight;
class ION_DLL SceneGraph final
{
public:
	static SceneGraphAllocator* GetAllocator();

public:
    SceneGraph();
    ~SceneGraph();

    const BoundingBox& GetBoundingBox() { return m_sceneBoundingBox; }

    void CreateDirectionalLightToScene();
    void DestroyDirectionalLightToScene();
    DirectionalLight* GetDirectionalLight();

    void AddToScene(Node* _node);
    void RemoveFromScene(Node* _node);
    void RemoveAll(const std::function< void(Node* _node) >& _lambda = nullptr);

    void UpdateAllCameraAspectRatio(RenderCore& _renderCore);

    void Begin();
    void End();

    void Update(ionFloat _deltaTime);
    void Render(RenderCore& _renderCore, ionU32 _x, ionU32 _y, ionU32 _width, ionU32 _height);

    // Input
    void RegisterToInput(Node* _node);
    void UnregisterFromInput(Node* _node);

    void UpdateMouseInput(const MouseState& _mouseState, ionFloat _deltaTime);
    void UpdateKeyboardInput(const KeyboardState& _keyboardState, ionFloat _deltaTime);

    // Utilities
    Node* GetObjectByName(const ionString& _name);
	Node* GetObjectByUUID(const UUID& _uuid);

private:
    SceneGraph(const SceneGraph& _Orig) = delete;
    SceneGraph& operator = (const SceneGraph&) = delete;

    void SortDrawSurfaces();

private:
    BoundingBox                                 m_sceneBoundingBox;
	DirectionalLight*							m_directionalLight;
	Node*										m_root;
    ionMap<Camera*, ionVector<DrawSurface, SceneGraphAllocator, GetAllocator>, SceneGraphAllocator, GetAllocator>     m_drawSurfaces;
    ionVector<Node*, SceneGraphAllocator, GetAllocator> m_registeredInput;
    ionBool                                     m_isMeshGeneratedFirstTime;  // is an helper
};

ION_NAMESPACE_END