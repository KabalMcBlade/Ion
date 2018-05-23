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

ionU32 Entity::GetPrimitiveCount(ionU32 _meshIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", -1);

    return static_cast<ionU32>(m_meshes[_meshIndex].GetPrimitives().size());
}

const void* Entity::GetVertexBuffer(ionU32 _meshIndex, ionU32 _primitiveIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);
    ionAssertReturnValue(_primitiveIndex >= 0 && _primitiveIndex < m_meshes[_meshIndex].GetPrimitives().size(), "Primitive index out of bound", nullptr);

    return m_meshes[_meshIndex].GetPrimitive(_primitiveIndex).m_vertexes.data();
}

ionSize Entity::GetVertexBufferSize(ionU32 _meshIndex, ionU32 _primitiveIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", -1);
    ionAssertReturnValue(_primitiveIndex >= 0 && _primitiveIndex < m_meshes[_meshIndex].GetPrimitives().size(), "Primitive index out of bound", -1);

    return m_meshes[_meshIndex].GetPrimitive(_primitiveIndex).m_vertexes.size();
}

const void* Entity::GetIndexBuffer(ionU32 _meshIndex, ionU32 _primitiveIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);
    ionAssertReturnValue(_primitiveIndex >= 0 && _primitiveIndex < m_meshes[_meshIndex].GetPrimitives().size(), "Primitive index out of bound", nullptr);

    return m_meshes[_meshIndex].GetPrimitive(_primitiveIndex).m_indexes.data();
}

ionSize Entity::GetIndexBufferSize(ionU32 _meshIndex, ionU32 _primitiveIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", -1);
    ionAssertReturnValue(_primitiveIndex >= 0 && _primitiveIndex < m_meshes[_meshIndex].GetPrimitives().size(), "Primitive index out of bound", -1);

    return m_meshes[_meshIndex].GetPrimitive(_primitiveIndex).m_indexes.size();
}

const Material* Entity::GetMaterial(ionU32 _meshIndex, ionU32 _primitiveIndex) const
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);
    ionAssertReturnValue(_primitiveIndex >= 0 && _primitiveIndex < m_meshes[_meshIndex].GetPrimitives().size(), "Primitive index out of bound", nullptr);

    return m_meshes[_meshIndex].GetPrimitive(_primitiveIndex).m_material;
}

Material* Entity::GetMaterial(ionU32 _meshIndex, ionU32 _primitiveIndex)
{
    ionAssertReturnValue(_meshIndex >= 0 && _meshIndex < m_meshes.size(), "Mesh index out of bound", nullptr);
    ionAssertReturnValue(_primitiveIndex >= 0 && _primitiveIndex < m_meshes[_meshIndex].GetPrimitives().size(), "Primitive index out of bound", nullptr);

    return m_meshes[_meshIndex].GetPrimitive(_primitiveIndex).m_material;
}

/*
void* Entity::GetJointBuffer(ionU32 _meshIndex, ionU32 _primitiveIndex) const
{

}

ionSize  Entity::GetJointBufferSize(ionU32 _meshIndex, ionU32 _primitiveIndex) const
{

}
*/

ION_NAMESPACE_END