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

    /*
    if (m_component == 3)
    {
        m_conversionForced = true;

        ionU32 bppPerChannel = Texture::BitsPerFormat(m_format) / 32;

        ionU8* rgba = (ionU8*)eosNewRaw(m_width * m_height * 4 * bppPerChannel, ION_MEMORY_ALIGNMENT_SIZE);
        const ionU8* rgb = (const ionU8*)m_buffer;
        for (ionSize i = 0; i < m_width * m_height; ++i)
        {
            for (ionS32 j = 0; j < 3; ++j)
            {
                rgba[j] = rgb[j];
            }
            rgba += 4;
            rgb += 3;
        }



        stbi_image_free(m_buffer);
        m_buffer = nullptr;

        m_buffer = rgba;

        m_component = 4;
    }
    */
    
    m_numLevels = CalculateMipMapPerFace(m_width, m_height);

    return (m_buffer != nullptr);
}

void CubemapHelper::Unload()
{
    if (!m_conversionForced && m_buffer != nullptr)
    {
        stbi_image_free(m_buffer);
    }
    if (m_conversionForced && m_buffer != nullptr)
    {
        eosDeleteRaw(m_buffer);
    }
    
    for (ionU32 i = 0; i < 6; ++i)
    {
        if (m_output[i] != nullptr)
        {
            eosDeleteRaw(m_output[i]);
        }
    }
    Clear();
}

void CubemapHelper::Clear()
{
    m_conversionForced = false;
    m_width = 0;
    m_height = 0;
    m_component = 0;
    m_numLevels = 0;
    m_format = ETextureFormat_None;
    m_buffer = nullptr;
    m_sizePerFace = 0;
    m_bufferSizePerFace = 0;
    m_numLevelsPerFace = 0;
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
        CopyBuffer(&destFace[i * _destSize * _component * _bppPerChannel], &sourceFace[(i + _y) * _sourceImageWidth * _component * _bppPerChannel + (_x * _component * _bppPerChannel)], _destSize * _component * _bppPerChannel);
    }
}

void CubemapHelper::GenerateCubemapFromCrossVertical(const void* _source, void* _dest[6], ionU32 _bpp)
{
    const ionU32 perChannel = _bpp / 32;

    // right
    _dest[0] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[0], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace * 2, m_sizePerFace);

    // left
    _dest[1] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[1], m_width, m_component, perChannel, m_sizePerFace, 0, m_sizePerFace);

    // top
    _dest[2] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[2], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, 0);

    // bottom
    _dest[3] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[3], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace * 2);

    // front
    _dest[4] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[4], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace);

    // back
    _dest[5] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);

    // here I need to read from bottom to top and  right to left....
    const ionU8* sourceFace = (const ionU8*)_source;
    ionU8* destFace = (ionU8*)(_dest[5]);
    ionU32 _x = m_sizePerFace, _y = m_sizePerFace * 3;
    for (ionS32 i = (m_sizePerFace - 1), j = 0; i >= 0; --i, ++j)
    {
        CopyBuffer(&(destFace)[j * m_sizePerFace * m_component * perChannel], &sourceFace[(i + _y) * m_width * m_component * perChannel + (_x * m_component * perChannel)], m_sizePerFace * m_component * perChannel);
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
    _dest[0] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[0], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace * 2, m_sizePerFace);

    // left
    _dest[1] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[1], m_width, m_component, perChannel, m_sizePerFace, 0, m_sizePerFace);

    // top
    _dest[2] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[2], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, 0);

    // bottom
    _dest[3] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[3], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace * 2);

    // front
    _dest[4] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    CopyBufferRegion(_source, _dest[4], m_width, m_component, perChannel, m_sizePerFace, m_sizePerFace, m_sizePerFace);

    // back
    _dest[5] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
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

void CubemapHelper::GenerateFaceFromLatLong(const void* _source, void* _dest, ionU32 _bppPerChannel, ionU32 _faceIndex)
{
    const ionU8* sourceBuffer = (const ionU8*)_source;
    ionU8* destFace = (ionU8*)_dest;

    const ionFloat faceWidthSc = (ionFloat)(2.0f / m_sizePerFace);

    for (ionU32 i = 0; i < m_sizePerFace; ++i) 
    {
        for (ionU32 j = 0; j < m_sizePerFace; ++j)
        {
            ionFloat x = 0.0f;
            ionFloat y = 0.0f;
            ionFloat z = 0.0f;

            const ionFloat a = faceWidthSc * j;
            const ionFloat b = faceWidthSc * i;

            switch (_faceIndex)
            {
            case 0: x = 1.0f - a; y = 1.0f;     z = 1.0f - b; break; // right
            case 1: x = a - 1.0f; y = -1.0f;    z = 1.0f - b; break; // left
            case 2: x = b - 1.0f; y = a - 1.0f; z = 1.0f;     break; // top
            case 3: x = 1.0f - b; y = a - 1.0f; z = -1.0f;    break; // bottom
            case 4: x = 1.0f;     y = a - 1.0f; z = 1.0f - b; break; // front
            case 5: x = -1.0f;    y = 1.0f - a; z = 1.0f - b; break; // back
            }

            const ionFloat theta = std::atan2(y, x);
            const ionFloat rad = std::sqrt(x * x + y * y);
            const ionFloat phi = std::atan2(z, rad);

            const ionFloat uf = 2.0f * (m_width / 4) * (theta + MathHelper::kPI) / MathHelper::kPI;
            const ionFloat vf = 2.0f * (m_width / 4) * (MathHelper::kHalfPI - phi) / MathHelper::kPI;
            const ionU32 ui = (ionU32)std::floorf(uf);
            const ionU32 vi = (ionU32)std::floorf(vf);

            {
                const ionU32 destIndex = (j + i * m_sizePerFace) << 2;
                const ionU32 sourceIndex = (((ionU32)ui % m_width) + m_width * Clamp(vi, 0, m_height - 1)) << 2;

                //destFace[destIndex + 0] = sourceBuffer[sourceIndex + 0];
                //destFace[destIndex + 1] = sourceFace[sourceIndex + 1];
                //destFace[destIndex + 2] = sourceBuffer[sourceIndex + 2];
                //destFace[destIndex + 3] = sourceBuffer[sourceIndex + 3];
                //CopyBuffer(&destFace[destIndex], &sourceBuffer[sourceIndex], m_component * _bppPerChannel);
                //CopyBuffer(destFace + destIndex, sourceBuffer, sourceIndex * m_component * _bppPerChannel);
                //memcpy(destFace + destIndex, sourceBuffer, sourceIndex * m_component * _bppPerChannel);
            }
        }
    }
}

void CubemapHelper::GenerateCubemapFromLatLong(const void* _source, void* _dest[6], ionU32 _bpp)
{
    const ionU32 perChannel = _bpp / 32;

    _dest[0] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, 1);
    _dest[1] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, 1);
    _dest[2] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, 1);
    _dest[3] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, 1);
    _dest[4] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, 1);
    _dest[5] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, 1);

    for (ionU32 i = 0; i < 6; ++i)
    {
        GenerateFaceFromLatLong(_source, _dest[i], perChannel, i);
    }
}

void CubemapHelper::CubemapFromLatLong()
{
    m_sizePerFace = 1 << std::lround(std::log(m_width / 4) / std::log(2));
    m_numLevelsPerFace = CalculateMipMapPerFace(m_sizePerFace, m_sizePerFace);
    GenerateCubemapFromLatLong(m_buffer, m_output, Texture::BitsPerFormat(m_format));
}


ION_NAMESPACE_END