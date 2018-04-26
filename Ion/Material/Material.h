#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/RenderState.h"

ION_NAMESPACE_BEGIN

class Texture;

class ION_DLL Material
{
public:
    Material(const eosString& _name);
    ~Material();

    ionU64 GetStateBits() const { return m_stateBits; }
    void SetStateBits(ionU64 _stateBits) { m_stateBits = _stateBits; }

    void SetRasterizationMode(ERasterization _state) { m_stateBits |= _state; }
    void UnsetRasterizationMode(ERasterization _state) { m_stateBits &= ~_state; }

    void SetCullingMode(ECullingMode _state) { m_stateBits |= _state; }
    void UnsetCullingMode(ECullingMode _state) { m_stateBits &= ~_state; }

    void SetBlendStateMode(EBlendState _state) { m_stateBits |= _state; }
    void UnsetBlendStateMode(EBlendState _state) { m_stateBits &= ~_state; }

    void SetBlendOperatorMode(EBlendOperator _state) { m_stateBits |= _state; }
    void UnsetBlendOperatorMode(EBlendOperator _state) { m_stateBits &= ~_state; }

    void SetColorMaskMode(EColorMask _state) { m_stateBits |= _state; }
    void UnsetColorMaskMode(EColorMask _state) { m_stateBits &= ~_state; }
  
    void SetDepthFunctionMode(EDepthFunction _state) { m_stateBits |= _state; }
    void UnsetDepthFunctionMode(EDepthFunction _state) { m_stateBits &= ~_state; }

    void SetStencilFrontFunctionMode(EStencilFrontFunction _state) { m_stateBits |= _state; }
    void UnsetStencilFrontFunctionMode(EStencilFrontFunction _state) { m_stateBits &= ~_state; }

    void SetStencilFrontOperatorMode(EStencilFrontOperator _state) { m_stateBits |= _state; }
    void UnsetStencilFrontOperatorMode(EStencilFrontOperator _state) { m_stateBits &= ~_state; }

    void SetStencilBackFunctionMode(EStencilFrontFunction _state) { m_stateBits |= _state; }
    void UnsetStencilBackFunctionMode(EStencilFrontFunction _state) { m_stateBits &= ~_state; }

    void SetStencilBackOperatorMode(EStencilBackOperator _state) { m_stateBits |= _state; }
    void UnsetStencilBackOperatorMode(EStencilBackOperator _state) { m_stateBits &= ~_state; }

    void SetStencilFunctionReferenceMode(EStencilFunctionReference _state) { m_stateBits |= _state; }
    void UnsetStencilFunctionReferenceMode(EStencilFunctionReference _state) { m_stateBits &= ~_state; }

    ionBool Create(Texture* _albedoMap = nullptr, Texture* _normalMap = nullptr, Texture* _roughnessMap = nullptr, Texture* _metalnessMap = nullptr, Texture* _ambientOcclusionMap = nullptr, Texture* _emissiveMap = nullptr);
    void Destroy();


    ionFloat GetAlphaCutoff() const { return m_alphaCutoff; }
    ionFloat GetMetallicFactor() const { return m_metallicFactor; }
    ionFloat GetRoughnessFactor() const { return m_roughnessFactor; }

    void SetAlphaCutoff(ionFloat _value) { m_alphaCutoff = _value; }
    void SetMetallicFactor(ionFloat _value) { m_metallicFactor = _value; }
    void SetRoughnessFactor(ionFloat _value) { m_roughnessFactor = _value; }

    const Texture* GetAlbedoMap() const { return m_albedoMap; }
    const Texture* GetNormalMap() const { return m_normalMap; }
    const Texture* GetRoughnessMap() const { return m_roughnessMap; }
    const Texture* GetMetalnessMap() const { return m_metalnessMap; }
    const Texture* GetAmbientOcclusionMap() const { return m_ambientOcclusionMap; }
    const Texture* GetEmissiveMap() const { return m_emissiveMap; }

private:
    eosString       m_name;
    ionU64          m_stateBits;
    ionFloat        m_alphaCutoff;
    ionFloat        m_metallicFactor;
    ionFloat        m_roughnessFactor;
    Texture*        m_albedoMap;
    Texture*        m_normalMap;
    Texture*        m_roughnessMap;
    Texture*        m_metalnessMap;
    Texture*        m_ambientOcclusionMap;
    Texture*        m_emissiveMap;
};

ION_NAMESPACE_END