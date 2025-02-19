// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\Entity.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Node.h"

#include "../Geometry/BoundingBox.h"
#include "../Geometry/Mesh.h"
#include "../Geometry/MeshRenderer.h"


#define ION_BASE_ENTITY_NAME "Entity"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using EntityAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class AnimationRenderer;
class ION_DLL Entity : public Node
{
public:
	static EntityAllocator* GetAllocator();

public:
    Entity();
    explicit Entity(const ionString & _name);
    virtual ~Entity();

    template<typename T>
    T* AddMeshRenderer()
    {
        m_boundingBox.Reset();
        m_meshes.clear();
        if (m_meshRenderer != nullptr)
        {
            ionDelete(m_meshRenderer, GetAllocator());
            m_meshRenderer = nullptr;
        }
        m_meshRenderer = ionNew(T, GetAllocator());
        return dynamic_cast<T*>(m_meshRenderer);
    }

    AnimationRenderer* AddAnimationRenderer();

    virtual const AnimationRenderer* GetAnimationRenderer() const override final { return m_animationRenderer; }
    virtual AnimationRenderer* GetAnimationRenderer()  override final { return m_animationRenderer; }

    virtual const BaseMeshRenderer* GetMeshRenderer() const override final { return m_meshRenderer; }
    virtual BaseMeshRenderer* GetMeshRenderer() override final { return m_meshRenderer; }

    virtual void PushBackMesh(const Mesh& _mesh) override final;
    virtual const Mesh* GetMesh(ionU32 _index) const override final { return &m_meshes[_index]; }
    virtual Mesh* GetMesh(ionU32 _index) override final { return &m_meshes[_index]; }
    virtual ionU32  GetMeshCount() const override final;

    virtual void PushBackInitialMorphTargetWeight(ionFloat _weight) override final;
    virtual ionFloat GetInitialMorphTargetWeight(ionU32 _index) const override final { return m_initialMorphTargetWeights[_index]; }
    virtual ionU32  GetInitialMorphTargetWeightCount() const override final;

    virtual void ResizeMorphTargetWeight(ionU32 _size) override final;
    virtual ionFloat GetMorphTargetWeight(ionU32 _index) const override final 
    { 
        return m_morphTargetWeights[_index];
    }
    virtual void SetMorphTargetWeight(ionU32 _index, ionFloat _value) override final 
    { 
        m_morphTargetWeights[_index] = _value; 
    }
    virtual ionU32 GetMorphTargetWeightCount() const override final;

    virtual BoundingBox* GetBoundingBox() override final { return &m_boundingBox; }

private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    BoundingBox				m_boundingBox;
    BaseMeshRenderer*       m_meshRenderer; // if has this one, means that this one is the root and any other children nodes can potentially contains meshes
    AnimationRenderer*      m_animationRenderer;
    ionVector<Mesh, EntityAllocator, GetAllocator>         m_meshes;
    ionVector<ionFloat, EntityAllocator, GetAllocator>     m_initialMorphTargetWeights;
    ionVector<ionFloat, EntityAllocator, GetAllocator>     m_morphTargetWeights;
};

ION_NAMESPACE_END