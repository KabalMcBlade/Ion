#include "Mesh.h"

#include "../Texture/Texture.h"

ION_NAMESPACE_BEGIN

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

/*
void Mesh::PushBackVertex(const Vertex& _vertex)
{
    m_vertexes.push_back(_vertex);
}

void Mesh::PushBackIndex(const Index& _index)
{
    m_indexes.push_back(_index);
}
*/

const eosVector(Primitive)& Mesh::GetPrimitives() const
{
    return m_primitives;
}

eosVector(Primitive)& Mesh::GetPrimitives()
{
    return m_primitives; 
}

const Primitive& Mesh::GetPrimitive(ionU32 _index) const
{
    ionAssert(_index >= 0 && _index < m_primitives.size(), "Index out of bound");
    return m_primitives[_index]; 
}

Primitive& Mesh::GetPrimitive(ionU32 _index)
{
    ionAssert(_index >= 0 && _index < m_primitives.size(), "Index out of bound");
    return m_primitives[_index]; 
}

void Mesh::AddPrimitive(const Primitive& _primitive)
{
    m_primitives.push_back(_primitive);
}

ION_NAMESPACE_END
