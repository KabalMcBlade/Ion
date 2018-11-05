#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "TextureCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Texture;
class CubemapHelper final
{
public:
    CubemapHelper();
    ~CubemapHelper();

    ionBool Load(const eosString& _path, ETextureFormat _format);
    void    Unload();

    ionBool Convert();

    ionS32 GetComponent() const { return m_component; }
    ionU32 GetBufferSizePerFace() const { return m_bufferSizePerFace; }
    ETextureFormat GetFormat() const { return m_format; }       // redundant but for completeness

    ionU32 GetSizePerFace() const { return m_sizePerFace; }
    ionU32 GetNumLevelPerFace() const { return m_numLevelsPerFace; }
    const void* GetBufferPerFace(ionU32 _index) const { return m_output[_index]; }
    ionBool IsHDR() const { return m_isHDR; }

private:
    void Clear();

    void CopyBufferRegion(const void* _source, void* _dest, ionU32 _sourceImageWidth, ionU32 _component, ionU32 _bppPerChannel, ionU32 _destSize, ionU32 _x, ionU32 _y);
    ionU32 CalculateMipMapPerFace(ionU32 _width, ionU32 _height);

    // The following 4 functions contains 6 allocation and for each one the mem copy, reading from the original buffer and writing one face at time
    // in other words, when I'll add multithreading, let work in parallel! 1 thread for each face!
    void GenerateCubemapFromCrossVertical(const void* _source, void* _dest[6], ionU32 _bpp);
    void GenerateCubemapFromCrossHorizontal(const void* _source, void* _dest[6], ionU32 _bpp);
    template<typename T>
    void GenerateCubemapFromLatLong(const void* _source, void* _dest[6], ionU32 _bpp);
    template<typename T>
    void GenerateFaceFromLatLong(const void* _source, void* _dest, ionU32 _bppPerChannel, ionU32 _faceIndex);
   
    ionS32 Clamp(ionS32 _n, ionS32 _lower, ionS32 _upper);

    ionBool IsLatLong();
    ionBool IsCubeCross();

    void CubemapFromCross();
    void CubemapFromLatLong();

private:
    ionS32 m_width;
    ionS32 m_height;
    ionS32 m_component;
    ionU32 m_numLevels;     // may not have sense :D
    ETextureFormat m_format;
    void* m_buffer;

    // output
    ionU32 m_sizePerFace;
    ionU32 m_bufferSizePerFace;
    ionU32 m_numLevelsPerFace;
    void* m_output[6];
    ionBool m_isHDR;
};


template<typename T>
void CubemapHelper::GenerateFaceFromLatLong(const void* _source, void* _dest, ionU32 _bppPerChannel, ionU32 _faceIndex)
{
    const T* sourceBuffer = (const T*)_source;
    T* destFace = (T*)_dest;

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

            const ionFloat uf = 2.0f * (m_width / 4) * (theta + kfPI) / kfPI;
            const ionFloat vf = 2.0f * (m_width / 4) * (kfHalfPI - phi) / kfPI;
            const ionU32 ui = (ionU32)std::floorf(uf);
            const ionU32 vi = (ionU32)std::floorf(vf);

            {
                const ionU32 destIndex = (j + i * m_sizePerFace) << 2;
                const ionU32 sourceIndex = (((ionU32)ui % m_width) + m_width * Clamp(vi, 0, m_height - 1)) << 2;

                destFace[destIndex + 0] = sourceBuffer[sourceIndex + 0];
                destFace[destIndex + 1] = sourceBuffer[sourceIndex + 1];
                destFace[destIndex + 2] = sourceBuffer[sourceIndex + 2];
                destFace[destIndex + 3] = sourceBuffer[sourceIndex + 3];
            }
        }
    }
}

template<typename T>
void CubemapHelper::GenerateCubemapFromLatLong(const void* _source, void* _dest[6], ionU32 _bpp)
{
    const ionU32 perChannel = _bpp / 32;

    _dest[0] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    _dest[1] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    _dest[2] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    _dest[3] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    _dest[4] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);
    _dest[5] = eosNewRaw(m_sizePerFace * m_sizePerFace * m_component * perChannel, ION_MEMORY_ALIGNMENT_SIZE);

    for (ionU32 i = 0; i < 6; ++i)
    {
        GenerateFaceFromLatLong<T>(_source, _dest[i], perChannel, i);
    }
}


ION_NAMESPACE_END