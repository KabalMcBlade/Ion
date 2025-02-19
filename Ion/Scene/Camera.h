// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\Camera.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Entity.h"
#include "../Geometry/Frustum.h"

#include "../Renderer/RenderCommon.h"

#include "../Core/MemorySettings.h"

#include "../Renderer/RenderCore.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using CameraAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class RenderCore;
class Skybox;
class ION_DLL Camera : public Node
{
public:
	static CameraAllocator* GetAllocator();

public:
    enum ECameraType 
    { 
        ECameraType_LookAt = 0,
        ECameraType_FirstPerson
    };
    
    explicit Camera(ionBool _clearBackground = true);
    Camera(const ionString & _name, ionBool _clearBackground = true);
    virtual ~Camera();

    void SetCameraType(ECameraType _type);
    void SetPerspectiveProjection(ionFloat _fovDeg, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar);
    void UpdateAspectRatio(ionFloat _aspect);

    // for the nature of the Skybox I need to call this function inside of the mapping!
    void UpdateView();

    ionFloat GetNear() const { return m_zNear; }
    ionFloat GetFar() const { return m_zFar; }
    ionFloat GetFovDeg() const { return NIX_RAD_TO_DEG(m_fov); }
    ionFloat GetFovRad() const { return m_fov; }

    const Matrix4x4& GetPerspectiveProjection() const { return m_projection; }
    const Matrix4x4& GetView() const { return m_view; }

    const Frustum& GetFrustum() const { return m_frustum; }

    const VkRenderPass& GetRenderPass() const { return m_vkRenderPass; }

    Skybox* AddSkybox();
    Skybox* GetSkybox();
    void RemoveSkybox();

    void RenderSkybox(RenderCore& _renderCore);
    void CustomRenderSkybox(RenderCore& _renderCore, VkCommandBuffer _commandBuffer, VkRenderPass _renderPass);

    //////////////////////////////////////////////////////////////////////////
    // Setting

    // The parameters are normalized depending from the screen
    void SetViewportParameters(ionFloat _x = 0.0f, ionFloat _y = 0.0f, ionFloat _width = 1.0f, ionFloat _height = 1.0f, ionFloat _minDepth = 0.0f, ionFloat _maxDepth = 1.0f);
    void SetScissorParameters(ionFloat _x = 0.0f, ionFloat _y = 0.0f, ionFloat _width = 1.0f, ionFloat _height = 1.0f);
    void SetRenderPassParameters(ionFloat _clearDepthValue = 1.0f, ionU8 _clearStencilValue = 0, ionFloat _clearRed = 1.0f, ionFloat _clearGreen = 1.0f, ionFloat _clearBlue = 1.0f);

    void ConputeRenderAreaViewportScissor(ionS32 _x, ionS32 _y, ionS32 _width, ionS32 _height);

    //////////////////////////////////////////////////////////////////////////
    // Render
    void SetViewport(RenderCore& _renderCore);
    void SetScissor(RenderCore& _renderCore);
    void StartRenderPass(RenderCore& _renderCore);
    void EndRenderPass(RenderCore& _renderCore);


    void SetViewport(RenderCore& _renderCore, VkCommandBuffer _commandBuffer);
    void SetScissor(RenderCore& _renderCore, VkCommandBuffer _commandBuffer);
    void StartRenderPass(RenderCore& _renderCore, VkRenderPass _renderPass, VkFramebuffer _frameBuffer, VkCommandBuffer _commandBuffer, const ionVector<VkClearValue, RenderCoreAllocator, RenderCore::GetAllocator>& _clearValues);
    void EndRenderPass(RenderCore& _renderCore, VkCommandBuffer _commandBuffer);

public:
    static Matrix4x4 PerspectiveProjectionMatrix(ionFloat _fov, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar);
    static Matrix4x4 OrthographicProjectionMatrix(ionFloat _left, ionFloat _right, ionFloat _bottom, ionFloat _top, ionFloat _zNear, ionFloat _zFar);

private:
    friend class RenderManager;
    friend class SceneGraph;

    void CreateRenderPassAndFrameBuffers(RenderCore& _renderCore);
    void DestroyRenderPassAndFrameBuffers(RenderCore& _renderCore);
    void RecreateRenderPassAndFrameBuffers(RenderCore& _renderCore);

    Camera(const Camera& _Orig) = delete;
    Camera& operator = (const Camera&) = delete;

private:
    Matrix4x4  m_projection;
    Matrix4x4  m_view;

    // render pass and framebuffer depending on camera type
    VkRenderPass                m_vkRenderPass;
    ionVector<VkFramebuffer, RenderCoreAllocator, RenderCore::GetAllocator>    m_vkFrameBuffers;

    Frustum m_frustum;

    Skybox* m_skybox;

    ECameraType m_type;

    VkRect2D m_renderArea;
    VkRect2D m_scissor;
    VkViewport m_viewport;

    ionFloat m_fov;
    ionFloat m_zNear;
    ionFloat m_zFar;

    ionFloat m_minDepth; 
    ionFloat m_maxDepth;

    ionFloat m_viewPortX;
    ionFloat m_viewPortY;
    ionFloat m_scissorX;
    ionFloat m_scissorY;

    ionFloat m_viewPortWidth;
    ionFloat m_viewPortHeight;
    ionFloat m_scissorWidth;
    ionFloat m_scissorHeight;

    ionFloat m_clearDepthValue;
    ionFloat m_clearRed;
    ionFloat m_clearGreen;
    ionFloat m_clearBlue;

    EFramebufferLoad m_framebufferLoadType;

    ionU8    m_clearStencilValue;
};

ION_NAMESPACE_END