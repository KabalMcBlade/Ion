// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Texture\CubemapHelper.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "CubemapHelper.h"

#include "TextureManager.h"

#include "../Renderer/BaseBufferObject.h"

//#include "../Renderer/StagingBufferManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#include "../Core/MemorySettings.h"

#include "../Dependencies/Miscellaneous/stb_image.h"
/*
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image_write.h"
*/

EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN

CubemapHelperAllocator* CubemapHelper::GetAllocator()
{
	static HeapArea<Settings::kCubeMapHelperAllocatorSize> memoryArea;
	static CubemapHelperAllocator memoryAllocator(memoryArea, "CubemapHelperFreeListAllocator");

	return &memoryAllocator;
}

CubemapHelper::CubemapHelper()
{
    Clear();
}

CubemapHelper::~CubemapHelper()
{
    Clear();
}

ionBool CubemapHelper::Load(const ionString& _path, ETextureFormat _format)
{
    m_format = _format;

    if (stbi_is_hdr(_path.c_str()))
    {
        m_isHDR = true;
        m_buffer = stbi_loadf(_path.c_str(), &m_width, &m_height, &m_component, 0);
    }
    else
    {
        m_isHDR = false;
        m_buffer = stbi_load(_path.c_str(), &m_width, &m_height, &m_component, 0);
    }

    m_numLevels = CalculateMipMapPerFace(m_width, m_height);

    return (m_buffer != nullptr);
}

void CubemapHelper::Unload()
{
    if (m_buffer != nullptr)
    {
        stbi_image_free(m_buffer);
    }

    for (ionU32 i = 0; i < 6; ++i)
    {
        if (m_output[i] != nullptr)
        {
            ionDeleteRaw(m_output[i], GetAllocator());
        }
    }
    Clear();
}

void CubemapHelper::Clear()
{
    m_width = 0;
    m_height = 0;
    m_component = 0;
    m_numLevels = 0;
    m_format = ETextureFormat_None;
    m_buffer = nullptr;
    m_sizePerFace = 0;
    m_bufferSizePerFace = 0;
    m_numLevelsPerFace = 0;
    m_isHDR = false;
    for (ionU32 i = 0; i < 6; ++i)
    {
        m_output[i] = nullptr;
    }
}

ionU32 CubemapHelper::CalculateMipMapPerFace(ionU32 _width, ionU32 _height)
{
    ionU32 numLevels = 1;

    ionS32 width = _width;
    ionS32 height = _height;
    while (width > 1 || height > 1)
    {
        width >>= 1;
        height >>= 1;

        numLevels++;
    }

    return numLevels;
}

ionBool CubemapHelper::IsLatLong()
{
    // it is a trick, not a real check
    const ionFloat aspect = (ionFloat)m_width / (ionFloat)m_height;
    return std::fabsf(aspect - 2.0f) < 0.00001f;
}

ionBool CubemapHelper::IsCubeCross()
{
    const ionBool isVertical = (m_width / 3 == m_height / 4) && (m_width % 3 == 0) && (m_height % 4 == 0);
    const ionBool isHorizontal = (m_width / 4 == m_height / 3) && (m_width % 4 == 0) && (m_height % 3 == 0);

    if (!isVertical && !isHorizontal)
    {
        return false;
    }

    return true;
}

ionBool CubemapHelper::Convert()
{
    if (IsCubeCross())
    {
        CubemapFromCross();
        return true;
    }
    else if (IsLatLong())
    {
        CubemapFromLatLong();
        return true;
    }
    else
    {
        ionAssertReturnValue(false, "Format not supported: Cubemap generation only from Cross or LatLong!", false);
    }
}

void CubemapHelper::CopyBufferRegion(const void* _source, void* _dest, ionU32 _sourceImageWidth, ionU32 _component, ionU32 _bppPerChannel, ionU32 _destSize, ionU32 _x, ionU32 _y)
{
    const ionU8* sourceFace = (const ionU8*)_source;
    ionU8* destFace = (ionU8*)_dest;
    for (ionU32 i = 0; i < _destSize; ++i)  // _destSize is the height but because we are in "cube map" width and height are the same
    {
		MemUtils::MemCpy(&destFace[i * _destSize * _component * _bppPerChannel], &sourceFace[(i + _y) * _sourceImageWidth * _component * _bppPerChannel + (_x * _component * _bppPerChannel)], _destSize * _component * _bppPerChannel);
    }
}

void CubemapHelper::GenerateCubemapFromCrossVertical(const void* _source, void* _dest[6], ionU32 _bpp)
{
    const ionU32 perChannel = _bpp / 32;

    // right
    _dest[0] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[0], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace * 2, m_sizePerFace);

    // left
    _dest[1] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[1], m_width, m_component, perChannel, m_sizePerFace, 0, m_sizePerFace);

    // top
    _dest[2] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[2], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, 0);

    // bottom
    _dest[3] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[3], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace * 2);

    // front
    _dest[4] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[4], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace);

    // back
    _dest[5] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());

    // here I need to read from bottom to top and  right to left....
    const ionU8* sourceFace = (const ionU8*)_source;
    ionU8* destFace = (ionU8*)(_dest[5]);
    ionU32 _x = m_sizePerFace, _y = m_sizePerFace * 3;
    for (ionS32 i = (m_sizePerFace - 1), j = 0; i >= 0; --i, ++j)
    {
		MemUtils::MemCpy(&(destFace)[j * m_sizePerFace * m_component * perChannel], &sourceFace[(i + _y) * m_width * m_component * perChannel + (_x * m_component * perChannel)], m_sizePerFace * m_component * perChannel);
    }
    for (ionU32 y = 0; y < m_sizePerFace; ++y)
    {
        for (ionU32 x = 0; x < m_sizePerFace / 2; ++x)
        {
            for (ionS32 c = 0; c < m_component; ++c)
            {
                for (ionU32 b = 0; b < perChannel; ++b)
                {
                    ionU8 swap = destFace[(m_sizePerFace * m_component * y) + (x * m_component) + (c * perChannel) + b];
                    destFace[(m_sizePerFace * m_component * y) + (x * m_component) + (c * perChannel) + b] = destFace[(m_sizePerFace * m_component * y) + (((m_sizePerFace - x) * m_component) - m_component) + (c * perChannel) + b];
                    destFace[(m_sizePerFace * m_component * y) + (((m_sizePerFace - x) * m_component) - m_component) + (c * perChannel) + b] = swap;
                }
            }
        }
    }
}

void CubemapHelper::GenerateCubemapFromCrossHorizontal(const void* _source, void* _dest[6], ionU32 _bpp)
{
    const ionU32 perChannel = _bpp / 32;

    // right
    _dest[0] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[0], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace * 2, m_sizePerFace);

    // left
    _dest[1] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[1], m_width, m_component, perChannel, m_sizePerFace, 0, m_sizePerFace);

    // top
    _dest[2] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[2], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, 0);

    // bottom
    _dest[3] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[3], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace * 2);

    // front
    _dest[4] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[4], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace);

    // back
    _dest[5] = ionNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, GetAllocator());
    CopyBufferRegion(_source, _dest[5], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace * 3, m_sizePerFace);
}

void CubemapHelper::CubemapFromCross()
{
    const ionBool isVertical = (m_width / 3 == m_height / 4) && (m_width % 3 == 0) && (m_height % 4 == 0);
    const ionBool isHorizontal = !isVertical;

    if (isVertical)
    {
        m_sizePerFace = m_width / 3;
        m_numLevelsPerFace = CalculateMipMapPerFace(m_sizePerFace, m_sizePerFace);
        GenerateCubemapFromCrossVertical(m_buffer, m_output, Texture::BitsPerFormat(m_format));
    }
    else
    {
        m_sizePerFace = m_width / 4;
        m_numLevelsPerFace = CalculateMipMapPerFace(m_sizePerFace, m_sizePerFace);
        GenerateCubemapFromCrossHorizontal(m_buffer, m_output, Texture::BitsPerFormat(m_format));
    }
}

ionS32 CubemapHelper::Clamp(ionS32 _n, ionS32 _lower, ionS32 _upper)
{
    return std::max(_lower, std::min(_n, _upper));
}

void CubemapHelper::CubemapFromLatLong()
{
    m_sizePerFace = 1 << std::lround(std::log(m_width / 4) / std::log(2));
    m_numLevelsPerFace = CalculateMipMapPerFace(m_sizePerFace, m_sizePerFace);

    // I need to convert to rgba if is not from source right here
    if (m_component == 3)
    {
        m_component = 4;

        ionU32 bppPerChannel = Texture::BitsPerFormat(m_format) / 32;

        void* rgba = ionNewRaw(m_width * m_height * m_component * bppPerChannel, GetAllocator());

        if (m_isHDR)
        {
            ionFloat* rgbaConverter = (ionFloat*)rgba;
            const ionFloat* rgb = (const ionFloat*)m_buffer;
            for (ionSize i = 0; i < m_width * m_height; ++i)
            {
                for (ionS32 j = 0; j < 3; ++j)
                {
                    rgbaConverter[j] = rgb[j];
                }
                rgbaConverter += 4;
                rgb += 3;
            }

            GenerateCubemapFromLatLong<ionFloat>(rgba, m_output, Texture::BitsPerFormat(m_format));
        }
        else
        {
            ionU8* rgbaConverter = (ionU8*)rgba;
            const ionU8* rgb = (const ionU8*)m_buffer;
            for (ionSize i = 0; i < m_width * m_height; ++i)
            {
                for (ionS32 j = 0; j < 3; ++j)
                {
                    rgbaConverter[j] = rgb[j];
                }
                rgbaConverter += 4;
                rgb += 3;
            }

            GenerateCubemapFromLatLong<ionU8>(rgba, m_output, Texture::BitsPerFormat(m_format));
        }

        ionDeleteRaw(rgba, GetAllocator());
    }
    else
    {
        if (m_isHDR)
        {
            GenerateCubemapFromLatLong<ionFloat>(m_buffer, m_output, Texture::BitsPerFormat(m_format));
        }
        else
        {
            GenerateCubemapFromLatLong<ionU8>(m_buffer, m_output, Texture::BitsPerFormat(m_format));
        }
    }
}


ION_NAMESPACE_END