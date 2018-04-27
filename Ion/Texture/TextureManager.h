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
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static TextureManager& Instance();

    TextureManager();
    ~TextureManager();

    void        Init(ETextureSamplesPerBit _textureSample);
    void        Shutdown();

    

    Texture*    CreateTextureFromFile(VkDevice _vkDevice, const eosString& _name, const eosString& _path, ionS32 _index = -1, ETextureFilter _filter = ETextureFilter_Default, ETextureRepeat _repeat = ETextureRepeat_Clamp, ETextureUsage _usage = ETextureUsage_Default, ETextureType _type = ETextureType_2D);
    Texture*    CreateTextureFromBinary(VkDevice _vkDevice, const eosString& _name, ionU32 _width, ionU32 _height, ionU32 _numChannels, ionU8* _buffer, VkDeviceSize _bufferSize, ionS32 _index = -1, ETextureFilter _filter = ETextureFilter_Default, ETextureRepeat _repeat = ETextureRepeat_Clamp, ETextureUsage _usage = ETextureUsage_Default, ETextureType _type = ETextureType_2D);
    Texture*    CreateTextureFromOptions(VkDevice _vkDevice, const eosString& _name, const TextureOptions& _options, ionS32 _index = -1);
    Texture*    GetTexture(const eosString& _name) const;
    Texture*    GetTexture(ionS32 _index) const;

    const ETextureSamplesPerBit& GetMainSamplePerBits() const { return m_mainSamplesPerBit; }

private:
    Texture*    CreateTexture(VkDevice _vkDevice, const eosString& _name, ionS32 _index = -1);
    void        DestroyTexture(Texture* _texture);
    void        DestroyTexture(ionSize _hash);

private:
    eosMap(ionSize, Texture*) m_hashTexture;

    ETextureSamplesPerBit m_mainSamplesPerBit;

    static TextureManager *s_instance;
};

ION_NAMESPACE_END


#define ionTextureManger() TextureManager::Instance()