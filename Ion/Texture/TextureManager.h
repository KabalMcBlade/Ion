#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "TextureCommon.h"
#include "TextureOptions.h"
#include "Texture.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderCore;

class ION_DLL TextureManager final
{
public:
    TextureManager();
    ~TextureManager();

    ION_NO_INLINE static TextureManager& Instance();

    void        Init(ETextureSamplesPerBit _textureSample, RenderCore& _renderCore);
    void        Shutdown();

    Texture*    CreateTextureFromOptions(VkDevice _vkDevice, const eosString& _name, const TextureOptions& _options);
    Texture*    GetTexture(const eosString& _name) const;

    const ETextureSamplesPerBit& GetMainSamplePerBits() const { return m_mainSamplesPerBit; }

private:
    Texture*    CreateTexture(VkDevice _vkDevice, const eosString& _name);
    void        DestroyTexture(Texture* _texture);
    void        DestroyTexture(ionSize _hash);

private:
    eosMap(ionSize, Texture*) m_hashTexture;

    RenderCore* m_RenderCorePtr;

    ETextureSamplesPerBit m_mainSamplesPerBit;
};

ION_NAMESPACE_END


#define ionTextureManger() TextureManager::Instance()