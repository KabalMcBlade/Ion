#include "Mesh.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


Mesh::Mesh()
{
    m_indexStart = 0;
    m_indexCount = 0;
    m_material = nullptr;
}

Mesh::~Mesh()
{
    m_morphTargets.clear();
    m_material = nullptr;
    m_indexStart = 0;
    m_indexCount = 0;
}

ionU32 Mesh::GetIndexStart() const
{
    return m_indexStart;
}

ionU32 Mesh::GetIndexCount() const
{
    return m_indexCount;
}

Material* Mesh::GetMaterial() const
{
    return m_material;
}

void Mesh::SetIndexStart(ionU32 _start)
{
    m_indexStart = _start;
}

void Mesh::SetIndexCount(ionU32 _count)
{
    m_indexCount = _count;
}

void Mesh::SetMaterial(Material* _material)
{
    m_material = _material;
}


ION_NAMESPACE_END


