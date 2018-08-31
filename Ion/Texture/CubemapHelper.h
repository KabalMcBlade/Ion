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

    ionU32 GetSizePerFace() const { return m_bufferSizePerFace; }
    ETextureFormat GetFormat() const { return m_format; }       // redundant but for completeness

private:
    void Clear();

    void CopyBufferRegion(const void* _source, void* _dest, ionU32 _sourceImageWidth, ionU32 _component, ionU32 _bpp, ionU32 _destSize, ionU32 _x, ionU32 _y);
    ionU32 CalculateMipMapPerFace(ionU32 _width, ionU32 _height);

    ionBool IsLatLong();
    ionBool IsCubeCross();

    ionBool CubemapFromCross();
    ionBool CubemapFromLatLong();

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
};


ION_NAMESPACE_END