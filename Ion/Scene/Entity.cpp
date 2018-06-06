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
    m_meshes.clear();
}

BoundingBox Entity::GetTransformedBoundingBox()
{
    return m_boundingBox.GetTransformed(*GetTransformHandle());
}

ionU32 Entity::GetMeshCount() const
{
    return static_cast<ionU32>(m_meshes.size());
}

ionSize Entity::GetIndexStart(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", -1);

    return m_meshes[_meshIndex].m_indexStart;
}

ionSize Entity::GetIndexCount(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", -1);

    return m_meshes[_meshIndex].m_indexCount;
}

const void* Entity::GetVertexBuffer(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);

    return m_meshes[_meshIndex].m_vertexes.data();
}

ionSize Entity::GetVertexBufferSize(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", -1);

    return m_meshes[_meshIndex].m_vertexes.size();
}

const void* Entity::GetIndexBuffer(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);

    return m_meshes[_meshIndex].m_indexes.data();
}

ionSize Entity::GetIndexBufferSize(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", -1);
 
    return m_meshes[_meshIndex].m_indexes.size();
}

const Material* Entity::GetMaterial(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);

    return m_meshes[_meshIndex].m_material;
}

Material* Entity::GetMaterial(ionU32 _meshIndex)
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);

    return m_meshes[_meshIndex].m_material;
}

/*
void* Entity::GetJointBuffer(ionU32 _meshIndex) const
{

}

ionSize  Entity::GetJointBufferSize(ionU32 _meshIndex) const
{

}
*/

ION_NAMESPACE_END