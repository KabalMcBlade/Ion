#include "Texture.h"

#include "TextureManager.h"

#include "../Renderer/StagingBufferManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image_write.h"

#include "../Renderer/BaseBufferObject.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Texture::Texture(VkDevice _vkDevice, const eosString& _name) :
    m_vkDevice(_vkDevice),
    m_name(_name)
{
    m_format = VK_FORMAT_UNDEFINED;
    m_image = VK_NULL_HANDLE;
    m_view = VK_NULL_HANDLE;
    m_layout = VK_IMAGE_LAYOUT_GENERAL;
    m_sampler = VK_NULL_HANDLE;

    m_optFilter = ETextureFilter_Default;
    m_optRepeat = ETextureRepeat_Repeat;
    m_optUsage = ETextureUsage_RGBA;
    m_optTextureType = ETextureType_2D;
    m_optFormat = ETextureFormat_RGBA8;
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
        for (int32_t j = 0; j < 3; ++j)
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
        ionU8* newBuffer = (ionU8*)eosNewRaw(sizeof(ionU8) * newBufferSize, ION_MEMORY_ALIGNMENT_SIZE);
        memset(newBuffer, 0, sizeof(ionU8) * newBufferSize);
        ConvertFrom3ChannelTo4Channel(m_width, m_height, _buffer, newBuffer);
        UploadTextureToMemory(m_numLevels, m_width, m_height, newBuffer, _index);
        eosDeleteRaw(newBuffer);
    }
    else
    {
        UploadTextureToMemory(m_numLevels, m_width, m_height, _buffer, _index);
    }
}

ionBool Texture::LoadTextureFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _buffer)
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

ionBool Texture::LoadTextureFromFile(const eosString& _path)
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

void Texture::CopyBufferRegion(const ionU8* _source, ionU32 _sourceWidth, ionU32 _sourceHeight, ionU32 _sourceComponentCount, ionU8* _dest, ionU32 _destWidth, ionU32 _destHeight, ionU32 _x, ionU32 _y)
{
    ION_UNUSED(_sourceHeight);
    for (ionU32 i = 0; i < _destHeight; ++i)
    {
        CopyBuffer(&_dest[i * _destWidth * _sourceComponentCount], &_source[(i + _y) * _sourceWidth * _sourceComponentCount + (_x * _sourceComponentCount)], _destWidth * _sourceComponentCount * sizeof(ionU8));
    }
}

ionBool Texture::GenerateCubemapFromCross(const ionU8* _buffer, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _outBuffers[6])
{
    if (m_numLevels > 1)
    {
        return false;
    }

    if ((_width / 3 == _height / 4) && (_width % 3 == 0) && (_height % 4 == 0))
    {
        GenerateCubemapFromCrossVertical(_buffer, _width, _height, _component, _outBuffers);
    }
    else if ((_width / 4 == _height / 3) && (_width % 4 == 0) && (_height % 3 == 0))
    {
        GenerateCubemapFromCrossHorizontal(_buffer, _width, _height, _component, _outBuffers);
    }
    else
    {
        return false;
    }

    return true;
}

void Texture::GenerateCubemapFromCrossVertical(const ionU8* _buffer, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _outBuffers[6])
{
    m_width = _width / 3;
    m_height = _height / 4;

    // { "_right.", "_left.", "_top.", "_bottom.", "_front.", "_back." };

    // right
    _outBuffers[0] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[0], m_width, m_height, m_width * 2, m_height);

    // left
    _outBuffers[1] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[1], m_width, m_height, 0, m_height);

    // top
    _outBuffers[2] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[2], m_width, m_height, m_width, 0);

    // bottom
    _outBuffers[3] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[3], m_width, m_height, m_width, m_height * 2);

    // front
    _outBuffers[4] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[4], m_width, m_height, m_width, m_height);

    // back
    _outBuffers[5] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    //CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[5], m_width, m_height, m_width, m_height * 3);
    // here I need to read from bottom to top and  right to left....
    ionU32 _x = m_width, _y = m_height * 3;
    for (ionS32 i = (m_height - 1), j = 0; i >= 0; --i, ++j)
    {
        CopyBuffer(&(_outBuffers[5])[j * m_width * _component], &_buffer[(i + _y) * _width * _component + (_x * _component)], m_width * _component * sizeof(ionU8));
    }
    for (ionU32 y = 0; y < m_height; ++y)
    {
        for (ionU32 x = 0; x < m_width / 2; ++x)
        {
            for (ionU32 c = 0; c < _component; ++c)
            {
                ionU8 swap = _outBuffers[5][(m_width * _component * y) + (x * _component) + c];
                _outBuffers[5][(m_width * _component * y) + (x * _component) + c] = _outBuffers[5][(m_width * _component * y) + (((m_width - x) * _component) - _component) + c];
                _outBuffers[5][(m_width * _component * y) + (((m_width - x) * _component) - _component) + c] = swap;

            }
        }
    }
}

void Texture::GenerateCubemapFromCrossHorizontal(const ionU8* _buffer, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _outBuffers[6])
{
    m_width = _width / 4;
    m_height = _height / 3;

    // { "_right.", "_left.", "_top.", "_bottom.", "_front.", "_back." };

    // right
    _outBuffers[0] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[0], m_width, m_height, m_width * 2, m_height);

    // left
    _outBuffers[1] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[1], m_width, m_height, 0, m_height);

    // top
    _outBuffers[2] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[2], m_width, m_height, m_width, 0);

    // bottom
    _outBuffers[3] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[3], m_width, m_height, m_width, m_height * 2);

    // front
    _outBuffers[4] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[4], m_width, m_height, m_width, m_height);

    // back
    _outBuffers[5] = (ionU8 *)eosNewRaw(m_height * m_width * _component * sizeof(ionU8), ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_buffer, _width, _height, _component, _outBuffers[5], m_width, m_height, m_width * 3, m_height);
}

ionBool Texture::LoadCubeTextureFromFile(const eosString& _path)
{
    ionS32 w = 0, h = 0, c = 0;
    m_numLevels = 0;

    ionU8* buffer = stbi_load(_path.c_str(), &w, &h, &c, 0);

    // will be replaced
    m_width = w;
    m_height = h;

    //first I need to change the component count for the image
    ionU8* newBuffer = nullptr;
    ionU8* finalBuffer = buffer;
    if (c == 3)
    {
        ionSize newBufferSize = m_width * m_height * 4;
        newBuffer = (ionU8*)eosNewRaw(sizeof(ionU8) * newBufferSize, ION_MEMORY_ALIGNMENT_SIZE);
        memset(newBuffer, 0, sizeof(ionU8) * newBufferSize);
        ConvertFrom3ChannelTo4Channel(m_width, m_height, buffer, newBuffer);

        c = 4;
        finalBuffer = newBuffer;
    }


    ionU8* buffers[6];
    GenerateCubemapFromCross(finalBuffer, w, h, c, buffers);

    if (newBuffer != nullptr)
    {
        eosDeleteRaw(newBuffer);    // this is pointed by finalBuffer, so final buffer is cleared too but just dirty
        stbi_image_free(buffer);    // this is another allocation at this point
    }
    else
    {
        stbi_image_free(buffer);    // this is pointed by finalBuffer, so final buffer is cleared too but just dirty
    }

    GenerateOptions();

    ionBool result = Create();
    if (result)
    {
        for (ionU32 i = 0; i < 6; ++i)
        {
            UploadTextureBuffer(buffers[i], c, i);
        }
    }

    for (ionU32 i = 0; i < 6; ++i)
    {
        eosDeleteRaw(buffers[i]);
    }

    GenerateMipMaps();

    return result;
}

ionBool Texture::LoadCubeTextureFromFiles(const eosVector(eosString)& paths)
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

            UploadTextureBuffer(buffer, c);

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
    barrier.subresourceRange.baseArrayLayer = 0;
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
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth / 2, mipHeight / 2, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
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
        case ETextureUsage_RGB1:
        case ETextureUsage_RGBA:
            m_optFormat = ETextureFormat_RGBA8;
            break;
        case ETextureUsage_RGB:
            m_optFormat = ETextureFormat_XRGB8;
            break;
        case ETextureUsage_Skybox:
            m_optFormat = ETextureFormat_HDR;
            break;
        default:
            ionAssertReturnVoid(false, "Cannot generate proper texture options!");
            m_optFormat = ETextureFormat_RGBA8;
        }
    }

    if (m_numLevels == 0) 
    {
        m_numLevels = 1;

        if (m_optFilter == ETextureFilter_Default)
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

ionBool Texture::CreateFromFile(const eosString& _path)
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
                ionAssertReturnValue(false, "Extension is not provided!", false);
            }

            eosString suffix[6] { "_right.", "_left.", "_top.", "_bottom.", "_front.", "_back." };
            eosVector(eosString) paths; paths.resize(6);
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

ionBool Texture::CreateFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _buffer, VkDeviceSize _bufferSize)
{
    if (!LoadTextureFromBuffer( _width, _height, _component, _buffer))
    {
        ionAssertReturnValue(false, "Cannot load binary texture!", false);
    }

    return true;
}

ionBool Texture::GenerateTexture(ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureRepeat _repeat, ionU32 _numLevel /*= 1*/)
{
    m_width = _width;
    m_height = _height;
    m_optFormat = _format;
    m_optRepeat = _repeat;
    m_numLevels = _numLevel;

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
            createInfo.samples = VK_SAMPLE_COUNT_1_BIT;//m_sampleCount;
            createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            createInfo.usage = m_numLevels > 1 ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT : VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            
            if (m_optFormat == ETextureFormat_BRDF)
            {
                createInfo.usage = createInfo.usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
            
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
            createInfo.viewType = (m_optTextureType == ETextureType_Cubic) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_format;
            createInfo.components = GetVulkanComponentMappingFromTextureFormat(m_optFormat);
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = m_numLevels;
            createInfo.subresourceRange.layerCount = (m_optTextureType == ETextureType_Cubic) ? 6 : 1;
            createInfo.subresourceRange.baseMipLevel = 0;
            //createInfo.subresourceRange.baseArrayLayer = 0;

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

ionU32 Texture::BitsPerFormat(ETextureFormat _format) const
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
    case ETextureFormat_HDR:                return 48;
    case ETextureFormat_BRDF:               return 32;
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
        for (int i = 0; i < size; i += 2) 
        {
            data[i] = tmpData[i + 1];
            data[i + 1] = tmpData[i];
        }
    }
    else 
    {
        CopyBuffer(data, _buffer, size);
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
    barrier.newLayout = m_numLevels > 1 ? VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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
    case ETextureFormat_HDR: return VK_FORMAT_R16G16B16A16_SFLOAT;
    case ETextureFormat_BRDF: return VK_FORMAT_R16G16_SFLOAT;
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
    case ETextureFormat_HDR:
        componentMapping.r = VK_COMPONENT_SWIZZLE_R;
        componentMapping.g = VK_COMPONENT_SWIZZLE_G;
        componentMapping.b = VK_COMPONENT_SWIZZLE_B;
        componentMapping.a = VK_COMPONENT_SWIZZLE_ONE;
        break;
    case ETextureFormat_BRDF:
        componentMapping.r = VK_COMPONENT_SWIZZLE_R;
        componentMapping.g = VK_COMPONENT_SWIZZLE_R;
        componentMapping.b = VK_COMPONENT_SWIZZLE_G;
        componentMapping.a = VK_COMPONENT_SWIZZLE_G;
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
    createInfo.maxAnisotropy = (ionFloat)m_maxAnisotropy;
    createInfo.anisotropyEnable = m_maxAnisotropy > 1 ? VK_TRUE : VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_NEVER; // : VK_COMPARE_OP_ALWAYS ?
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.minLod = 0;
    createInfo.maxLod = static_cast<ionFloat>(m_numLevels);

    switch (m_optFilter) 
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
    
    switch (m_optRepeat) 
    {
    case ETextureRepeat_Repeat:
        createInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
    case ETextureRepeat_Clamp:
        createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
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

    if (m_optFormat == ETextureFormat_BRDF)
    {
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
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

ionBool Texture::Save(const eosString& _path) const
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
        return stbi_write_png(_path.c_str(), m_width, m_height, component, data, m_width * m_height * sizeof(ionU8)) == 1;
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
        //return stbi_write_hdr(_path.c_str(), m_width, m_height, component, data);
        ionAssertReturnValue(false, "HDR file format is not supported!", false);
    }
    else 
    {
        ionAssertReturnValue(false, "Format not supported!", false);
    }
}

ION_NAMESPACE_END