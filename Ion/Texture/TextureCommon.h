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

    // alpha channel only
    ETextureFormat_Alpha,

    // Luminance give value across RGB with a constant A of 255
    // Intensity give value across RGBA
    ETextureFormat_Luminance8Alpha8,	// 16 bpp
    ETextureFormat_Luminance8,		    //  8 bpp
    ETextureFormat_Intensity8,		    //  8 bpp

    // compress format
    ETextureFormat_DXT1,	// 4 bpp
    ETextureFormat_DXT5,	// 8 bpp

    // depth buffer
    ETextureFormat_Depth    // 24 bpp
};

enum ETextureColor
{
    ETextureColor_Default   = 0,	// RGBA
    ETextureColor_Normal_DXT5,		// XY format and use the fast DXT5 compressor
    ETextureColor_Green_To_Aalpha	// Copy the alpha channel to green
};

 enum ETextureUsage
 {
    ETextureUsage_Specular,			// maybe compressed, and always zeros the alpha channel
    ETextureUsage_Diffuse,			// maybe compressed
    ETextureUsage_Default,			// RGBA texture
    ETextureUsage_Bump,				// maybe compressed with 8 bit lookup
    ETextureUsage_Font,				// Font image
    ETextureUsage_Light,			// Light image
    ETextureUsage_LookUp_Mono,	    // Mono lookup table (including alpha) used for falloff for instance
    ETextureUsage_LookUp_Alpha,	    // Alpha lookup table with a white color channel
    ETextureUsage_LookUp_RGB1,	    // RGB lookup table with a solid white alpha
    ETextureUsage_LookUp_RGBA,	    // RGBA lookup table
    ETextureUsage_Coverage,			// coverage map for fill depth pass when YCoCG is used
    ETextureUsage_Depth,			// depth buffer copy for motion blur
};

 enum ETextureFilter
 {
     ETextureFilter_Linear,
     ETextureFilter_Nearest,
     ETextureFilter_Default
 };

 enum ETextureRepeat
 {
     ETextureRepeat_Repeat,
     ETextureRepeat_Clamp,
     ETextureRepeat_ClampZero,	// guarantee 0,0,0,255 edge for projected textures
     ETextureRepeat_ClampAlpha	// guarantee 0 alpha edge for projected textures
 };

ION_NAMESPACE_END
