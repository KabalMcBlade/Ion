#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "TextureCommon.h"
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

    void        Init(VkDevice _vkDevice, ETextureSamplesPerBit _textureSample);
    void        Shutdown();

    void        SetDepthFormat(VkFormat _depthFormat) { m_depthFormat = _depthFormat; }
    VkFormat    GetDepthFormat() const { return m_depthFormat; }

    void        SetSamplerAnysotropy(ionBool _samplerAnysotrpy) { m_samplerAnisotropy = _samplerAnysotrpy; }
    ionBool     GetSamplerAnysotropy() const { return m_samplerAnisotropy; }

    Texture*    CreateTextureFromFile(const eosString& _name, const eosString& _path, ETextureFilter _filter = ETextureFilter_Default, ETextureRepeat _repeat = ETextureRepeat_Clamp, ETextureUsage _usage = ETextureUsage_RGBA, ETextureType _type = ETextureType_2D, ionU32 _maxAnisotrpy = 1);
    Texture*    CreateTextureFromBuffer(const eosString& _name, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _buffer, VkDeviceSize _bufferSize, ETextureFilter _filter = ETextureFilter_Default, ETextureRepeat _repeat = ETextureRepeat_Clamp, ETextureUsage _usage = ETextureUsage_RGBA, ETextureType _type = ETextureType_2D, ionU32 _maxAnisotrpy = 1);
    Texture*    GenerateTexture(const eosString& _name, ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureFilter _filter = ETextureFilter_Default, ETextureRepeat _repeat = ETextureRepeat_Clamp, ETextureType _type = ETextureType_2D, ionU32 _numLevel = 1, ionU32 _maxAnisotrpy = 1);
    void        GenerateMipMaps(Texture* _texture);

    Texture*    GetTexture(const eosString& _name) const;

    ionBool     SaveTexture(const eosString& _path, const Texture* _texture) const;

    void        DestroyTexture(const eosString& _name);

    const ETextureSamplesPerBit& GetMainSamplePerBits() const { return m_mainSamplesPerBit; }

private:
    Texture*    CreateTexture(VkDevice _vkDevice, const eosString& _name);
    void        DestroyTexture(Texture* _texture);
    void        DestroyTexture(ionSize _hash);

private:
    VkDevice    m_vkDevice;
    eosMap(ionSize, Texture*) m_hashTexture;

    ETextureSamplesPerBit   m_mainSamplesPerBit;
    VkFormat                m_depthFormat;
    ionBool                 m_samplerAnisotropy;

    static TextureManager *s_instance;
};

ION_NAMESPACE_END


#define ionTextureManger() ion::TextureManager::Instance()