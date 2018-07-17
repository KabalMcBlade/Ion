#include "RenderTexture.h"


#include "../Dependencies/Eos/Eos/Eos.h"


#include "../Material/Material.h"

#include "../Shader/ShaderProgramManager.h"

#include "../Texture/Texture.h"

#include "VertexCacheManager.h"
#include "StagingBufferManager.h"
#include "IndexBufferObject.h"
#include "VertexBufferObject.h"

#include "RenderDefs.h"
#include "RenderCore.h"

#include "RenderState.h"

#include "RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderTexture::RenderTexture() : m_textureColor(nullptr), m_stateBits(0), m_width(0), m_height(0)
{
}

RenderTexture::~RenderTexture()
{
}

ionBool RenderTexture::Init(const RenderCore& _renderCore, const Texture* _textureColor, const Texture* _textureDepth /*= nullptr*/)
{
    // the only mandatory
    ionAssertReturnValue(_textureColor != nullptr, "Texture is NULL!", false);

    if (_textureDepth != nullptr)
    {
        ionAssertReturnValue(_textureColor->GetWidth() == _textureDepth->GetWidth() && _textureColor->GetHeight() == _textureDepth->GetHeight(), "Texture is NULL!", false);
    }

    m_textureColor = _textureColor;
    m_textureDepth = _textureDepth;

    m_width = _textureColor->GetWidth();
    m_height = _textureColor->GetHeight();

    if (!CreateCommandBuffer(_renderCore))
    {
        return false;
    }

    if(!CreateSemaphore())
    {
        return false;
    }

    if (!CreateRenderPass())
    {
        return false;
    }

    if (!CreateFrameBuffer())
    {
        return false;
    }

    return true;
}

void RenderTexture::Shutdown(const RenderCore& _renderCore)
{
    vkDestroyFramebuffer(m_textureColor->GetDevice(), m_frameBuffer, vkMemory);

    vkDestroyRenderPass(m_textureColor->GetDevice(), m_renderPass, vkMemory);

    vkFreeCommandBuffers(_renderCore.GetDevice(), _renderCore.GetCommandPool(), 1, &m_commandBuffer);

    m_textureColor = nullptr;
}

void RenderTexture::Recreate(const RenderCore& _renderCore)
{
    vkDeviceWaitIdle(_renderCore.GetDevice());

    vkDestroyFramebuffer(_renderCore.GetDevice(), m_frameBuffer, vkMemory);

    vkDestroyRenderPass(_renderCore.GetDevice(), m_renderPass, vkMemory);

    vkFreeCommandBuffers(_renderCore.GetDevice(), _renderCore.GetCommandPool(), 1, &m_commandBuffer);

    CreateCommandBuffer(_renderCore);
    CreateRenderPass();
    CreateFrameBuffer();
}

ionBool RenderTexture::CreateCommandBuffer(const RenderCore& _renderCore)
{
    {
        VkCommandBufferAllocateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        createInfo.commandPool = _renderCore.GetCommandPool();
        createInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(m_textureColor->GetDevice(), &createInfo, &m_commandBuffer);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create command buffer!", false);
    }

    {
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkResult result = vkCreateFence(m_textureColor->GetDevice(), &createInfo, vkMemory, &m_commandBufferFence);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create fence!", false);
    }

    return true;
}

ionBool RenderTexture::CreateSemaphore()
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(m_textureColor->GetDevice(), &createInfo, vkMemory, &m_acquiringSemaphore);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for locking!", false);

    result = vkCreateSemaphore(m_textureColor->GetDevice(), &createInfo, vkMemory, &m_completedSemaphore);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for unlocking!", false);

    return true;
}

ionBool RenderTexture::CreateRenderPass()
{
    eosVector(VkAttachmentDescription) attachments;

    if (m_textureDepth != nullptr)
    {
        attachments.resize(2);
    }
    else
    {
        attachments.resize(1);
    }

    VkAttachmentDescription& colorResolver = attachments[0];
    colorResolver.format = m_textureColor->GetFormat();
    colorResolver.samples = VK_SAMPLE_COUNT_1_BIT;
    colorResolver.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorResolver.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorResolver.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorResolver.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorResolver.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorResolver.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if (m_textureDepth != nullptr)
    {
        VkAttachmentDescription& depthResolver = attachments[1];
        depthResolver.format = m_textureDepth->GetFormat();
        depthResolver.samples = VK_SAMPLE_COUNT_1_BIT;
        depthResolver.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthResolver.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthResolver.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthResolver.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthResolver.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthResolver.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
   
    VkAttachmentReference colorRef = {};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef = {};
    if (m_textureDepth != nullptr)
    {
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;
    subpass.pResolveAttachments = nullptr;

    eosVector(VkSubpassDependency) dependencies;
    dependencies.resize(2);

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<ionU32>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 2;
    renderPassCreateInfo.pDependencies = dependencies.data();

    VkResult result = vkCreateRenderPass(m_textureColor->GetDevice(), &renderPassCreateInfo, vkMemory, &m_renderPass);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create render pass!", false);

    return true;
}

ionBool RenderTexture::CreateFrameBuffer()
{
    eosVector(VkImageView) attachments;
    if (m_textureDepth != nullptr)
    {
        attachments.resize(2);
        attachments[0] = m_textureColor->GetView();
        attachments[1] = m_textureDepth->GetView();
    }
    else
    {
        attachments.resize(1);
        attachments[0] = m_textureColor->GetView();
    }

    VkFramebufferCreateInfo frameBufferCreateInfo{};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = m_renderPass;
    frameBufferCreateInfo.attachmentCount = static_cast<ionU32>(attachments.size());
    frameBufferCreateInfo.pAttachments = attachments.data();
    frameBufferCreateInfo.width = m_width;
    frameBufferCreateInfo.height = m_height;
    frameBufferCreateInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(m_textureColor->GetDevice(), &frameBufferCreateInfo, vkMemory, &m_frameBuffer);
    ionAssertReturnValue(result == VK_SUCCESS, "Impossible to create frame buffer.", false);

    return true;
}

ionBool RenderTexture::StartFrame()
{
    ionStagingBufferManager().Submit();
    ionShaderProgramManager().StartFrame();

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult result = vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo);
    ionAssertReturnValue(result == VK_SUCCESS, "vkBeginCommandBuffer failed!", false);

    return true;
}

void RenderTexture::StartRenderPass(ionFloat _clearDepthValue, ionU8 _clearStencilValue, ionFloat _clearRed, ionFloat _clearGreen, ionFloat _clearBlue)
{
    ionAssertReturnVoid(_clearDepthValue >= 0.0f && _clearDepthValue <= 1.0f, "Clear depth must be between 0 and 1!");
    ionAssertReturnVoid(_clearRed >= 0.0f && _clearRed <= 1.0f, "Clear red must be between 0 and 1!");
    ionAssertReturnVoid(_clearGreen >= 0.0f && _clearGreen <= 1.0f, "Clear green must be between 0 and 1!");
    ionAssertReturnVoid(_clearBlue >= 0.0f && _clearBlue <= 1.0f, "Clear blue must be between 0 and 1!");

    eosVector(VkClearValue) clearValues;
    if (m_textureDepth != nullptr)
    {
        clearValues.resize(2);
        clearValues[0].color = { { _clearRed, _clearGreen, _clearBlue, 1.0f } };
        clearValues[1].depthStencil = { _clearDepthValue, _clearStencilValue };
    }
    else
    {
        clearValues.resize(1);
        clearValues[0].color = { { _clearRed, _clearGreen, _clearBlue, 1.0f } };
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_renderPass;
    renderPassBeginInfo.framebuffer = m_frameBuffer;
    renderPassBeginInfo.renderArea.extent.width = m_width;
    renderPassBeginInfo.renderArea.extent.height = m_height;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.clearValueCount = static_cast<ionU32>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderTexture::EndRenderPass()
{
    vkCmdEndRenderPass(m_commandBuffer);
}

void RenderTexture::EndFrame(const RenderCore& _renderCore)
{
    // Transition our swap image to present.
    // Do this instead of having the renderpass do the transition
    // so we can take advantage of the general layout to avoid 
    // additional image barriers.
    /*
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_vkSwapchainImages[m_currentSwapIndex];
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    */

    VkResult result = vkEndCommandBuffer(m_commandBuffer);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkEndCommandBuffer failed!");

    ionShaderProgramManager().EndFrame();

    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_acquiringSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_completedSemaphore;
    submitInfo.pWaitDstStageMask = &dstStageMask;

    result = vkQueueSubmit(_renderCore.GetGraphicQueue(), 1, &submitInfo, m_commandBufferFence);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkQueueSubmit failed!");
}

void RenderTexture::SetDefaultState()
{
    m_stateBits = 0;

    SetState(0);

    SetScissor(0, 0, m_width, m_height);
}

void RenderTexture::SetState(ionU64 _stateBits)
{
    m_stateBits = _stateBits | (m_stateBits & ERasterization_DepthTest_Mask);

    // if the current rendered draw view is a mirror should set the 
    // m_stateBits |= ERasterization_View_Specular;
}

void RenderTexture::SetScissor(ionS32 _leftX, ionS32 _bottomY, ionU32 _width, ionU32 _height)
{
    VkRect2D scissor;
    scissor.offset.x = _leftX;
    scissor.offset.y = _bottomY;
    scissor.extent.width = _width;
    scissor.extent.height = _height;
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void RenderTexture::SetViewport(ionFloat _leftX, ionFloat _bottomY, ionFloat _width, ionFloat _height, ionFloat _minDepth, ionFloat _maxDepth)
{
    VkViewport viewport;
    viewport.x = _leftX;
    viewport.y = _bottomY;
    viewport.width = _width;
    viewport.height = _height;
    viewport.minDepth = _minDepth;
    viewport.maxDepth = _maxDepth;
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

void RenderTexture::SetPolygonOffset(ionFloat _scale, ionFloat _bias)
{
    vkCmdSetDepthBias(m_commandBuffer, _bias, 0.0f, _scale);
}

void RenderTexture::SetDepthBoundsTest(ionFloat _zMin, ionFloat _zMax)
{
    if (_zMin > _zMax)
    {
        return;
    }

    if (_zMin == 0.0f && _zMax == 0.0f)
    {
        m_stateBits = m_stateBits & ~ERasterization_DepthTest_Mask;
    }
    else
    {
        m_stateBits |= ERasterization_DepthTest_Mask;
        vkCmdSetDepthBounds(m_commandBuffer, _zMin, _zMax);
    }
}

void RenderTexture::CopyFrameBuffer(Texture* _texture, ionS32 _width, ionS32 _height)
{
    vkCmdEndRenderPass(m_commandBuffer);

    VkImageMemoryBarrier dstBarrier = {};
    dstBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    dstBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.image = _texture->GetImage();
    dstBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    dstBarrier.subresourceRange.baseMipLevel = 0;
    dstBarrier.subresourceRange.levelCount = 1;
    dstBarrier.subresourceRange.baseArrayLayer = 0;
    dstBarrier.subresourceRange.layerCount = 1;

    // Pre copy transitions
    {
        // Transition the color dest texture so we can transfer to it.
        dstBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dstBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dstBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);
    }

    // Perform the blit/copy
    {
        VkImageBlit region = {};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.baseArrayLayer = 0;
        region.srcSubresource.mipLevel = 0;
        region.srcSubresource.layerCount = 1;
        region.srcOffsets[1] = { _width, _height, 1 };

        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.baseArrayLayer = 0;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.layerCount = 1;
        region.dstOffsets[1] = { _texture->GetWidth(), _texture->GetHeight(), 1 };

        vkCmdBlitImage(m_commandBuffer, m_textureColor->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_NEAREST);
    }

    // Post copy transitions
    {
        // Transition the color dest texture so we can transfer to it.
        dstBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dstBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);
    }

    eosVector(VkClearValue) clearValues;
    if (m_textureDepth != nullptr)
    {
        clearValues.resize(2);
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };
    }
    else
    {
        clearValues.resize(1);
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_renderPass;
    renderPassBeginInfo.framebuffer = m_frameBuffer;
    renderPassBeginInfo.renderArea.extent.width = m_width;
    renderPassBeginInfo.renderArea.extent.height = m_height;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.clearValueCount = static_cast<ionU32>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderTexture::Draw(const RenderCore& _renderCore, const DrawSurface& _surface)
{
    ionShaderProgramManager().SetRenderParamMatrix(ION_MODEL_MATRIX_PARAM_HASH, &_surface.m_modelMatrix[0]);
    ionShaderProgramManager().SetRenderParamMatrix(ION_VIEW_MATRIX_PARAM_HASH, &_surface.m_viewMatrix[0]);
    ionShaderProgramManager().SetRenderParamMatrix(ION_PROJ_MATRIX_PARAM_HASH, &_surface.m_projectionMatrix[0]);

    ionS32  vertexShaderIndex = -1;
    ionS32  fragmentShaderIndex = -1;
    ionS32  tessellationControlIndex = -1;
    ionS32  tessellationEvaluationIndex = -1;
    ionS32  geometryIndex = -1;
    ionBool useJoint = false;
    ionBool useSkinning = false;

    _surface.m_material->GetShaders(vertexShaderIndex, fragmentShaderIndex, tessellationControlIndex, tessellationEvaluationIndex, geometryIndex, useJoint, useSkinning);

    const ionS32 shaderProgramIndex =
        ionShaderProgramManager().FindProgram(_surface.m_material->GetShaderProgramName(), _surface.m_material->GetVertexLayout(), _surface.m_material->GetConstantsShaders(),
            vertexShaderIndex, fragmentShaderIndex, tessellationControlIndex, tessellationEvaluationIndex, geometryIndex, useJoint, useSkinning);

    ionShaderProgramManager().BindProgram(shaderProgramIndex);
    ionShaderProgramManager().CommitCurrent(_renderCore, m_stateBits, m_commandBuffer);

    ionSize indexOffset = 0;
    ionSize vertexOffset = 0;
    IndexBuffer indexBuffer;
    if (ionVertexCacheManager().GetIndexBuffer(_surface.m_indexCache, &indexBuffer))
    {
        const VkBuffer buffer = indexBuffer.GetObject();
        const VkDeviceSize offset = indexBuffer.GetOffset();
        indexOffset = offset;
        vkCmdBindIndexBuffer(m_commandBuffer, buffer, offset, VK_INDEX_TYPE_UINT32);
    }

    VertexBuffer vertexBufer;
    if (ionVertexCacheManager().GetVertexBuffer(_surface.m_vertexCache, &vertexBufer))
    {
        const VkBuffer buffer = vertexBufer.GetObject();
        const VkDeviceSize offset = vertexBufer.GetOffset();
        vertexOffset = offset;
        vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &buffer, &offset);
    }

    vkCmdDrawIndexed(m_commandBuffer, _surface.m_indexCount, 1, _surface.m_indexStart /*(indexOffset >> 1)*/, 0 /*vertexOffset / sizeof(Vertex)*/, 0);
}

ION_NAMESPACE_END