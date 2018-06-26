#include "Entity.h"

#include "../Material/Material.h"

ION_NAMESPACE_BEGIN

Entity::Entity() : Node()
{
    m_meshes.resize(0);

    m_nodeType = ENodeType_Entity;
}

Entity::Entity(const eosString & _name) : Node(_name)
{
    m_meshes.resize(0);

    m_nodeType = ENodeType_Entity;
}

Entity::~Entity()
{
    auto begin = m_meshes.begin(), end = m_meshes.end();
    std::vector<BaseMesh*>::iterator it = begin;
    for (; it != end; ++it)
    {
        eosDelete((*it)); 
    }
    m_meshes.clear();
}

BoundingBox Entity::GetTransformedBoundingBox()
{
    return m_boundingBox.GetTransformed(GetTransformHandle()->GetMatrix());
}

ionU32 Entity::GetMeshCount() const
{
    return static_cast<ionU32>(m_meshes.size());
}

ION_NAMESPACE_END