#include "Entity.h"

#include "../Material/Material.h"

#include "../Animation/Animation.h"
#include "../Animation/AnimationRenderer.h"


ION_NAMESPACE_BEGIN

Entity::Entity() : Node(), m_meshRenderer(nullptr), m_animationRenderer(nullptr)
{
    m_meshes.clear();

    m_nodeType = ENodeType_Entity;
}

Entity::Entity(const eosString & _name) : Node(_name), m_meshRenderer(nullptr), m_animationRenderer(nullptr)
{
    m_meshes.clear();

    m_nodeType = ENodeType_Entity;
}

Entity::~Entity()
{
    m_meshes.clear();
    if (m_meshRenderer != nullptr)
    {
        eosDelete(m_meshRenderer);
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
        eosDelete(m_animationRenderer);
        m_animationRenderer = nullptr;
    }
    m_animationRenderer = eosNew(AnimationRenderer, ION_MEMORY_ALIGNMENT_SIZE);
    return m_animationRenderer;
}

ION_NAMESPACE_END