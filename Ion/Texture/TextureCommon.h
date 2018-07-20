#pragma once

#include "../Core/CoreDefs.h"


ION_NAMESPACE_BEGIN

enum ETextureType
{
    ETextureType_None   = -1,
    ETextureType_2D,
    ETextureType_Cubic
};

enum ETextureSamplesPerBit
{
    ETextureSamplesPerBit_1  = 0b00000001,
    ETextureSamplesPerBit_2  = 0b00000010,
    ETextureSamplesPerBit_4  = 0b00000100,
    ETextureSamplesPerBit_8  = 0b00001000,
    ETextureSamplesPerBit_16 = 0b00010000,
    ETextureSamplesPerBit_32 = 0b00100000,
    ETextureSamplesPerBit_64 = 0b01000000
};

enum ETextureFormat
{
    ETextureFormat_None = -1,

    ETextureFormat_RGBA8,   // 32 bpp
    ETextureFormat_XRGB8,   // 32 bpp
    ETextureFormat_RGB565,  // 16 bpp
    ETextureFormat_HDR,     // RGBE, floating point, 48 bpp (3 x 16 bits)

    ETextureFormat_Depth,   // 24 bpp

    // alpha channel only
    ETextureFormat_Alpha,

    // Luminance give value across RGB with a constant A of 255
    // Intensity give value across RGBA
    ETextureFormat_Luminance8Alpha8,    // 16 bpp
    ETextureFormat_Luminance8,            //  8 bpp
    ETextureFormat_Intensity8,            //  8 bpp

    ETextureFormat_BRDF,             // 32 bpp, 16 red and 16 green, used for PBR
    ETextureFormat_RenderTexture    // 32 bpp ready to write
};

 enum ETextureUsage
 {
    ETextureUsage_Light,            // Light image
    ETextureUsage_Mono,                // Mono used for falloff for instance
    ETextureUsage_Alpha,            // Alpha with a white color channel
    ETextureUsage_RGB1,                // RGB with Alpha to 1
    ETextureUsage_RGBA,                // RGBA
    ETextureUsage_RGB,                // RGB
    ETextureUsage_Depth,                // Depth
    ETextureUsage_Skybox                // skybox hdr
};

 enum ETextureFilter
 {
     ETextureFilter_Linear,
     ETextureFilter_Nearest,    
     ETextureFilter_Default         // Using this filtering, the TextureOptions::m_numLevels will be computed by width and height, and the filtering will be set as ETextureFilter_Linear
 };

 enum ETextureRepeat
 {
     ETextureRepeat_Repeat,
     ETextureRepeat_Clamp,
     ETextureRepeat_ClampZero,    // guarantee 0,0,0,255 edge for projected textures
     ETextureRepeat_ClampAlpha    // guarantee 0 alpha edge for projected textures
 };

ION_NAMESPACE_END
