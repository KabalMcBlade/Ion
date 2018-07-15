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
};

ION_NAMESPACE_END