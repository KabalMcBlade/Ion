#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Renderer/RenderDefs.h"

#include "../Renderer/RenderCommon.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderCore;
class Material;
class MeshPlain;

class ION_DLL Skybox
{
public:
    explicit Skybox();
    virtual ~Skybox();

    static EVertexLayout GetVertexLayout() { return EVertexLayout_Pos; }

    void SetMaterial(Material* _material);
    Material* GetMaterial();

    void UpdateUniformBuffer(const Matrix& _projection, const Matrix& _view, const Matrix& _model);
    void Draw(VkRenderPass _renderPass, RenderCore& _renderCore);
    void CustomDraw(RenderCore& _renderCore, VkCommandBuffer _commandBuffer, VkRenderPass _renderPass);

private:
    Skybox(const Skybox& _Orig) = delete;
    Skybox& operator = (const Skybox&) = delete;

private:
    void GenerateMesh();

private:
    MeshPlain*  m_mesh;
    DrawSurface m_drawSurface;
};

ION_NAMESPACE_END