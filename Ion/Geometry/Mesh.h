#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/IndexBufferObject.h"
#include "../Renderer/VertexBufferObject.h"

#include "../Material/Material.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

struct Mesh
{
    eosVector(Vertex)   m_vertexes;
    eosVector(Index)    m_indexes;
    ionU32              m_indexStart;
    ionU32              m_indexCount;
    Material*           m_material;

    Mesh()
    {
        m_indexStart = 0;
        m_indexCount = 0;
        m_material = nullptr;
    }

    ~Mesh()
    {
        m_vertexes.clear();
        m_indexes.clear();
        m_material = nullptr;
    }
};

ION_NAMESPACE_END