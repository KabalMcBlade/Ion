// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Geometry\MeshRenderer.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "MeshRenderer.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/IndexBufferObject.h"
#include "../Renderer/VertexBufferObject.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


MeshRendererAllocator* BaseMeshRenderer::GetAllocator()
{
	static HeapArea<Settings::kMeshRendererAllocatorSize> memoryArea;
	static MeshRendererAllocator memoryAllocator(memoryArea, "MeshRendererFreeListAllocator");

	return &memoryAllocator;
}


//////////////////////////////////////////////////////////////////////////
BaseMeshRenderer::BaseMeshRenderer()
{
}

BaseMeshRenderer::~BaseMeshRenderer()
{
    m_indices.clear();
}

void BaseMeshRenderer::PushBackIndex(const Index& _index)
{
    m_indices.push_back(_index);
}

const ionU32 BaseMeshRenderer::GetIndexDataCount() const
{
    return static_cast<ionU32>(m_indices.size());
}

const void* BaseMeshRenderer::GetIndexData() const
{
    return m_indices.data();
}

ionSize BaseMeshRenderer::GetSizeOfIndex() const
{
    return sizeof(Index);
}

EVertexLayout BaseMeshRenderer::GetLayout() const
{
    return EVertexLayout_Empty;
}

void BaseMeshRenderer::PushBackMorphTarget(const VertexMorphTarget& _vertex)
{
    m_morphTargets.push_back(_vertex);
}

const ionU32 BaseMeshRenderer::GetMorphTargetDataCount() const
{
    return static_cast<ionU32>(m_morphTargets.size());
}

const void* BaseMeshRenderer::GetMorphTargetData() const
{
    return m_morphTargets.data();
}

ionSize BaseMeshRenderer::GetSizeOfMorphTarget() const
{
    return sizeof(VertexMorphTarget);
}

//////////////////////////////////////////////////////////////////////////
MeshRendererPlain::MeshRendererPlain() : BaseMeshRenderer()
{
}

MeshRendererPlain::~MeshRendererPlain()
{
    m_vertices.clear();
}

void MeshRendererPlain::PushBackVertex(const VertexPlain& _vertex)
{
    m_vertices.push_back(_vertex);
}

const ionU32 MeshRendererPlain::GetVertexDataCount() const
{
    return static_cast<ionU32>(m_vertices.size());
}

const void* MeshRendererPlain::GetVertexData() const
{
    return m_vertices.data();
}

ionSize MeshRendererPlain::GetSizeOfVertex() const
{
    return sizeof(VertexPlain);
}

EVertexLayout MeshRendererPlain::GetLayout() const
{
    return EVertexLayout_Pos;
}

//////////////////////////////////////////////////////////////////////////
MeshRendererColored::MeshRendererColored() : BaseMeshRenderer()
{
}

MeshRendererColored::~MeshRendererColored()
{
    m_vertices.clear();
}

void MeshRendererColored::PushBackVertex(const VertexColored& _vertex)
{
    m_vertices.push_back(_vertex);
}

const ionU32 MeshRendererColored::GetVertexDataCount() const
{
    return static_cast<ionU32>(m_vertices.size());
}

const void* MeshRendererColored::GetVertexData() const
{
    return m_vertices.data();
}

ionSize MeshRendererColored::GetSizeOfVertex() const
{
    return sizeof(VertexColored);
}

EVertexLayout MeshRendererColored::GetLayout() const
{
    return EVertexLayout_Pos_Color;
}

//////////////////////////////////////////////////////////////////////////
MeshRendererUV::MeshRendererUV() : BaseMeshRenderer()
{
}

MeshRendererUV::~MeshRendererUV()
{
    m_vertices.clear();
}

void MeshRendererUV::PushBackVertex(const VertexUV& _vertex)
{
    m_vertices.push_back(_vertex);
}

const ionU32 MeshRendererUV::GetVertexDataCount() const
{
    return static_cast<ionU32>(m_vertices.size());
}

const void* MeshRendererUV::GetVertexData() const
{
    return m_vertices.data();
}

ionSize MeshRendererUV::GetSizeOfVertex() const
{
    return sizeof(VertexUV);
}

EVertexLayout MeshRendererUV::GetLayout() const
{
    return EVertexLayout_Pos_UV;
}

//////////////////////////////////////////////////////////////////////////
MeshRendererNormal::MeshRendererNormal() : BaseMeshRenderer()
{
}

MeshRendererNormal::~MeshRendererNormal()
{
    m_vertices.clear();
}

void MeshRendererNormal::PushBackVertex(const VertexNormal& _vertex)
{
    m_vertices.push_back(_vertex);
}

const ionU32 MeshRendererNormal::GetVertexDataCount() const
{
    return static_cast<ionU32>(m_vertices.size());
}

const void* MeshRendererNormal::GetVertexData() const
{
    return m_vertices.data();
}

ionSize MeshRendererNormal::GetSizeOfVertex() const
{
    return sizeof(VertexNormal);
}

EVertexLayout MeshRendererNormal::GetLayout() const
{
    return EVertexLayout_Pos_Normal;
}

//////////////////////////////////////////////////////////////////////////
MeshRendererSimple::MeshRendererSimple() : BaseMeshRenderer()
{
}

MeshRendererSimple::~MeshRendererSimple()
{
    m_vertices.clear();
}

void MeshRendererSimple::PushBackVertex(const VertexSimple& _vertex)
{
    m_vertices.push_back(_vertex);
}

const ionU32 MeshRendererSimple::GetVertexDataCount() const
{
    return static_cast<ionU32>(m_vertices.size());
}

const void* MeshRendererSimple::GetVertexData() const
{
    return m_vertices.data();
}

ionSize MeshRendererSimple::GetSizeOfVertex() const
{
    return sizeof(VertexSimple);
}

EVertexLayout MeshRendererSimple::GetLayout() const
{
    return EVertexLayout_Pos_UV_Normal;
}


//////////////////////////////////////////////////////////////////////////
MeshRenderer::MeshRenderer() : BaseMeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
    m_vertices.clear();
}

void MeshRenderer::PushBackVertex(const Vertex& _vertex)
{
    m_vertices.push_back(_vertex);
}

const ionU32 MeshRenderer::GetVertexDataCount() const
{
    return static_cast<ionU32>(m_vertices.size());
}

const void* MeshRenderer::GetVertexData() const
{
    return m_vertices.data();
}

ionSize MeshRenderer::GetSizeOfVertex() const
{
    return sizeof(Vertex);
}

EVertexLayout MeshRenderer::GetLayout() const
{
    return EVertexLayout_Full;
}

Vertex& MeshRenderer::GetVertex(ionSize _index)
{
    return m_vertices[_index];
}

const Vertex& MeshRenderer::GetVertex(ionSize _index) const
{
    return m_vertices[_index];
}

ION_NAMESPACE_END


