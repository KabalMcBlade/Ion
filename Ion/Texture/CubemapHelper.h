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

    ionBool ConvertToTexture(Texture* _output);

private:
    void Clear();

    ionBool IsLatLong();
    ionBool IsHStrip();
    ionBool IsVStrip();
    ionBool IsCubeCross();

    ionBool CubemapFromCross(Texture* _output);
    ionBool CubemapFromLatLong(Texture* _output);
    ionBool CubemapFromStrip(Texture* _output);

private:
    ionS32 m_width;
    ionS32 m_height;
    ionS32 m_component;
    ionU32 m_size;
    ionU32 m_numLevels;
    ETextureFormat m_format;
    void* m_buffer;
    ionU8* m_originalBufferBytes;
    ionU16* m_originalBufferShorts;
    ionFloat* m_originalBufferFloats;
};


ION_NAMESPACE_END