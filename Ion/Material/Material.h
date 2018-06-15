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

    void SetCustomBits(ionU64 _stateBits) { m_stateBits |= _stateBits; }
    void UnsetCustomBits(ionU64 _stateBits) { m_stateBits &= ~_stateBits; }

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

    // actually do nothing... just to be conform with the texture manager
    ionBool Create();
    void Destroy();

    const ionFloat* GetColor() const { return &m_color[0]; }
    const ionFloat* GetEmissiveColor() const { return &m_emissiveColor[0]; }

    void SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a) {
        m_color[0] = _r; 
        m_color[1] = _g;
        m_color[2] = _b;
        m_color[3] = _a;
    }
    void SetEmissiveColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a) {
        m_emissiveColor[0] = _r;
        m_emissiveColor[1] = _g;
        m_emissiveColor[2] = _b;
        m_emissiveColor[3] = _a;
    }

    ionFloat GetAlphaCutoff() const { return m_alphaCutoff; }
    ionFloat GetMetallicFactor() const { return m_metallicFactor; }
    ionFloat GetRoughnessFactor() const { return m_roughnessFactor; }
    ionFloat GetNormalFactor() const { return m_normalFactor; }
    ionFloat GetOcclusionFactor() const { return m_occlusionFactor; }

    void SetAlphaCutoff(ionFloat _value) { m_alphaCutoff = _value; }
    void SetMetallicFactor(ionFloat _value) { m_metallicFactor = _value; }
    void SetRoughnessFactor(ionFloat _value) { m_roughnessFactor = _value; }
    void SetNormalFactor(ionFloat _value) { m_normalFactor = _value; }
    void SetOcclusionFactor(ionFloat _value) { m_occlusionFactor = _value; }

    const Texture* GetAlbedoMap() const { return m_albedoMap; }
    const Texture* GetNormalMap() const { return m_normalMap; }
    const Texture* GetRoughnessMap() const { return m_roughnessMap; }
    const Texture* GetMetalnessMap() const { return m_metalnessMap; }
    const Texture* GetOcclusionMap() const { return m_occlusionMap; }
    const Texture* GetEmissiveMap() const { return m_emissiveMap; }

    void SetAlbedoMap(Texture* _texture) { m_albedoMap = _texture; }
    void SetNormalMap(Texture* _texture) { m_normalMap = _texture; }
    void SetRoughnessMap(Texture* _texture) { m_roughnessMap = _texture; }
    void SetMetalnessMap(Texture* _texture) { m_metalnessMap = _texture; }
    void SetOcclusionMap(Texture* _texture) { m_occlusionMap = _texture; }
    void SetEmissiveMap(Texture* _texture) { m_emissiveMap = _texture; }

    void SetShaders(const ionS32 _vertexIndex, const ionS32 _fragmentIndex = -1, const ionS32 _tessellationControlIndex = -1, const ionS32 _tessellationEvaluationIndex = -1, const ionS32 _geometryIndex = -1, const ionBool _useJoint = false, const ionBool _useSkinning = false);
    void GetShaders(ionS32& _vertexIndex, ionS32& _fragmentIndex, ionS32& _tessellationControlIndex, ionS32& _tessellationEvaluationIndex, ionS32& _geometryIndex, ionBool& _useJoint, ionBool& _useSkinning) const;

    void SetVertexLayout(EVertexLayout _layout) { m_vertexLayout = _layout; }
    EVertexLayout GetVertexLayout() const { return m_vertexLayout; }

    void SetShaderProgramName(const eosString& _name) { m_shaderProgramName = _name; }
    const eosString& GetShaderProgramName() const { return m_shaderProgramName; }

private:
    eosString       m_name;
    eosString       m_shaderProgramName;

    ionU64          m_stateBits;

    ionS32          m_vertexShaderIndex;
    ionS32          m_fragmentShaderIndex;
    ionS32          m_tessellationControlIndex;
    ionS32          m_tessellationEvaluationIndex;
    ionS32          m_geometryIndex;

    EVertexLayout   m_vertexLayout;

    ionFloat        m_alphaCutoff;
    ionFloat        m_metallicFactor;
    ionFloat        m_roughnessFactor;
    ionFloat        m_normalFactor;
    ionFloat        m_occlusionFactor;

    Texture*        m_albedoMap;
    Texture*        m_normalMap;
    Texture*        m_roughnessMap;
    Texture*        m_metalnessMap;
    Texture*        m_occlusionMap;
    Texture*        m_emissiveMap;

    ionFloat        m_color[4];
    ionFloat        m_emissiveColor[4];

    ionBool         m_useJoint;
    ionBool         m_useSkinning;
};

ION_NAMESPACE_END