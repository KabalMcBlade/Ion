#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "Mesh.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
class ION_DLL BaseModel
{
public:
    BaseModel();
    virtual ~BaseModel();

    EVertexLayout GetLayout() const;

    void PushBackMeshInfo(ionU32 _indexStart, ionU32 _indexCount, Material* _material);

    void PushBackIndex(const Index& _index);
    const void* GetIndexData() const;

    virtual const void* GetVertexData() const { return nullptr; }

protected:
    EVertexLayout       m_layout;

    eosVector(Index)    m_indices;

    eosVector(MeshPrototype) m_meshes;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL ModelPlain final : public BaseModel
{
public:
    ModelPlain();
    virtual ~ModelPlain();

    void PushBackVertex(const VertexPlain& _vertex);

    virtual const void* GetVertexData() const override final;

private:
    eosVector(VertexPlain)   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL ModelColored final : public BaseModel
{
public:
    ModelColored();
    virtual ~ModelColored();

    void PushBackVertex(const VertexColored& _vertex);

    virtual const void* GetVertexData() const override final;

private:
    eosVector(VertexColored)   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL ModelUV final : public BaseModel
{
public:
    ModelUV();
    virtual ~ModelUV();

    void PushBackVertex(const VertexUV& _vertex);

    virtual const void* GetVertexData() const override final;

private:
    eosVector(VertexUV)   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL ModelSimple final : public BaseModel
{
public:
    ModelSimple();
    virtual ~ModelSimple();

    void PushBackVertex(const VertexSimple& _vertex);

    virtual const void* GetVertexData() const override final;

private:
    eosVector(VertexSimple)   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL Model final : public BaseModel
{
public:
    Model();
    virtual ~Model();

    void PushBackVertex(const Vertex& _vertex);

    virtual const void* GetVertexData() const override final;

private:
    eosVector(Vertex)   m_vertices;
};


ION_NAMESPACE_END