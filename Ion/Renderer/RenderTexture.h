#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"


ION_NAMESPACE_BEGIN

class Texture;
class RenderCore;
class ION_DLL RenderTexture final
{
public:
    RenderTexture();
    ~RenderTexture();

    ionBool Init(const RenderCore& _renderCore, const Texture* _textureColor, const Texture* _textureDepth = nullptr);
    void    Shutdown(const RenderCore& _renderCore);
    void    Recreate(const RenderCore& _renderCore);

    ionBool StartFrame();
    void    EndFrame(const RenderCore& _renderCore);
    void    StartRenderPass(ionFloat _clearDepthValue, ionU8 _clearStencilValue, ionFloat _clearRed, ionFloat _clearGreen, ionFloat _clearBlue);
    void    EndRenderPass();
    void    SetDefaultState();
    void    SetState(ionU64 _stateBits);
    void    SetScissor(ionS32 _leftX, ionS32 _bottomY, ionU32 _width, ionU32 _height);
    void    SetViewport(ionFloat _leftX, ionFloat _bottomY, ionFloat _width, ionFloat _height, ionFloat _minDepth, ionFloat _maxDepth);
    void    SetPolygonOffset(ionFloat _scale, ionFloat _bias);
    void    SetDepthBoundsTest(ionFloat _zMin, ionFloat _zMax);
    /*
    void    CopyFrameBuffer(Texture* _texture, ionS32 _width, ionS32 _height);
    void    Draw(const DrawSurface& _surface);
    */
private:
    RenderTexture(const RenderTexture& _Orig) = delete;
    RenderTexture& operator = (const RenderTexture&) = delete;

private:
    ionBool CreateCommandBuffer(const RenderCore& _renderCore);
    ionBool CreateSemaphore();
    ionBool CreateRenderPass();
    ionBool CreateFrameBuffer();

private:
    const Texture*  m_textureColor;
    const Texture*  m_textureDepth;

    VkSemaphore     m_acquiringSemaphore;
    VkSemaphore     m_completedSemaphore;

    VkRenderPass    m_renderPass;
    VkFramebuffer   m_frameBuffer;

    VkCommandBuffer m_commandBuffer;
    VkFence         m_commandBufferFence;

    ionU64          m_stateBits;

    ionU32          m_width;
    ionU32          m_height;

};

ION_NAMESPACE_END