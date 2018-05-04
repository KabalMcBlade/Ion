#include "Texture.h"

#include "TextureManager.h"

#include "../Renderer/StagingBufferManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Texture::Texture(VkDevice _vkDevice, const eosString& _name, ionS32 _index /*= -1*/) :
    m_vkDevice(_vkDevice),
    m_name(_name),
    m_index(_index)
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

ionBool Texture::LoadTexture(ionU32 _width, ionU32 _height, ionU32 _numChannels, ionU8* _buffer)
{
    m_options.m_width = _width;
    m_options.m_height = _height;
    m_options.m_numChannels = _numChannels;
    m_options.m_numLevels = 0;

    GenerateOptions();

    if (Create())
    {
        UploadTextureToMemory(m_options.m_numLevels, m_options.m_width, m_options.m_height, _buffer, 0);

        return true;
    }

    return false;
}

ionBool Texture::LoadTexture2D(const eosString& _path)
{
    ionS32 w = 0, h = 0, c = 0;
    ionU8* buffer = stbi_load(_path.c_str(), &w, &h, &c, 0);

    m_options.m_width = w;
    m_options.m_height = h;
    m_options.m_numChannels = c;
    m_options.m_numLevels = 0;

    GenerateOptions();

    ionBool result = Create();
    if (result)
    {
        UploadTextureToMemory(m_options.m_numLevels, m_options.m_width, m_options.m_height, buffer, 0);
    }

    stbi_image_free(buffer);

    return result;
}

// Adding the side name to the file with "_": for instance: "test.png" become "test_left.png", "test_top.png" etc etc
ionBool Texture::LoadTexture3D(const eosString& _path)
{
    eosString ext;
    eosString path;
    ionSize i = _path.rfind('.', _path.length());
    if (i != std::string::npos) 
    {
        path = _path.substr(0, i);
        ext = _path.substr(i + 1, _path.length() - i);
    }

    if (ext.empty())
    {
        return false;
    }

    eosString suffix[6]{ "_left", "_top", "_front", "_bottom", "_right", "_back" };

    ionBool result = true;

    ionS32 w = 0, h = 0, c = 0;
    for(ionU8 i = 0; i < 6; ++i)
    {
        eosString newPath = path + suffix[i] + ext;
        ionU8* buffer = stbi_load(newPath.c_str(), &w, &h, &c, 0);

        m_options.m_width = w;
        m_options.m_height = h;
        m_options.m_numChannels = c;
        m_options.m_numLevels = 0;

        GenerateOptions();

        // if one fail, all fails
        result &= Create();
        if (result)
        {
            UploadTextureToMemory(m_options.m_numLevels, m_options.m_width, m_options.m_height, buffer, i);
        }

        stbi_image_free(buffer);
    }

    return result;
}

void Texture::SetOptions(const TextureOptions& _options)
{
    m_options = _options;
}

void Texture::GenerateOptions()
{
    if (m_options.m_format == ETextureFormat_None)
    {
        m_options.m_colorFormat = ETextureColor_Default;
        switch (m_usage) 
        {
        case ETextureUsage_Coverage:
            m_options.m_format = ETextureFormat_DXT1;
            m_options.m_colorFormat = ETextureColor_Green_To_Aalpha;
            break;
        case ETextureUsage_Depth:
            m_options.m_format = ETextureFormat_Depth;
            break;
        case ETextureUsage_Diffuse:
            m_options.m_useGammaMips = true;
            m_options.m_format = ETextureFormat_DXT5;
            m_options.m_colorFormat = ETextureColor_RGBA_To_CoCgY;
            break;
        case ETextureUsage_Specular:
            m_options.m_useGammaMips = true;
            m_options.m_format = ETextureFormat_DXT1;
            m_options.m_colorFormat = ETextureColor_Default;
            break;
        case ETextureUsage_Default:
            m_options.m_useGammaMips = true;
            m_options.m_format = ETextureFormat_DXT5;
            m_options.m_colorFormat = ETextureColor_Default;
            break;
        case ETextureUsage_Bump:
            m_options.m_format = ETextureFormat_DXT5;
            m_options.m_colorFormat = ETextureColor_Normal_DXT5;
            break;
        case ETextureUsage_Font:
            m_options.m_format = ETextureFormat_DXT1;
            m_options.m_colorFormat = ETextureColor_Green_To_Aalpha;
            m_options.m_numLevels = 4; 
            m_options.m_useGammaMips = true;
            break;
        case ETextureUsage_Light:
            m_options.m_format = ETextureFormat_RGB565;
            m_options.m_useGammaMips = true;
            break;
        case ETextureUsage_LookUp_Mono:
            m_options.m_format = ETextureFormat_Intensity8;
            break;
        case ETextureUsage_LookUp_Alpha:
            m_options.m_format = ETextureFormat_Alpha;
            break;
        case ETextureUsage_LookUp_RGB1:
        case ETextureUsage_LookUp_RGBA:
            m_options.m_format = ETextureFormat_RGBA8;
            break;
        default:
            ionAssertReturnVoid(false, "Cannot generate proper texture options!");
            m_options.m_format = ETextureFormat_RGBA8;
        }
    }

    if (m_options.m_numLevels == 0) 
    {
        m_options.m_numLevels = 1;

        if (m_filter != ETextureFilter_Linear && m_filter != ETextureFilter_Nearest)
        {
            ionS32 width = m_options.m_width;
            ionS32 height = m_options.m_height;
            while (width > 1 || height > 1)
            {
                width >>= 1;
                height >>= 1;
                if ((m_options.m_format == ETextureFormat_DXT1 || m_options.m_format == ETextureFormat_DXT5) && ((width & 0x3) != 0 || (height & 0x3) != 0))
                {
                    break;
                }
                m_options.m_numLevels++;
            }
        }
    }
}

ionBool Texture::CreateFromFile(const eosString& _path, ETextureFilter _filter /*= ETextureFilter_Default*/, ETextureRepeat _repeat /*= ETextureRepeat_Clamp*/, ETextureUsage _usage /*= ETextureUsage_Default*/, ETextureType _type /*= ETextureType_2D*/)
{
    // clear before create
    Destroy();

    m_options.m_textureType = _type;

    m_filter = _filter;
    m_repeat = _repeat;
    m_usage = _usage;
    
    if (m_options.m_textureType == ETextureType_Cubic)
    {
        m_repeat = ETextureRepeat_Clamp;
        if (!LoadTexture3D(_path))
        {
            ionAssertReturnValue(false, "Cannot load 3d texture!", false);
        }
    }
    else if (m_options.m_textureType == ETextureType_Cubic)
    {
        if (!LoadTexture2D(_path))
        {
            ionAssertReturnValue(false, "Cannot load 2d texture!", false);
        }
    }
    else
    {
        ionAssertReturnValue(false, "Texture type invalid!", false);
    }

    return true;
}

ionBool Texture::CreateFromBinary(ionU32 _width, ionU32 _height, ionU32 _numChannels, ionU8* _buffer, VkDeviceSize _bufferSize)
{
    // clear before create
    Destroy();

    ionU8* buffer;
    VkDeviceSize bufferSize;
    ionBool releaseBuffer = false;
    if (_numChannels == 3)
    {
        bufferSize = _width * _height * 4;
        buffer = (ionU8*)eosNewRaw(sizeof(ionU8) * bufferSize, EOS_MEMORY_ALIGNMENT_SIZE);
        ionU8* rgba = buffer;
        ionU8* rgb = _buffer;
        for (ionSize i = 0; i < _width * _height; ++i)
        {
            for (int32_t j = 0; j < 3; ++j)
            {
                rgba[j] = rgb[j];
            }
            rgba += 4;
            rgb += 3;
        }
        _numChannels = 4;
        releaseBuffer = true;
    }
    else
    {
        buffer = _buffer;
        bufferSize = _bufferSize;
    }

    if (!LoadTexture( _width, _height, _numChannels, buffer))
    {
        if (releaseBuffer)
        {
            eosDeleteRaw(buffer);
        }
        releaseBuffer = false;
        ionAssertReturnValue(false, "Cannot load binary texture!", false);
    }

    if(releaseBuffer)
    {
        eosDeleteRaw(buffer);
    }

    return true;
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

ionU32 Texture::BitsPerFormat(ETextureFormat _format)
{
    switch (_format) 
    {
    case ETextureFormat_None:		        return 0;
    case ETextureFormat_RGBA8:		        return 32;
    case ETextureFormat_XRGB8:		        return 32;
    case ETextureFormat_RGB565:	            return 16;
    case ETextureFormat_Luminance8Alpha8:	return 16;
    case ETextureFormat_Alpha:		        return 8;
    case ETextureFormat_Luminance8:		    return 8;
    case ETextureFormat_Intensity8:		    return 8;
    case ETextureFormat_DXT1:		        return 4;
    case ETextureFormat_DXT5:		        return 8;
    case ETextureFormat_Depth:		        return 32;
    case ETextureFormat_X16:		        return 16;
    case ETextureFormat_Y16_X16:	        return 32;
    default:
        ionAssertReturnValue(false, "Invalid format!", 0);
        return 0;
    }
}

void Texture::UploadTextureToMemory(ionU32 _mipMapLevel, ionU32 _width, ionU32 _height, const ionU8* _buffer, ionU32 _index /* = 0 // index of texture for cube-map, 0 by default */ )
{
    if (IsCompressed()) 
    {
        _width = (_width + 3) & ~3;
        _height = (_height + 3) & ~3;
    }

    ionSize size = _width * _height * BitsPerFormat(m_options.m_format) / 8;

    VkBuffer buffer;
    VkCommandBuffer commandBuffer;
    ionSize offset = 0;
    ionU8* data = ionStagingBufferManager().Stage(size, 16, commandBuffer, buffer, offset);
    if (m_options.m_format == ETextureFormat_RGB565)
    {
        ionU8* tmpData = (ionU8*)_buffer;
        for (int i = 0; i < size; i += 2) 
        {
            data[i] = tmpData[i + 1];
            data[i + 1] = tmpData[i];
        }
    }
    else 
    {
        memcpy(data, _buffer, size);
    }

    VkBufferImageCopy imgCopy = {};
    imgCopy.bufferOffset = offset;
    //imgCopy.bufferRowLength = 0;
    //imgCopy.bufferImageHeight = _height;
    imgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imgCopy.imageSubresource.layerCount = 1;
    imgCopy.imageSubresource.mipLevel = _mipMapLevel;
    imgCopy.imageSubresource.baseArrayLayer = _index;
    //imgCopy.imageOffset.x = 0;
    //imgCopy.imageOffset.y = 0;
    //imgCopy.imageOffset.z = 0;
    imgCopy.imageExtent.width = _width;
    imgCopy.imageExtent.height = _height;
    imgCopy.imageExtent.depth = 1;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_options.m_numLevels;
    barrier.subresourceRange.baseArrayLayer = _index;               // 0 for 2d
    barrier.subresourceRange.layerCount = 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    vkCmdCopyBufferToImage(commandBuffer, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imgCopy);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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
    case ETextureFormat_X16: return VK_FORMAT_R16_UNORM;
    case ETextureFormat_Y16_X16: return VK_FORMAT_R16G16_UNORM;
    case ETextureFormat_RGB565: return VK_FORMAT_R5G6B5_UNORM_PACK16;
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