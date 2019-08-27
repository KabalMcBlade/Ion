#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemoryWrapper.h"

#include "TextureCommon.h"
#include "Texture.h"


#define ION_NULL_TEXTURENAME                  "NULLTEXTURE"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderCore;

class ION_DLL TextureManager final
{
public:
    static TextureManager& Instance();

    TextureManager();
    ~TextureManager();

    void        Init(VkDevice _vkDevice, ETextureSamplesPerBit _textureSample);
    void        Shutdown();

    void        SetDepthFormat(VkFormat _depthFormat) { m_depthFormat = _depthFormat; }
    VkFormat    GetDepthFormat() const { return m_depthFormat; }

    void        SetSamplerAnysotropy(ionBool _samplerAnysotrpy) { m_samplerAnisotropy = _samplerAnysotrpy; }
    ionBool     GetSamplerAnysotropy() const { return m_samplerAnisotropy; }

    Texture*    CreateTextureFromFile(const ionString& _name, const ionString& _path, ETextureFilterMin _filterMin = ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag _filterMag = ETextureFilterMag_Linear, ETextureRepeat _repeat = ETextureRepeat_Repeat, ETextureUsage _usage = ETextureUsage_RGBA, ETextureType _type = ETextureType_2D, ionU32 _maxAnisotrpy = 1, ETextureRepeat _customRepeatU = ETextureRepeat_Repeat, ETextureRepeat _customRepeatV = ETextureRepeat_Repeat, ETextureRepeat _customRepeatW = ETextureRepeat_Repeat);
    Texture*    CreateTextureFromBuffer(const ionString& _name, ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer, VkDeviceSize _bufferSize, ETextureFilterMin _filterMin = ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag _filterMag = ETextureFilterMag_Linear, ETextureRepeat _repeat = ETextureRepeat_Repeat, ETextureUsage _usage = ETextureUsage_RGBA, ETextureType _type = ETextureType_2D, ionU32 _maxAnisotrpy = 1, ETextureRepeat _customRepeatU = ETextureRepeat_Repeat, ETextureRepeat _customRepeatV = ETextureRepeat_Repeat, ETextureRepeat _customRepeatW = ETextureRepeat_Repeat);
    Texture*    GenerateTexture(const ionString& _name, ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureFilterMin _filterMin = ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag _filterMag = ETextureFilterMag_Linear, ETextureRepeat _repeat = ETextureRepeat_Repeat, ETextureType _type = ETextureType_2D, ionU32 _numLevel = 1, ionU32 _maxAnisotrpy = 1, ETextureRepeat _customRepeatU = ETextureRepeat_Repeat, ETextureRepeat _customRepeatV = ETextureRepeat_Repeat, ETextureRepeat _customRepeatW = ETextureRepeat_Repeat);

    Texture*    GetTexture(const ionString& _name) const;

    ionBool     SaveTexture(const ionString& _path, const Texture* _texture) const;

    void        GenerateMipMaps(Texture* _texture);

    // this one actually destroy/delete the texture!
    void        DestroyTexture(const ionString& _name);

    const ETextureSamplesPerBit& GetMainSamplePerBits() const { return m_mainSamplesPerBit; }

private:
    VkSamplerAddressMode ConvertAddressMode(ETextureRepeat _repeat);

    Texture*    CreateTexture(VkDevice _vkDevice, const ionString& _name);
    void        DestroyTexture(Texture* _texture);
    void        DestroyTexture(ionSize _hash);          // this one actually destroy/delete the texture!

private:
    VkDevice    m_vkDevice;
    ionMap<ionSize, Texture*> m_hashTexture;

    ETextureSamplesPerBit   m_mainSamplesPerBit;
    VkFormat                m_depthFormat;
    ionBool                 m_samplerAnisotropy;
};

ION_NAMESPACE_END


#define ionTextureManger() ion::TextureManager::Instance()