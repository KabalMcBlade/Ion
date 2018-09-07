#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "Mesh.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

template<class T>
class ION_DLL Model
{
public:
    void PushBackIndex(const Index& _index);
    void PushBackVertex(const T& _vertex);

    const void* GetVertexData() const;
    const void* GetIndexData() const;

private:
    EVertexLayout       m_layout;

    eosVector(Index)    m_indices;
    eosVector(T)        m_vertices;

    eosVector(MeshPrototype) m_meshes;
};

template<class T>
void Model<T>::PushBackIndex(const Index& _index)
{
    m_indices.push_back(_index);
}

template<class T>
void Model<T>::PushBackVertex(const T& _vertex)
{
    m_vertices.push_back(_vertex);
}

template<class T>
const void* Model<T>::GetVertexData() const
{
    return m_vertices.data();
}

template<class T>
const void* Model<T>::GetIndexData() const
{
    return m_indices.data();
}

ION_NAMESPACE_END