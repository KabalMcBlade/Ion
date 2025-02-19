// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\Entity.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "Entity.h"

#include "../Material/Material.h"

#include "../Animation/Animation.h"
#include "../Animation/AnimationRenderer.h"


ION_NAMESPACE_BEGIN

EntityAllocator* Entity::GetAllocator()
{
	static HeapArea<Settings::kEntityAllocatorSize> memoryArea;
	static EntityAllocator memoryAllocator(memoryArea, "EntityFreeListAllocator");

	return &memoryAllocator;
}


Entity::Entity() : Node(ION_BASE_ENTITY_NAME), m_meshRenderer(nullptr), m_animationRenderer(nullptr)
{
    m_nodeType = ENodeType_Entity;
}

Entity::Entity(const ionString& _name) : Node(_name), m_meshRenderer(nullptr), m_animationRenderer(nullptr)
{
    m_nodeType = ENodeType_Entity;
}

Entity::~Entity()
{
    m_meshes.clear();
    m_initialMorphTargetWeights.clear();

	if (m_animationRenderer != nullptr)
	{
		ionDelete(m_animationRenderer, GetAllocator());
		m_animationRenderer = nullptr;
	}

    if (m_meshRenderer != nullptr)
    {
        ionDelete(m_meshRenderer, GetAllocator());
        m_meshRenderer = nullptr;
    }
}

void Entity::PushBackMesh(const Mesh& _mesh)
{
    m_meshes.push_back(_mesh);
}

ionU32 Entity::GetMeshCount() const
{
    return static_cast<ionU32>(m_meshes.size());
}

AnimationRenderer* Entity::AddAnimationRenderer()
{
    if (m_animationRenderer != nullptr)
    {
        ionDelete(m_animationRenderer, GetAllocator());
        m_animationRenderer = nullptr;
    }
    m_animationRenderer = ionNew(AnimationRenderer, GetAllocator());
    return m_animationRenderer;
}

void Entity::PushBackInitialMorphTargetWeight(ionFloat _weight)
{
    m_initialMorphTargetWeights.push_back(_weight);
}

ionU32 Entity::GetInitialMorphTargetWeightCount() const
{
    return static_cast<ionU32>(m_initialMorphTargetWeights.size());
}

void Entity::ResizeMorphTargetWeight(ionU32 _size)
{
    //should work that resize of the same size, actually do nothing...
    m_morphTargetWeights.resize(_size);
}

ionU32 Entity::GetMorphTargetWeightCount() const
{
    return static_cast<ionU32>(m_morphTargetWeights.size());
}


ION_NAMESPACE_END