// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Texture\TextureManager.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "TextureManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Renderer/RenderCore.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

TextureManagerAllocator* TextureManager::GetAllocator()
{
	static HeapArea<Settings::kTextureManagerAllocatorSize> memoryArea;
	static TextureManagerAllocator memoryAllocator(memoryArea, "TextureManagerFreeListAllocator");

	return &memoryAllocator;
}

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

TextureManager& TextureManager::Instance()
{
    static TextureManager instance;
    return instance;
}

void TextureManager::Init(VkDevice _vkDevice, ETextureSamplesPerBit _textureSample)
{
    m_vkDevice = _vkDevice;
    m_mainSamplesPerBit = _textureSample;
}

void TextureManager::Shutdown()
{
    auto begin = m_hashTexture.begin(), end = m_hashTexture.end();
    ionMap<ionSize, Texture*, TextureManagerAllocator, GetAllocator>::iterator it = begin;
    for (; it != end; ++it)
    {
        DestroyTexture(it->second);
        ionDelete(it->second, Texture::GetAllocator());
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
    default:                            
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

Texture* TextureManager::CreateTextureFromFile(const ionString& _name, const ionString& _path, ETextureFilterMin _filterMin /*= ETextureFilterMin_Linear_MipMap_Linear*/, ETextureFilterMag _filterMag /*= ETextureFilterMag_Linear*/, ETextureRepeat _repeat /*= ETextureRepeat_Repeat*/, ETextureUsage _usage /*= ETextureUsage_RGBA*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _maxAnisotrpy /*= 1*/, ETextureRepeat _customRepeatU /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatV /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatW /*= ETextureRepeat_Repeat*/)
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
    texture->m_optFilterMin = _filterMin;
    texture->m_optFilterMag = _filterMag;
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

Texture* TextureManager::CreateTextureFromBuffer(const ionString& _name, ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer, VkDeviceSize _bufferSize, ETextureFilterMin _filterMin /*= ETextureFilterMin_Linear_MipMap_Linear*/, ETextureFilterMag _filterMag /*= ETextureFilterMag_Linear*/, ETextureRepeat _repeat /*= ETextureRepeat_Repeat*/, ETextureUsage _usage /*= ETextureUsage_RGBA*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _maxAnisotrpy /*= 1*/, ETextureRepeat _customRepeatU /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatV /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatW /*= ETextureRepeat_Repeat*/)
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
    texture->m_optFilterMin = _filterMin;
    texture->m_optFilterMag = _filterMag;
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

Texture* TextureManager::GenerateTexture(const ionString& _name, ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureFilterMin _filterMin /*= ETextureFilterMin_Linear_MipMap_Linear*/, ETextureFilterMag _filterMag /*= ETextureFilterMag_Linear*/, ETextureRepeat _repeat/*= ETextureRepeat_Repeat*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _numLevel /*= 1*/, ionU32 _maxAnisotrpy /*= 1*/, ETextureRepeat _customRepeatU /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatV /*= ETextureRepeat_Repeat*/, ETextureRepeat _customRepeatW /*= ETextureRepeat_Repeat*/)
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
    texture->m_optFilterMin = _filterMin;
    texture->m_optFilterMag = _filterMag;
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

Texture* TextureManager::GetTexture(const ionString& _name) const
{
    if (_name.empty())
    {
        return nullptr;
    }
    
    ionSize hash = std::hash<ionString>{}(_name);   // from the original with extension

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

ionBool TextureManager::SaveTexture(const ionString& _path, const Texture* _texture) const
{
    if (_path.empty() || _texture == nullptr)
    {
        return false;
    }

    return _texture->Save(_path);
}

void TextureManager::DestroyTexture(const ionString& _name)
{
    ionSize hash = std::hash<ionString>{}(_name);
    DestroyTexture(hash);
}

Texture* TextureManager::CreateTexture(VkDevice _vkDevice, const ionString& _name)
{
    if (_name.empty())
    {
        return nullptr;
    }

    ionSize hash = std::hash<ionString>{}(_name);

    // just to inform the user
    auto search = m_hashTexture.find(hash);
    ionAssertReturnValue(!(search != m_hashTexture.end()), "An image with the same name has already added!", nullptr);

    Texture* texture = ionNew(Texture, Texture::GetAllocator(), _vkDevice, _name);

    m_hashTexture[hash] = texture;

    return texture;
}

void TextureManager::DestroyTexture(ionSize _hash)
{
    auto search = m_hashTexture.find(_hash);
    if (search != m_hashTexture.end())
    {
        DestroyTexture(search->second);
        ionDelete(search->second, Texture::GetAllocator());
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

void TextureManager::GenerateMipMaps(Texture* _texture)
{
    if (_texture == nullptr)
    {
        return;
    }

    _texture->GenerateMipMaps();
}

ION_NAMESPACE_END