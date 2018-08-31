#include "CubemapHelper.h"

#include "TextureManager.h"

#include "../Renderer/BaseBufferObject.h"

//#include "../Renderer/StagingBufferManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Miscellaneous/stb_image.h"
/*
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image_write.h"
*/

EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN


CubemapHelper::CubemapHelper()
{
    Clear();
}

CubemapHelper::~CubemapHelper()
{
    Clear();
}

ionBool CubemapHelper::Load(const eosString& _path, ETextureFormat _format)
{
    m_format = _format;

    if (stbi_is_hdr(_path.c_str()))
    {
        m_buffer = stbi_loadf(_path.c_str(), &m_width, &m_height, &m_component, 0);
    }
    else
    {
        m_buffer = stbi_load(_path.c_str(), &m_width, &m_height, &m_component, 0);
    }
    
    m_numLevels = CalculateMipMapPerFace(m_width, m_height);

    return (m_buffer != nullptr);
}

void CubemapHelper::Unload()
{
    stbi_image_free(m_buffer);
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
        return CubemapFromCross();
    }
    else if (IsLatLong())
    {
        return CubemapFromLatLong();
    }
    else
    {
        return false;
    }
}

void CubemapHelper::CopyBufferRegion(const void* _source, void* _dest, ionU32 _sourceImageWidth, ionU32 _component, ionU32 _bpp, ionU32 _destSize, ionU32 _x, ionU32 _y)
{
    const ionU8* sourceFace = (const ionU8*)_source;
    ionU8* destFace = (ionU8*)_dest;
    ionU32 perChannel = _bpp / 32;
    for (ionU32 i = 0; i < _destSize; ++i)  // _destSize is the height but because we are in "cube map" width and height are the same
    {
        CopyBuffer(&destFace[i * _destSize * _component * perChannel], &sourceFace[(i + _y) * _sourceImageWidth * _component * perChannel + (_x * _component * perChannel)], _destSize * _component * perChannel);
    }
}

ionBool CubemapHelper::CubemapFromCross()
{
    const ionBool isVertical = (m_width / 3 == m_height / 4) && (m_width % 3 == 0) && (m_height % 4 == 0);
    const ionBool isHorizontal = !isVertical;

    if (isVertical)
    {
        m_sizePerFace = m_width / 3;
        m_numLevelsPerFace = CalculateMipMapPerFace(m_sizePerFace, m_sizePerFace);
    }
    else
    {
        m_sizePerFace = m_width / 4;
        m_numLevelsPerFace = CalculateMipMapPerFace(m_sizePerFace, m_sizePerFace);
    }

    return false;
}

ionBool CubemapHelper::CubemapFromLatLong()
{
    return false;
}


ION_NAMESPACE_END