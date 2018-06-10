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
    }
    m_hashTexture.clear();
}

Texture* TextureManager::CreateTextureFromFile(const eosString& _name, const eosString& _path, ETextureFilter _filter /*= ETextureFilter_Default*/, ETextureRepeat _repeat /*= ETextureRepeat_Clamp*/, ETextureUsage _usage /*= ETextureUsage_RGBA*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _maxAnisotrpy /*= 1*/)
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

    texture->m_sampleCount = m_sampleCount;
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

Texture* TextureManager::CreateTextureFromBuffer(const eosString& _name, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _buffer, VkDeviceSize _bufferSize, ETextureFilter _filter /*= ETextureFilter_Default*/, ETextureRepeat _repeat /*= ETextureRepeat_Clamp*/, ETextureUsage _usage /*= ETextureUsage_RGBA*/, ETextureType _type /*= ETextureType_2D*/, ionU32 _maxAnisotrpy /*= 1*/)
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

    texture->m_sampleCount = m_sampleCount;
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

void TextureManager::DestroyTexture(const eosString& _name)
{
	ionSize hash = std::hash<eosString>{}(_name);   // from the original with extension
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

		m_hashTexture.erase(_hash);
    }
}

void TextureManager::DestroyTexture(Texture* _texture)
{
    if (_texture != nullptr)
    {
        _texture->Destroy();
        eosDelete(_texture);
    }
}

ION_NAMESPACE_END