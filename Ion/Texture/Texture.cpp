// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Texture\Texture.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "Texture.h"

#include "TextureManager.h"

#include "../Renderer/StagingBufferManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../GPU/GpuDataStructure.h"
#include "../GPU/GpuMemoryAllocator.h"
#include "../GPU/GpuMemoryManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image_write.h"

#include "../Renderer/BaseBufferObject.h"

#include "CubemapHelper.h"

EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

TextureAllocator* Texture::GetAllocator()
{
	static HeapArea<Settings::kTextureAllocatorSize> memoryArea;
	static TextureAllocator memoryAllocator(memoryArea, "TextureFreeListAllocator");

	return &memoryAllocator;
}

Texture::Texture(VkDevice _vkDevice, const ionString& _name) :
    m_vkDevice(_vkDevice),
    m_name(_name)
{
    m_format = VK_FORMAT_UNDEFINED;
    m_image = VK_NULL_HANDLE;
    m_view = VK_NULL_HANDLE;
    m_layout = VK_IMAGE_LAYOUT_GENERAL;
    m_sampler = VK_NULL_HANDLE;

    m_optFilterMag = ETextureFilterMag_Linear;
    m_optFilterMin = ETextureFilterMin_Linear_MipMap_Linear;
    m_optRepeat = ETextureRepeat_Repeat;
    m_optUsage = ETextureUsage_RGBA;
    m_optTextureType = ETextureType_2D;
    m_optFormat = ETextureFormat_None;
}

Texture::~Texture()
{
    Destroy();
}

void Texture::ConvertFrom3ChannelTo4Channel(ionU32 _width, ionU32 _height, const ionU8* _inBuffer, ionU8* _outBuffer)
{
    ionU8* rgba = _outBuffer;
    const ionU8* rgb = _inBuffer;
    for (ionSize i = 0; i < _width * _height; ++i)
    {
        for (ionS32 j = 0; j < 3; ++j)
        {
            rgba[j] = rgb[j];
        }
        rgba += 4;
        rgb += 3;
    }
}

void Texture::UploadTextureBuffer(const ionU8* _buffer, ionU32 _component, ionU32 _index /*= 0  index of texture for cube-map, 0 by default */)
{
    if (_component == 3)
    {
        ionSize newBufferSize = m_width * m_height * 4;
        ionU8* newBuffer = (ionU8*)ionNewRaw(sizeof(ionU8) * newBufferSize, GetAllocator());
        memset(newBuffer, 0, sizeof(ionU8) * newBufferSize);
        ConvertFrom3ChannelTo4Channel(m_width, m_height, _buffer, newBuffer);
        UploadTextureToMemory(m_numLevels, m_width, m_height, newBuffer, _index);
        ionDeleteRaw(newBuffer, GetAllocator());
    }
    else
    {
        UploadTextureToMemory(m_numLevels, m_width, m_height, _buffer, _index);
    }
}

ionBool Texture::LoadTextureFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer)
{
    m_width = _width;
    m_height = _height;
    m_numLevels = 0;

    GenerateOptions();

    ionBool result = Create();
    if (result)
    {
        UploadTextureBuffer(_buffer, _component);
    }
    
    GenerateMipMaps();

    return result;
}

ionBool Texture::LoadTextureFromFile(const ionString& _path)
{
    ionS32 w = 0, h = 0, c = 0;
    ionU8* buffer = stbi_load(_path.c_str(), &w, &h, &c, 0);

    m_width = w;
    m_height = h;
    m_numLevels = 0;

    GenerateOptions();

    ionBool result = Create();
    if (result)
    {
        UploadTextureBuffer(buffer, c);
    }

    stbi_image_free(buffer);

    GenerateMipMaps();

    return result;
}

ionBool Texture::LoadCubeTextureFromFile(const ionString& _path)
{
    GenerateOptions();

    CubemapHelper cubemap;
    if (cubemap.Load(_path.c_str(), m_optFormat))
    {
        ionBool result = false;
        if (cubemap.Convert())
        {
            m_width = cubemap.GetSizePerFace();
            m_height = cubemap.GetSizePerFace();
            m_numLevels = cubemap.GetNumLevelPerFace();

            result = Create();
            if (result)
            {
                for (ionU32 i = 0; i < 6; ++i)
                {
                    UploadTextureBuffer((const ionU8*)cubemap.GetBufferPerFace(i), cubemap.GetComponent(), i);
                }
            }

            GenerateMipMaps();
        }

        cubemap.Unload();

        return result;
    }
    else
    {
        return false;
    }
}

ionBool Texture::LoadCubeTextureFromFiles(const ionVector<ionString, TextureAllocator, GetAllocator>& paths)
{
    ionS32 w = 0, h = 0, c = 0;

    ionU8* buffer = stbi_load(paths[0].c_str(), &w, &h, &c, 0);
    stbi_image_free(buffer);

    m_width = w;
    m_height = h;
    m_numLevels = 0;

    GenerateOptions();

    ionBool result = Create();
    if (result)
    {
        for (ionU32 i = 0; i < 6; ++i)
        {
            ionU8* buffer = stbi_load(paths[i].c_str(), &w, &h, &c, 0);

            UploadTextureBuffer(buffer, c, i);

            stbi_image_free(buffer);
        }
    }

    GenerateMipMaps();

    return result;
}

void Texture::GenerateMipMaps()
{
    // I do not generate mipmap if level is just 1
    if (m_numLevels < 2)
    {
        return;
    }

    ionSize size = m_width * m_height * BitsPerFormat(m_optFormat) / 8;


    ionU32 indexCount = (m_optTextureType == ETextureType_Cubic) ? 6 : 1;

    for (ionU32 face = 0; face < indexCount; ++face)
    {
        VkBuffer buffer;
        VkCommandBuffer commandBuffer;
        ionSize offset = 0;
        ionU8* data = ionStagingBufferManager().Stage(size, ION_MEMORY_ALIGNMENT_SIZE, commandBuffer, buffer, offset);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = m_image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = face;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        ionS32 mipWidth = static_cast<ionS32>(m_width);
        ionS32 mipHeight = static_cast<ionS32>(m_height);

        for (ionU32 i = 1; i < m_numLevels; ++i)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = face;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth / 2, mipHeight / 2, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = face;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = m_numLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }
}

void Texture::GenerateOptions()
{
    if (m_optFormat == ETextureFormat_None)
    {
        switch (m_optUsage) 
        {
        case ETextureUsage_Light:
            m_optFormat = ETextureFormat_RGB565;
            break;
        case ETextureUsage_Mono:
            m_optFormat = ETextureFormat_Intensity8;
            break;
        case ETextureUsage_Alpha:
            m_optFormat = ETextureFormat_Alpha;
            break;
        case ETextureUsage_Depth:
            m_optFormat = ETextureFormat_Depth;
            break;
        case ETextureUsage_RGB1:
        case ETextureUsage_RGBA:
            m_optFormat = ETextureFormat_RGBA8;
            break;
        case ETextureUsage_RGB:
            m_optFormat = ETextureFormat_XRGB8;
            break;
        case ETextureUsage_SkyboxHDR:
            m_optFormat = ETextureFormat_HDR;
            break;
        case ETextureUsage_Skybox:
            m_optFormat = ETextureFormat_RGBA8;
            break;
        default:
            ionAssertReturnVoid(false, "Cannot generate proper texture options!");
            m_optFormat = ETextureFormat_RGBA8;
        }
    }

    if (m_numLevels == 0) 
    {
        m_numLevels = 1;

        if (m_optFilterMin > ETextureFilterMin_Nearest)
        {
            ionS32 width = m_width;
            ionS32 height = m_height;
            while (width > 1 || height > 1)
            {
                width >>= 1;
                height >>= 1;

                m_numLevels++;
            }
        }
    }
}

ionBool Texture::CreateFromFile(const ionString& _path)
{
    if (m_optTextureType == ETextureType_Cubic)
    {
        m_optRepeat = ETextureRepeat_Clamp;

        ionBool isSingleFile = true;
        FILE *filecheck = nullptr;
        if (fopen_s(&filecheck, _path.c_str(), "r") == 0)
        {
            fclose(filecheck);
            isSingleFile = true;
        }
        else 
        {
            isSingleFile = false;
        }

        if (isSingleFile)
        {
            // "cross version" (both top vertical or horizontal)
            if (!LoadCubeTextureFromFile(_path))
            {
                ionAssertReturnValue(false, "Cannot load 3d texture!", false);
            }
        }
        else
        {
            // Adding the side name to the file with "_": for instance: "test.png" become "test_left.png", "test_top.png" etc etc
			ionString ext;
			ionString path;
            ionSize i = _path.rfind('.', _path.length());
            if (i != std::string::npos)
            {
                path = _path.substr(0, i);
                ext = _path.substr(i + 1, _path.length() - i);
            }

            if (ext.empty())
            {
                ionAssertReturnValue(false, "Extension is not provided!", false);
            }

			ionString suffix[6] { "_right.", "_left.", "_top.", "_bottom.", "_front.", "_back." };
            ionVector<ionString, TextureAllocator, GetAllocator> paths; paths.resize(6);
            for (ionU32 i = 0; i < 6; ++i)
            {
                paths[i] = path + suffix[i] + ext;
            }

            if (!LoadCubeTextureFromFiles(paths))
            {
                ionAssertReturnValue(false, "Cannot load 3d texture!", false);
            }
        }
    }
    else if (m_optTextureType == ETextureType_2D)
    {
        if (!LoadTextureFromFile(_path))
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

ionBool Texture::CreateFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer, VkDeviceSize _bufferSize)
{
    if (!LoadTextureFromBuffer( _width, _height, _component, _buffer))
    {
        ionAssertReturnValue(false, "Cannot load binary texture!", false);
    }

    return true;
}

ionBool Texture::GenerateTexture(ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureRepeat _repeat, ETextureType _type /*= ETextureType_2D*/, ionU32 _numLevel /*= 1*/)
{
    m_width = _width;
    m_height = _height;
    m_optFormat = _format;
    m_optRepeat = _repeat;
    m_numLevels = _numLevel;
    m_optTextureType = _type;

    GenerateOptions();

    return Create();
}

ionBool Texture::Create()
{
    // clear before create
    Destroy();

    if (CreateSampler())
    {
        m_format = GetVulkanFormatFromTextureFormat(m_optFormat);

        {
            VkImageCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            createInfo.flags = (m_optTextureType == ETextureType_Cubic) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
            createInfo.imageType = VK_IMAGE_TYPE_2D;
            createInfo.format = m_format;
            createInfo.extent.width = m_width;
            createInfo.extent.height = m_height;
            createInfo.extent.depth = 1;
            createInfo.mipLevels = m_numLevels;
            createInfo.arrayLayers = (m_optTextureType == ETextureType_Cubic) ? 6 : 1;
            createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            //createInfo.usage = m_numLevels > 1 ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

            if (m_optFormat == ETextureFormat_Depth)
            {
                createInfo.usage &= ~VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
            createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VkResult result = vkCreateImage(m_vkDevice, &createInfo, vkMemory, &m_image);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image!", false);
        }

        VkMemoryRequirements memoryRequirements = {};
        vkGetImageMemoryRequirements(m_vkDevice, m_image, &memoryRequirements);

        {
			GpuMemoryCreateInfo createInfo = {};
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
            createInfo.viewType = (m_optTextureType == ETextureType_Cubic) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_format;
            createInfo.components = GetVulkanComponentMappingFromTextureFormat(m_optFormat);
            createInfo.subresourceRange.aspectMask = (m_optFormat == ETextureFormat_Depth) ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = m_numLevels;
            createInfo.subresourceRange.layerCount = (m_optTextureType == ETextureType_Cubic) ? 6 : 1;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.baseArrayLayer = 0;

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
    case ETextureFormat_None:               return 0;
    case ETextureFormat_RGBA8:              return 32;
    case ETextureFormat_XRGB8:              return 32;
    case ETextureFormat_RGB565:             return 16;
    case ETextureFormat_Luminance8Alpha8:   return 16;
    case ETextureFormat_Alpha:              return 8;
    case ETextureFormat_Luminance8:         return 8;
    case ETextureFormat_Intensity8:         return 8;
    case ETextureFormat_HDR:                return 128; //64;//48;
    case ETextureFormat_BRDF:               return 32;
    case ETextureFormat_Depth:              return 32;  // should be 24, but it works with 32
    case ETextureFormat_Irradiance:             return 128;
    case ETextureFormat_PrefilteredEnvironment: return 64;
    default:
        ionAssertReturnValue(false, "Invalid format!", 0);
        return 0;
    }
}

void Texture::UploadTextureToMemory(ionU32 _mipMapLevel, ionU32 _width, ionU32 _height, const ionU8* _buffer, ionU32 _index /* = 0 // index of texture for cube-map, 0 by default */ )
{
    ionSize size = _width * _height * BitsPerFormat(m_optFormat) / 8;

    VkBuffer buffer;
    VkCommandBuffer commandBuffer;
    ionSize offset = 0;
    ionU8* data = ionStagingBufferManager().Stage(size, ION_MEMORY_ALIGNMENT_SIZE, commandBuffer, buffer, offset);
    if (m_optFormat == ETextureFormat_RGB565)
    {
        ionU8* tmpData = (ionU8*)_buffer;
        for (ionU8 i = 0; i < size; i += 2)
        {
            data[i] = tmpData[i + 1];
            data[i + 1] = tmpData[i];
        }
    }
    else
    {
		MemUtils::MemCpy(data, _buffer, size);
    }

    VkBufferImageCopy imgCopy = {};
    imgCopy.bufferOffset = offset;
    imgCopy.bufferRowLength = 0;
    imgCopy.bufferImageHeight = 0;  //_height;
    imgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imgCopy.imageSubresource.layerCount = 1;
    imgCopy.imageSubresource.mipLevel = 0;  // _mipMapLevel;
    imgCopy.imageSubresource.baseArrayLayer = _index;           // 0 for 2d
    imgCopy.imageOffset.x = 0;
    imgCopy.imageOffset.y = 0;
    imgCopy.imageOffset.z = 0;
    imgCopy.imageExtent.width = _width;
    imgCopy.imageExtent.height = _height;
    imgCopy.imageExtent.depth = 1;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_numLevels;
    barrier.subresourceRange.baseArrayLayer = _index;               // 0 for 2d
    barrier.subresourceRange.layerCount = 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    vkCmdCopyBufferToImage(commandBuffer, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imgCopy);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = m_numLevels > 1 ? VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
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
    case ETextureFormat_RGB565: return VK_FORMAT_R8G8B8_UNORM;      // fall back on this format
    case ETextureFormat_HDR: return VK_FORMAT_R32G32B32A32_SFLOAT;// VK_FORMAT_R16G16B16A16_SFLOAT;
    case ETextureFormat_BRDF: return VK_FORMAT_R16G16_SFLOAT;
    case ETextureFormat_Depth: return ionTextureManger().GetDepthFormat(); //VK_FORMAT_R8G8B8_UNORM;
    case ETextureFormat_Irradiance: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ETextureFormat_PrefilteredEnvironment: return VK_FORMAT_R16G16B16A16_SFLOAT;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

VkComponentMapping Texture::GetVulkanComponentMappingFromTextureFormat(ETextureFormat _format)
{
    VkComponentMapping componentMapping = 
    {
        VK_COMPONENT_SWIZZLE_ZERO,
        VK_COMPONENT_SWIZZLE_ZERO,
        VK_COMPONENT_SWIZZLE_ZERO,
        VK_COMPONENT_SWIZZLE_ZERO
    };

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

void Texture::GetVulkanFiltersFromTextureFilters(ETextureFilterMin _min0, ETextureFilterMag _mag0, VkFilter& _min, VkFilter& _mag, VkSamplerMipmapMode& _mipmap)
{
    switch (_mag0)
    {
    case ETextureFilterMag_Nearest:
        _mag = VK_FILTER_NEAREST;
        break;
    case ETextureFilterMag_Linear:
    default:
        _mag = VK_FILTER_LINEAR;
        break;
    }

    switch (_min0)
    {
    case ETextureFilterMin_Linear:
        _min = VK_FILTER_LINEAR;
        _mipmap = VK_SAMPLER_MIPMAP_MODE_LINEAR;        // by default is 0 set in the sampler, that means linear mipmap
        break;
    case ETextureFilterMin_Nearest:
        _min = VK_FILTER_NEAREST;
        _mipmap = VK_SAMPLER_MIPMAP_MODE_LINEAR;        // by default is 0 set in the sampler, that means linear mipmap
        break;
    case ETextureFilterMin_Nearest_MipMap_Nearest:
        _min = VK_FILTER_NEAREST;
        _mipmap = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case ETextureFilterMin_Linear_MipMap_Nearest:
        _min = VK_FILTER_LINEAR;
        _mipmap = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case ETextureFilterMin_Nearest_MipMap_Linear:
        _min = VK_FILTER_NEAREST;
        _mipmap = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;
    case ETextureFilterMin_Linear_MipMap_Linear:
    default:
        _min = VK_FILTER_LINEAR;
        _mipmap = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;
    }
}

ionBool Texture::CreateSampler()
{
    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.maxAnisotropy = ionTextureManger().GetSamplerAnysotropy() ? static_cast<ionFloat>(m_maxAnisotropy) : 1.0f;
    createInfo.anisotropyEnable = ionTextureManger().GetSamplerAnysotropy();
    createInfo.compareEnable = (m_optFormat == ETextureFormat_Depth);
    createInfo.compareOp = (m_optFormat == ETextureFormat_Depth) ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_NEVER;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.minLod = 0;
    createInfo.maxLod = static_cast<ionFloat>(m_numLevels);

    GetVulkanFiltersFromTextureFilters(m_optFilterMin, m_optFilterMag, createInfo.minFilter, createInfo.magFilter, createInfo.mipmapMode);

    switch (m_optRepeat) 
    {
    case ETextureRepeat_Repeat:
        createInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; //VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
    case ETextureRepeat_Clamp:
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; //VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
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
    case ETextureRepeat_Mirrored:
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        break;
    case ETextureRepeat_MirroredClamp:
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        break;
    case ETextureRepeat_Custom:
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        createInfo.addressModeU = m_optCustomRepeat[0];
        createInfo.addressModeV = m_optCustomRepeat[1];
        createInfo.addressModeW = m_optCustomRepeat[2];
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
        m_allocation = GpuMemoryAllocation();

        vkDestroyImageView(m_vkDevice, m_view, vkMemory);

        m_view = VK_NULL_HANDLE;
        m_image = VK_NULL_HANDLE;
    }
}

ionBool Texture::Save(const ionString& _path) const
{
	ionString ext;
	ionString path;
    ionSize i = _path.rfind('.', _path.length());
    if (i != std::string::npos)
    {
        path = _path.substr(0, i);
        ext = _path.substr(i + 1, _path.length() - i);
    }

    if (ext.empty())
    {
        ionAssertReturnValue(false, "Extension is not provided!", false);
    }

    ionU32 component = BitsPerFormat(m_optFormat) / 8;
    ionSize size = m_width * m_height * component;

    VkBuffer buffer;
    VkCommandBuffer commandBuffer;
    ionSize offset = 0;
    ionU8* data = ionStagingBufferManager().Stage(size, ION_MEMORY_ALIGNMENT_SIZE, commandBuffer, buffer, offset);

    if (data == nullptr)
    {
        return false;
    }

    memcpy(data, &m_image, size);


    if (ext.compare("png") == 0)
    {
        return stbi_write_png(_path.c_str(), m_width, m_height, component, data, m_width * component * sizeof(ionU8)) == 1;
    }
    else if(ext.compare("bmp") == 0)
    {
        return  stbi_write_bmp(_path.c_str(), m_width, m_height, component, data) == 1;
    }
    else if (ext.compare("tga") == 0)
    {
        return stbi_write_tga(_path.c_str(), m_width, m_height, component, data) == 1;
    }
    else if (ext.compare("jpg") == 0)
    {
        return stbi_write_jpg(_path.c_str(), m_width, m_height, component, data, 100) == 1;
    }
    else if (ext.compare("hdr") == 0)
    {
        //return stbi_write_hdr(_path.c_str(), m_width, m_height, component, data) == 1;
        ionAssertReturnValue(false, "HDR file format is not supported!", false);
    }
    else 
    {
        ionAssertReturnValue(false, "Format not supported!", false);
    }
}

ION_NAMESPACE_END