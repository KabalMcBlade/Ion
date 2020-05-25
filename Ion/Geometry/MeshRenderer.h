#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

//#include "../Renderer/RenderCommon.h"
#include "../Renderer/IndexBufferObject.h"
#include "../Renderer/VertexBufferObject.h"

#include "../Material/Material.h"

#include "../Animation/Animation.h"

#include "../Core/MemorySettings.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////////////////

using MeshRendererAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class ION_DLL BaseMeshRenderer
{
public:
	static MeshRendererAllocator* GetAllocator();

public:
    BaseMeshRenderer();
    virtual ~BaseMeshRenderer();

    virtual EVertexLayout GetLayout() const;

    void PushBackIndex(const Index& _index);

    const ionU32 GetIndexDataCount() const;
    const void* GetIndexData() const;
    ionSize GetSizeOfIndex() const;

    virtual const ionU32 GetVertexDataCount() const { return 0; }
    virtual const void* GetVertexData() const { return nullptr; }
    virtual ionSize GetSizeOfVertex() const { return 0; }

    void PushBackMorphTarget(const VertexMorphTarget& _vertex);

    const ionU32 GetMorphTargetDataCount() const;
    const void* GetMorphTargetData() const;
    ionSize GetSizeOfMorphTarget() const;

    ionBool IsUsingMorphTarget() const { return m_morphTargets.size() > 0; }

protected:
    ionVector<Index, MeshRendererAllocator, BaseMeshRenderer::GetAllocator> m_indices;
    ionVector<VertexMorphTarget, MeshRendererAllocator, BaseMeshRenderer::GetAllocator> m_morphTargets;
};


//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshRendererPlain final : public BaseMeshRenderer
{
public:
    MeshRendererPlain();
    virtual ~MeshRendererPlain();

    virtual EVertexLayout GetLayout() const override final;

    void PushBackVertex(const VertexPlain& _vertex);

    virtual const ionU32 GetVertexDataCount() const override final;
    virtual const void* GetVertexData() const override final;
    virtual ionSize GetSizeOfVertex() const override final;

private:
    ionVector<VertexPlain, MeshRendererAllocator, BaseMeshRenderer::GetAllocator>   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshRendererColored final : public BaseMeshRenderer
{
public:
    MeshRendererColored();
    virtual ~MeshRendererColored();

    virtual EVertexLayout GetLayout() const override final;

    void PushBackVertex(const VertexColored& _vertex);

    virtual const ionU32 GetVertexDataCount() const override final;
    virtual const void* GetVertexData() const override final;
    virtual ionSize GetSizeOfVertex() const override final;

private:
    ionVector<VertexColored, MeshRendererAllocator, BaseMeshRenderer::GetAllocator>   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshRendererUV final : public BaseMeshRenderer
{
public:
    MeshRendererUV();
    virtual ~MeshRendererUV();

    virtual EVertexLayout GetLayout() const override final;

    void PushBackVertex(const VertexUV& _vertex);

    virtual const ionU32 GetVertexDataCount() const override final;
    virtual const void* GetVertexData() const override final;
    virtual ionSize GetSizeOfVertex() const override final;

private:
    ionVector<VertexUV, MeshRendererAllocator, BaseMeshRenderer::GetAllocator>   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshRendererNormal final : public BaseMeshRenderer
{
public:
    MeshRendererNormal();
    virtual ~MeshRendererNormal();

    virtual EVertexLayout GetLayout() const override final;

    void PushBackVertex(const VertexNormal& _vertex);

    virtual const ionU32 GetVertexDataCount() const override final;
    virtual const void* GetVertexData() const override final;
    virtual ionSize GetSizeOfVertex() const override final;

private:
    ionVector<VertexNormal, MeshRendererAllocator, BaseMeshRenderer::GetAllocator>   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshRendererSimple final : public BaseMeshRenderer
{
public:
    MeshRendererSimple();
    virtual ~MeshRendererSimple();

    virtual EVertexLayout GetLayout() const override final;

    void PushBackVertex(const VertexSimple& _vertex);

    virtual const ionU32 GetVertexDataCount() const override final;
    virtual const void* GetVertexData() const override final;
    virtual ionSize GetSizeOfVertex() const override final;

private:
    ionVector<VertexSimple, MeshRendererAllocator, BaseMeshRenderer::GetAllocator>   m_vertices;
};

//////////////////////////////////////////////////////////////////////////
class ION_DLL MeshRenderer final : public BaseMeshRenderer
{
public:
    MeshRenderer();
    virtual ~MeshRenderer();

    virtual EVertexLayout GetLayout() const override final;

    void PushBackVertex(const Vertex& _vertex);

    virtual const ionU32 GetVertexDataCount() const override final;
    virtual const void* GetVertexData() const override final;
    virtual ionSize GetSizeOfVertex() const override final;

    // Special accessor for this class only
    Vertex& GetVertex(ionSize _index);
    const Vertex& GetVertex(ionSize _index) const;

private:
    ionVector<Vertex, MeshRendererAllocator, BaseMeshRenderer::GetAllocator>   m_vertices;
};


ION_NAMESPACE_END