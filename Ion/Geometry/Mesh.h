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

struct Primitive
{
    eosVector(Vertex)   m_vertexes;
    eosVector(Index)    m_indexes;
    Material*           m_material;

    Primitive()
    {
        m_material = nullptr;
    }

    ~Primitive()
    {
        m_vertexes.clear();
        m_indexes.clear();
        m_material = nullptr;
    }
};


class ION_DLL Mesh 
{
public:
    Mesh();
    ~Mesh();

    const eosVector(Primitive)& GetPrimitives() const;
    eosVector(Primitive)& GetPrimitives();

    const Primitive& GetPrimitive(ionU32 _index) const;
    Primitive& GetPrimitive(ionU32 _index);

    void AddPrimitive(const Primitive& _primitive);

    /*
    void PushBackVertex(const Vertex& _vertex);
    void PushBackIndex(const Index& _index);

	const eosVector(Vertex)& GetVertexList() const { return m_vertexes; }
	const eosVector(Index)& GetIndexList() const { return m_indexes; }
    */

protected:
    ionBool m_uniformDataChanged;

private:
    eosVector(Primitive)  m_primitives;
};

ION_NAMESPACE_END