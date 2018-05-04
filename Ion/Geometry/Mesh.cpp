#include "Mesh.h"

#include "../Texture/Texture.h"

ION_NAMESPACE_BEGIN

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::PushBackVertex(const Vertex& _vertex)
{
    m_vertexes.push_back(_vertex);
}

void Mesh::PushBackIndex(const Index& _index)
{
    m_indexes.push_back(_index);
}

ION_NAMESPACE_END
