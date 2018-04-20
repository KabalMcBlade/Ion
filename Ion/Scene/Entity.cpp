#include "Entity.h"


ION_NAMESPACE_BEGIN

Entity::Entity() : Node()
{
    m_meshes.resize(0);
}

Entity::Entity(const eosString & _name) : Node(_name)
{
    m_meshes.resize(0);
}

Entity::~Entity()
{
    m_meshes.clear();
}

ionBool Entity::LoadFromGLTF(const eosString & _fileName)
{
    return false;
}

ION_NAMESPACE_END