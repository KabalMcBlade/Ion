#include "Texture.h"

#include "TextureManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Texture::Texture(VkDevice _vkDevice, const eosString& _name) :
    m_vkDevice(_vkDevice),
    m_name(_name)
{
    m_isSwapChainImage = false;
    m_format = VK_FORMAT_UNDEFINED;
    m_image = VK_NULL_HANDLE;
    m_view = VK_NULL_HANDLE;
    m_layout = VK_IMAGE_LAYOUT_GENERAL;
    m_sampler = VK_NULL_HANDLE;
    m_filter = ETextureFilter_Default;
    m_repeat = ETextureRepeat_Repeat;
    m_usage = ETextureUsage_Default;
    m_isCubeMap = false;
    m_isProvedurallyGenerated = false;
}

Texture::~Texture()
{
    if (!m_isSwapChainImage)
    {
        Destroy();
    }
}

void Texture::SetOptions(const TextureOptions& _options)
{
    m_options = _options;
}

ionBool Texture::Create()
{
    // clear before create
    Destroy();

    if (CreateSampler())
    {
        m_format = GetVulkanFormatFromTextureFormat(m_options.m_format);

        VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
        if (m_options.m_format == ETextureFormat_Depth)
        {
            usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        else
        {
            usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        {
            VkImageCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            createInfo.flags = (m_options.m_textureType == ETextureType_Cubic) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
            createInfo.imageType = VK_IMAGE_TYPE_2D;
            createInfo.format = m_format;
            createInfo.extent.width = m_options.m_width;
            createInfo.extent.height = m_options.m_height;
            createInfo.extent.depth = 1;
            createInfo.mipLevels = m_options.m_numLevels;
            createInfo.arrayLayers = (m_options.m_textureType == ETextureType_Cubic) ? 6 : 1;
            createInfo.samples = static_cast<VkSampleCountFlagBits>(m_options.m_samples);
            createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            createInfo.usage = usageFlags;
            createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VkResult result = vkCreateImage(m_vkDevice, &createInfo, vkMemory, &m_image);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image!", false);
        }

        VkMemoryRequirements memoryRequirements = {};
        vkGetImageMemoryRequirements(m_vkDevice, m_image, &memoryRequirements);

        {
            vkGpuMemoryCreateInfo createInfo = {};
            createInfo.m_size = memoryRequirements.size;
            createInfo.m_align = memoryRequirements.alignment;
            createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
            createInfo.m_usage = EMemoryUsage_GPU;
            createInfo.m_type = EGpuMemoryType_ImageOptimal;

            m_allocation = ionGPUMemoryManager().Alloc(createInfo);
            ionAssertReturnValue(m_allocation.m_result == VK_SUCCESS, "Cannot Allocate memory!", false);

            VkResult result = vkBindImageMemory(m_vkDevice, m_image, m_allocation.m_memory, m_allocation.m_offset);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot bind the image memory!", false);
        }

        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_image;
            createInfo.viewType = (m_options.m_textureType == ETextureType_Cubic) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_format;
            createInfo.components = GetVulkanComponentMappingFromTextureFormat(m_options.m_format, m_options.m_colorFormat);
            createInfo.subresourceRange.aspectMask = (m_options.m_format == ETextureFormat_Depth) ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = m_options.m_numLevels;
            createInfo.subresourceRange.layerCount = (m_options.m_textureType == ETextureType_Cubic) ? 6 : 1;
            createInfo.subresourceRange.baseMipLevel = 0;

            VkResult result = vkCreateImageView(m_vkDevice, &createInfo, vkMemory, &m_view);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image view!", false);
        }

        return true;
    }
    else
    {
        return false;
    }
}

VkFormat Texture::GetVulkanFormatFromTextureFormat(ETextureFormat _format)
{
    switch (_format)
    {
    case ETextureFormat_RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
    case ETextureFormat_XRGB8: return VK_FORMAT_R8G8B8_UNORM;
    case ETextureFormat_Alpha: return VK_FORMAT_R8_UNORM;
    case ETextureFormat_Luminance8Alpha8: return VK_FORMAT_R8G8_UNORM;
    case ETextureFormat_Luminance8: return VK_FORMAT_R8_UNORM;
    case ETextureFormat_Intensity8: return VK_FORMAT_R8_UNORM;
    case ETextureFormat_DXT1: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    case ETextureFormat_DXT5: return VK_FORMAT_BC3_UNORM_BLOCK;
    case ETextureFormat_Depth: return m_options.m_vkDepthFormat;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

VkComponentMapping Texture::GetVulkanComponentMappingFromTextureFormat(ETextureFormat _format, ETextureColor _color)
{
    VkComponentMapping componentMapping = 
    {
        VK_COMPONENT_SWIZZLE_ZERO,
        VK_COMPONENT_SWIZZLE_ZERO,
        VK_COMPONENT_SWIZZLE_ZERO,
        VK_COMPONENT_SWIZZLE_ZERO
    };

    if (_color == ETextureColor_Green_To_Aalpha)
    {
        componentMapping.r = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.g = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.b = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.a = VK_COMPONENT_SWIZZLE_G;
        return componentMapping;
    }

    switch (_format)
    {
    case ETextureFormat_Luminance8:
        componentMapping.r = VK_COMPONENT_SWIZZLE_R;
        componentMapping.g = VK_COMPONENT_SWIZZLE_R;
        componentMapping.b = VK_COMPONENT_SWIZZLE_R;
        componentMapping.a = VK_COMPONENT_SWIZZLE_ONE;
        break;
    case ETextureFormat_Luminance8Alpha8:
        componentMapping.r = VK_COMPONENT_SWIZZLE_R;
        componentMapping.g = VK_COMPONENT_SWIZZLE_R;
        componentMapping.b = VK_COMPONENT_SWIZZLE_R;
        componentMapping.a = VK_COMPONENT_SWIZZLE_G;
        break;
    case ETextureFormat_Alpha:
        componentMapping.r = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.g = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.b = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.a = VK_COMPONENT_SWIZZLE_R;
        break;
    case ETextureFormat_Intensity8:
        componentMapping.r = VK_COMPONENT_SWIZZLE_R;
        componentMapping.g = VK_COMPONENT_SWIZZLE_R;
        componentMapping.b = VK_COMPONENT_SWIZZLE_R;
        componentMapping.a = VK_COMPONENT_SWIZZLE_R;
        break;
    default:
        componentMapping.r = VK_COMPONENT_SWIZZLE_R;
        componentMapping.g = VK_COMPONENT_SWIZZLE_G;
        componentMapping.b = VK_COMPONENT_SWIZZLE_B;
        componentMapping.a = VK_COMPONENT_SWIZZLE_A;
        break;
    }

    return componentMapping;
}

ionBool Texture::CreateSampler()
{
    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.maxAnisotropy = 1.0f;
    createInfo.anisotropyEnable = VK_FALSE;
    createInfo.compareEnable = (m_options.m_format == ETextureFormat_Depth);
    createInfo.compareOp = (m_options.m_format == ETextureFormat_Depth) ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_NEVER;

    switch (m_filter) 
    {
    case ETextureFilter_Default:
    case ETextureFilter_Linear:
        createInfo.minFilter = VK_FILTER_LINEAR;
        createInfo.magFilter = VK_FILTER_LINEAR;
        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;
    case ETextureFilter_Nearest:
        createInfo.minFilter = VK_FILTER_NEAREST;
        createInfo.magFilter = VK_FILTER_NEAREST;
        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    default:
        ionAssertReturnValue(false, "Texture filter not supported", false);
    }

    switch (m_repeat) 
    {
    case ETextureRepeat_Repeat:
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
    case ETextureRepeat_Clamp:
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        break;
    case ETextureRepeat_ClampAlpha:
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        break;
    case ETextureRepeat_ClampZero:
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        break;
    default:
        ionAssertReturnValue(false, "Texture repeat mode not supported", false);
    }

    VkResult result = vkCreateSampler(m_vkDevice, &createInfo, vkMemory, &m_sampler);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create sampler", false);

    return true;
}

void Texture::Destroy()
{
    if (m_sampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(m_vkDevice, m_sampler, vkMemory);
        m_sampler = VK_NULL_HANDLE;
    }

    if (m_image != VK_NULL_HANDLE) 
    {
        vkDestroyImage(m_vkDevice, m_image, vkMemory);
        ionGPUMemoryManager().Free(m_allocation);
        m_allocation = vkGpuMemoryAllocation();

        vkDestroyImageView(m_vkDevice, m_view, vkMemory);

        m_view = VK_NULL_HANDLE;
        m_image = VK_NULL_HANDLE;
    }
}

ION_NAMESPACE_END