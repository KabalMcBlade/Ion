// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\Skybox.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "Skybox.h"


#include "../Renderer/VertexCacheManager.h"

#include "../Renderer/RenderCore.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

SkyboxAllocator* Skybox::GetAllocator()
{
	static HeapArea<Settings::kSkyBoxAllocatorSize> memoryArea;
	static SkyboxAllocator memoryAllocator(memoryArea, "SkyboxListAllocator");

	return &memoryAllocator;
}


Skybox::Skybox()
{
    GenerateMesh();
}

Skybox::~Skybox()
{
}

void Skybox::GenerateMesh()
{
    ionVector<Index, SkyboxAllocator, GetAllocator> indices;
    indices.resize(36);
    indices = {
        0,1,2,0,2,3,
        4,5,6,4,6,7,
        8,9,10,8,10,11,
        12,13,14,12,14,15,
        16,17,18,16,18,19,
        20,21,22,20,22,23 };

    Vector4 positions[24] = {
        Vector4(-0.5f, 0.5f, 0.5f, 1.0f),
        Vector4(-0.5f, -0.5f, 0.5f, 1.0f),
        Vector4(0.5f, -0.5f, 0.5f, 1.0f),
        Vector4(0.5f, 0.5f, 0.5f, 1.0f),

        Vector4(0.5f, 0.5f, -0.5f, 1.0f),
        Vector4(0.5f, -0.5f, -0.5f, 1.0f),
        Vector4(-0.5f, -0.5f, -0.5f, 1.0f),
        Vector4(-0.5f, 0.5f, -0.5f, 1.0f),

        Vector4(0.5f, 0.5f, 0.5f, 1.0f),
        Vector4(0.5f, -0.5f, 0.5f, 1.0f),
        Vector4(0.5f, -0.5f, -0.5f, 1.0f),
        Vector4(0.5f, 0.5f, -0.5f, 1.0f),

        Vector4(-0.5f, 0.5f, -0.5f, 1.0f),
        Vector4(-0.5f, 0.5f, 0.5f, 1.0f),
        Vector4(0.5f, 0.5f, 0.5f, 1.0f),
        Vector4(0.5f, 0.5f, -0.5f, 1.0f),

        Vector4(-0.5f, 0.5f, -0.5f, 1.0f),
        Vector4(-0.5f, -0.5f, -0.5f, 1.0f),
        Vector4(-0.5f, -0.5f, 0.5f, 1.0f),
        Vector4(-0.5f, 0.5f, 0.5f, 1.0f),

        Vector4(-0.5f, -0.5f, 0.5f, 1.0f),
        Vector4(-0.5f, -0.5f, -0.5f, 1.0f),
        Vector4(0.5f, -0.5f, -0.5f, 1.0f),
        Vector4(0.5f, -0.5f, 0.5f, 1.0f)
    };

    ionVector<VertexPlain, SkyboxAllocator, GetAllocator> vertices;
    vertices.resize(24);

    for (ionU32 i = 0; i < 24; ++i)
    {
        vertices[i].SetPosition(positions[i]);
    }
    for (ionU32 i = 0; i < 24; ++i)
    {
        m_meshRenderer.PushBackVertex(vertices[i]);
    }
    for (ionU32 i = 0; i < 36; ++i)
    {
        m_meshRenderer.PushBackIndex(indices[i]);
    }

    m_mesh.SetIndexCount(36);
    m_mesh.SetIndexStart(0);
}

void Skybox::SetMaterial(Material* _material)
{
    m_mesh.SetMaterial(_material);

    m_drawSurface.m_indexStart = m_mesh.GetIndexStart();
    m_drawSurface.m_indexCount = m_mesh.GetIndexCount();
    m_drawSurface.m_material = m_mesh.GetMaterial();
    m_drawSurface.m_visible = true;
    m_drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(m_meshRenderer.GetVertexData(), m_meshRenderer.GetVertexDataCount(), m_meshRenderer.GetSizeOfVertex());
    m_drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(m_meshRenderer.GetIndexData(), m_meshRenderer.GetIndexDataCount(), m_meshRenderer.GetSizeOfIndex());
}

Material* Skybox::GetMaterial()
{
    return m_mesh.GetMaterial();
}

void Skybox::UpdateUniformBuffer(const Matrix4x4& _projection, const Matrix4x4& _view, const Matrix4x4& _model)
{
    m_drawSurface.m_modelMatrix = _model;
    m_drawSurface.m_viewMatrix = _view;
    m_drawSurface.m_projectionMatrix = _projection;
}

void Skybox::Draw(VkRenderPass _renderPass, RenderCore& _renderCore)
{
    _renderCore.SetState(m_mesh.GetMaterial()->GetState().GetStateBits());
    _renderCore.Draw(_renderPass, m_drawSurface);
}

void Skybox::CustomDraw(RenderCore& _renderCore, VkCommandBuffer _commandBuffer, VkRenderPass _renderPass)
{
    _renderCore.SetState(m_mesh.GetMaterial()->GetState().GetStateBits());
    _renderCore.Draw(_commandBuffer, _renderPass, m_drawSurface);
}

ION_NAMESPACE_END