#include "TextureManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Renderer/RenderCore.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


TextureManager *TextureManager::s_instance = nullptr;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

void TextureManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(TextureManager, ION_MEMORY_ALIGNMENT_SIZE);
    }
}

void TextureManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

TextureManager& TextureManager::Instance()
{
    return *s_instance;
}

void TextureManager::Init(VkDevice _vkDevice, ETextureSamplesPerBit _textureSample)
{
    m_vkDevice = _vkDevice;
    m_mainSamplesPerBit = _textureSample;
}

void TextureManager::Shutdown()
{
    auto begin = m_hashTexture.begin(), end = m_hashTexture.end();
    std::map<ionSize, Texture*, StlAllocator<ionSize, HeapAllocPolicy<ionSize> > >::iterator it = begin;
    for (; it != end; ++it)
    {
        DestroyTexture(it->second);
        eosDelete(it->second);
    }

    m_hashTexture.clear();
}

VkSamplerAddressMode TextureManager::ConvertAddressMode(ETextureRepeat _repeat)
{
    switch (_repeat)
    {
    case ETextureRepeat_Repeat:         return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case ETextureRepeat_Clamp:          return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case ETextureRepeat_ClampZero:      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case ETextureRepeat_ClampAlpha:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case ETextureRepeat_Mirrored:       return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case ETextureRepeat_MirroredClamp:  return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    case ETextureRepeat_Custom:
    case ETextureRepeat_NoSampler:
    default:                            
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

Texture* TextureManager::CreateTextureFromFile(const eosString& _name, const eosString& _path, ETextureFilter _filter /*= ETextureFilter_Default*/, ETextureRepeat _repeat /*= ETextureRepeat_Repeat*/, ETextureUsage _usage /*= ETextureUsage_RGBA*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _maxAnisotrpy /*= 1*/, ETextureRepeat _customRepeatU /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatV /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatW /*= ETextureRepeat_Repeat*/)
{
    if (_name.empty() || _path.empty())
    {
        return nullptr;
    }

    Texture* texture = GetTexture(_name);
    if (texture == nullptr)
    {
        texture = CreateTexture(m_vkDevice, _name);
    }
    else
    {
        DestroyTexture(texture);
    }

    texture->m_optUsage = _usage;
    texture->m_optFilter = _filter;
    texture->m_optRepeat = _repeat;
    texture->m_optTextureType = _type;
    texture->m_optCustomRepeat[0] = ConvertAddressMode(_customRepeatU);
    texture->m_optCustomRepeat[1] = ConvertAddressMode(_customRepeatV);
    texture->m_optCustomRepeat[2] = ConvertAddressMode(_customRepeatW);
    texture->m_maxAnisotropy = _maxAnisotrpy;

    if (texture->CreateFromFile(_path))
    {
        return texture;
    }
    else
    {
        return nullptr;
    }
}

Texture* TextureManager::CreateTextureFromBuffer(const eosString& _name, ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer, VkDeviceSize _bufferSize, ETextureFilter _filter /*= ETextureFilter_Default*/, ETextureRepeat _repeat /*= ETextureRepeat_Repeat*/, ETextureUsage _usage /*= ETextureUsage_RGBA*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _maxAnisotrpy /*= 1*/, ETextureRepeat _customRepeatU /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatV /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatW /*= ETextureRepeat_Repeat*/)
{
    if (_name.empty())
    {
        return nullptr;
    }

    Texture* texture = GetTexture(_name);
    if (texture == nullptr)
    {
        texture = CreateTexture(m_vkDevice, _name);
    }
    else
    {
        DestroyTexture(texture);
    }

    texture->m_optUsage = _usage;
    texture->m_optFilter = _filter;
    texture->m_optRepeat = _repeat;
    texture->m_optTextureType = _type;
    texture->m_optCustomRepeat[0] = ConvertAddressMode(_customRepeatU);
    texture->m_optCustomRepeat[1] = ConvertAddressMode(_customRepeatV);
    texture->m_optCustomRepeat[2] = ConvertAddressMode(_customRepeatW);
    texture->m_maxAnisotropy = _maxAnisotrpy;

    if (texture->CreateFromBuffer(_width, _height, _component, _buffer, _bufferSize))
    {
        return texture;
    }
    else
    {
        return nullptr;
    }
}

Texture* TextureManager::GenerateTexture(const eosString& _name, ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureFilter _filter /*= ETextureFilter_Default*/, ETextureRepeat _repeat/*= ETextureRepeat_Repeat*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _numLevel /*= 1*/, ionU32 _maxAnisotrpy /*= 1*/, ETextureRepeat _customRepeatU /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatV /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatW /*= ETextureRepeat_Repeat*/)
{
    if (_name.empty())
    {
        return nullptr;
    }

    Texture* texture = GetTexture(_name);
    if (texture == nullptr)
    {
        texture = CreateTexture(m_vkDevice, _name);
    }
    else
    {
        DestroyTexture(texture);
    }

    texture->m_maxAnisotropy = _maxAnisotrpy;
    texture->m_optFilter = _filter;
    texture->m_optCustomRepeat[0] = ConvertAddressMode(_customRepeatU);
    texture->m_optCustomRepeat[1] = ConvertAddressMode(_customRepeatV);
    texture->m_optCustomRepeat[2] = ConvertAddressMode(_customRepeatW);
    if (texture->GenerateTexture(_width, _height, _format, _repeat, _type, _numLevel))
    {
        return texture;
    }
    else
    {
        return nullptr;
    }
}

Texture* TextureManager::GetTexture(const eosString& _name) const
{
    if (_name.empty())
    {
        return nullptr;
    }
    
    ionSize hash = std::hash<eosString>{}(_name);   // from the original with extension

    auto search = m_hashTexture.find(hash);
    if (search != m_hashTexture.end())
    {
        return search->second;
    }
    else 
    {
        return nullptr;
    }
}

ionBool TextureManager::SaveTexture(const eosString& _path, const Texture* _texture) const
{
    if (_path.empty() || _texture == nullptr)
    {
        return false;
    }

    return _texture->Save(_path);
}

void TextureManager::DestroyTexture(const eosString& _name)
{
    ionSize hash = std::hash<eosString>{}(_name);
    DestroyTexture(hash);
}

Texture* TextureManager::CreateTexture(VkDevice _vkDevice, const eosString& _name)
{
    if (_name.empty())
    {
        return nullptr;
    }

    ionSize hash = std::hash<eosString>{}(_name);

    // just to inform the user
    auto search = m_hashTexture.find(hash);
    ionAssert(!(search != m_hashTexture.end()), "An image with the same name has already added!");

    Texture* texture = eosNew(Texture, ION_MEMORY_ALIGNMENT_SIZE, _vkDevice, _name);

    m_hashTexture[hash] = texture;

    return texture;
}

void TextureManager::DestroyTexture(ionSize _hash)
{
    auto search = m_hashTexture.find(_hash);
    if (search != m_hashTexture.end())
    {
        DestroyTexture(search->second);
        eosDelete(search->second);
        m_hashTexture.erase(_hash);
    }
}

void TextureManager::DestroyTexture(Texture* _texture)
{
    if (_texture != nullptr)
    {
        _texture->Destroy();
    }
}

ION_NAMESPACE_END