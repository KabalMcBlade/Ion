#include "Entity.h"

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
    m_meshes.clear();
}


ION_NAMESPACE_END