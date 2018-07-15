#include "RenderTexture.h"


#include "../Dependencies/Eos/Eos/Eos.h"
//#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

/*

#include "../Texture/TextureManager.h"

#include "../Shader/ShaderProgramManager.h"

#include "../Material/Material.h"

#include "StagingBufferManager.h"
#include "VertexCacheManager.h"

#include "RenderState.h"
*/

#include "../Texture/Texture.h"


#include "RenderDefs.h"
#include "RenderCore.h"

//VK_ALLOCATOR_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderTexture::RenderTexture() : m_textureColor(nullptr)
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
    VkCommandBuffer m_commandBuffer;
    VkFence         m_commandBufferFence;

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
    frameBufferCreateInfo.width = m_textureColor->GetWidth();
    frameBufferCreateInfo.height = m_textureColor->GetHeight();
    frameBufferCreateInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(m_textureColor->GetDevice(), &frameBufferCreateInfo, vkMemory, &m_frameBuffer);
    ionAssertReturnValue(result == VK_SUCCESS, "Impossible to create frame buffer.", false);

    return true;
}

ION_NAMESPACE_END