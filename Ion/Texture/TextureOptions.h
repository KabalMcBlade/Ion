#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "TextureCommon.h"


ION_NAMESPACE_BEGIN

struct TextureOptions final
{
    TextureOptions();
    ~TextureOptions();

    ionBool	operator==(const TextureOptions& _other);

    VkFormat                m_vkDepthFormat;

    ETextureType		    m_textureType;
    ETextureSamplesPerBit	m_samples;
    ETextureFormat		    m_format;
    ETextureColor		    m_colorFormat;
    
    ionU32					m_width;
    ionU32					m_height;			// not needed for cube maps, actually.. it is a cube and so.. all same width :)
    ionU32					m_numLevels;		// if 0, will be 1 for NEAREST / LINEAR filters, otherwise based on size
    ionU32                  m_numChannels;      // channel amount in the texture file (only valid for file!)
    ionBool				    m_useGammaMips;		// if true, mipsmap will be generated with gamma correction
};


ION_NAMESPACE_END