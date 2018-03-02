#pragma once

#include "../Core/CoreDefs.h"

#include "TextureCommon.h"


ION_NAMESPACE_BEGIN

struct TextureOptions final
{
    TextureOptions();
    ~TextureOptions();

    ionBool	operator==(const TextureOptions& _other);

    ETextureType		    m_textureType;
    ETextureSamplesPerBit	m_samples;
    ETextureFormat		    m_format;
    ETextureColor		    m_colorFormat;
    
    ionU32					m_width;
    ionU32					m_height;			// not needed for cube maps, actually.. it is a cube and so.. all same width :)
    ionU32					m_numLevels;		// if 0, will be 1 for NEAREST / LINEAR filters, otherwise based on size
    ionBool				    m_useGammaMips;		// if true, mipsmap will be generated with gamma correction
};


ION_NAMESPACE_END