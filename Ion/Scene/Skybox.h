// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\Skybox.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Renderer/RenderDefs.h"

#include "../Renderer/RenderCommon.h"

#include "../Geometry/Mesh.h"
#include "../Geometry/MeshRenderer.h"

#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using SkyboxAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class RenderCore;
class Material;

class ION_DLL Skybox
{
public:
	static SkyboxAllocator* GetAllocator();

public:
    Skybox();
    virtual ~Skybox();

    static EVertexLayout GetVertexLayout() { return EVertexLayout_Pos; }

    void SetMaterial(Material* _material);
    Material* GetMaterial();

    void UpdateUniformBuffer(const Matrix4x4& _projection, const Matrix4x4& _view, const Matrix4x4& _model);
    void Draw(VkRenderPass _renderPass, RenderCore& _renderCore);
    void CustomDraw(RenderCore& _renderCore, VkCommandBuffer _commandBuffer, VkRenderPass _renderPass);

private:
    Skybox(const Skybox& _Orig) = delete;
    Skybox& operator = (const Skybox&) = delete;

private:
    void GenerateMesh();

private:
    MeshRendererPlain m_meshRenderer;
    Mesh m_mesh;
    DrawSurface m_drawSurface;
};

ION_NAMESPACE_END