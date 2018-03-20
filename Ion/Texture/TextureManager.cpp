#include "TextureManager.h"

#include "../Renderer/RenderCore.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


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

void TextureManager::Init(ETextureSamplesPerBit _textureSample, RenderCore& _renderCore)
{
    m_RenderCorePtr = &_renderCore;
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

Texture* TextureManager::CreateTextureFromOptions(VkDevice _vkDevice, const eosString& _name, const TextureOptions& _options)
{
    if (_name.empty())
    {
        return nullptr;
    }

    Texture* texture = GetTexture(_name);
    if (texture == nullptr)
    {
        texture = CreateTexture(_vkDevice, _name);
    }
    else 
    {
        DestroyTexture(texture);
    }

    texture->SetOptions(_options);
    texture->Create();

    return texture;
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

    Texture* texture = eosNew(Texture, EOS_MEMORY_ALIGNMENT_SIZE, _vkDevice, _name);

    m_hashTexture[hash] = texture;

    return texture;
}

void TextureManager::DestroyTexture(ionSize _hash)
{
    auto search = m_hashTexture.find(_hash);
    if (search != m_hashTexture.end())
    {
        DestroyTexture(search->second);
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