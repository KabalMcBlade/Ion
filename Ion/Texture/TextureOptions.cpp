#include "TextureOptions.h"

ION_NAMESPACE_BEGIN


TextureOptions::TextureOptions() :
    m_textureType(ETextureType_2D),
    m_samples(ETextureSamplesPerBit_1),
    m_format(ETextureFormat_None),
    m_colorFormat(ETextureColor_Default),
    m_width(0),
    m_height(0),
    m_numLevels(0),
    m_numChannels(0),
    m_useGammaMips(false)
{
}

TextureOptions:: ~TextureOptions()
{

}

ionBool	TextureOptions::operator==(const TextureOptions& _other)
{
    return (std::memcmp(this, &_other, sizeof(*this)) == 0);
}


ION_NAMESPACE_END