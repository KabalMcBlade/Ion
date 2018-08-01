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


class ION_DLL BaseMesh
{
public:
    BaseMesh()
    {
        m_indexStart = 0;
        m_indexCount = 0;
        m_material = nullptr;
    }

    virtual ~BaseMesh()
    {
        m_indexes.clear();
        m_material = nullptr;
        m_indexStart = 0;
        m_indexCount = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    // virtual
    virtual EVertexLayout GetLayout() const 
    {
        return EVertexLayout_Empty;
    }

    virtual const void* GetVertexData() const
    {
        return nullptr;
    }

    virtual ionSize GetVertexSize() const
    {
        return -1;
    }

    virtual const void* GetJointData() const
    {
        return nullptr;
    }

    virtual ionSize GetJointSize() const
    {
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////
    // implementation
    
    //
    // Getter
    const void* GetIndexData() const
    {
        return m_indexes.data();
    }

    ionSize GetIndexSize() const
    {
        return m_indexes.size();
    }

    ionU32 GetIndexStart() const
    {
        return m_indexStart;
    }

    // this is the same as GetIndexSize() just more directly
    ionU32 GetIndexCount() const
    {
        return m_indexCount;
    }

    Material* GetMaterial() const
    {
        return m_material;
    }

    //
    // Setter
    void PushBackIndex(Index _index)
    {
        m_indexes.push_back(_index);
    }

    void SetIndexStart(ionU32 _start)
    {
        m_indexStart = _start;
    }

    void SetIndexCount(ionU32 _count)
    {
        m_indexCount = _count;
    }

    void SetMaterial(Material* _material)
    {
        m_material = _material;
    }


protected:
    // in the inherit class, declare the vertex type. 
    eosVector(Index)        m_indexes;
    ionU32                  m_indexStart;
    ionU32                  m_indexCount;
    Material*               m_material;
};


//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshPlain : public BaseMesh
{
public:
    MeshPlain() : BaseMesh()
    {
    }

    virtual ~MeshPlain()
    {
        m_vertexes.clear();
    }

    void PushBackVertex(const VertexPlain& _vertex)
    {
        m_vertexes.push_back(_vertex);
    }

    virtual EVertexLayout GetLayout() const override
    {
        return EVertexLayout_Pos;
    }

    virtual const void* GetVertexData() const override
    {
        return m_vertexes.data();
    }

    virtual ionSize GetVertexSize() const override
    {
        return m_vertexes.size();
    }

private:
    eosVector(VertexPlain)   m_vertexes;
};


//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshColored : public BaseMesh
{
public:
    MeshColored() : BaseMesh()
    {
    }

    virtual ~MeshColored()
    {
        m_vertexes.clear();
    }

    void PushBackVertex(const VertexColored& _vertex)
    {
        m_vertexes.push_back(_vertex);
    }

    virtual EVertexLayout GetLayout() const override
    {
        return EVertexLayout_Pos_Color;
    }

    virtual const void* GetVertexData() const override
    {
        return m_vertexes.data();
    }

    virtual ionSize GetVertexSize() const override
    {
        return m_vertexes.size();
    }

private:
    eosVector(VertexColored)   m_vertexes;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshUV : public BaseMesh
{
public:
    MeshUV() : BaseMesh()
    {
    }

    virtual ~MeshUV()
    {
        m_vertexes.clear();
    }

    void PushBackVertex(const VertexUV& _vertex)
    {
        m_vertexes.push_back(_vertex);
    }

    virtual EVertexLayout GetLayout() const override
    {
        return EVertexLayout_Pos_UV;
    }

    virtual const void* GetVertexData() const override
    {
        return m_vertexes.data();
    }

    virtual ionSize GetVertexSize() const override
    {
        return m_vertexes.size();
    }

private:
    eosVector(VertexUV)   m_vertexes;
};


//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshSimple : public BaseMesh
{
public:
    MeshSimple() : BaseMesh()
    {
    }

    virtual ~MeshSimple()
    {
        m_vertexes.clear();
    }

    void PushBackVertex(const VertexSimple& _vertex)
    {
        m_vertexes.push_back(_vertex);
    }

    virtual EVertexLayout GetLayout() const override
    {
        return EVertexLayout_Pos_UV;
    }

    virtual const void* GetVertexData() const override
    {
        return m_vertexes.data();
    }

    virtual ionSize GetVertexSize() const override
    {
        return m_vertexes.size();
    }

private:
    eosVector(VertexSimple)   m_vertexes;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL Mesh : public BaseMesh
{
public:
    Mesh() : BaseMesh()
    {
    }

    virtual ~Mesh()
    {
        m_vertexes.clear();
    }

    void PushBackVertex(const Vertex& _vertex)
    {
        m_vertexes.push_back(_vertex);
    }

    virtual EVertexLayout GetLayout() const override
    {
        return EVertexLayout_Full;
    }

    virtual const void* GetVertexData() const override
    {
        return m_vertexes.data();
    }

    virtual ionSize GetVertexSize() const override
    {
        return m_vertexes.size();
    }

    // special accessor for model

    Vertex& GetVertex(ionSize _index)
    {
        return m_vertexes[_index];
    }

    const Vertex& GetVertex(ionSize _index) const
    {
        return m_vertexes[_index];
    }

private:
    eosVector(Vertex)   m_vertexes;
};



/*
struct Mesh
{
    eosVector(Vertex)   m_vertexes;
    eosVector(Index)    m_indexes;
    VkIndexType         m_indexType;
    ionU32              m_indexStart;
    ionU32              m_indexCount;
    Material*           m_material;

    Mesh()
    {
        m_indexType = VK_INDEX_TYPE_UINT32;
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
*/

ION_NAMESPACE_END