#include "Entity.h"

#include "../Material/Material.h"

ION_NAMESPACE_BEGIN

Entity::Entity() : Node(), m_meshRenderer(nullptr)
{
    m_meshes.clear();

    m_nodeType = ENodeType_Entity;
}

Entity::Entity(const eosString & _name) : Node(_name), m_meshRenderer(nullptr)
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

ION_NAMESPACE_END