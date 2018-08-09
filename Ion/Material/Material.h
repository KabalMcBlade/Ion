#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"


#include "MaterialState.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/RenderState.h"

#include "../Shader/ShaderProgram.h"

ION_NAMESPACE_BEGIN

class Texture;

//////////////////////////////////////////////////////////////////////////

class ION_DLL BasePBR
{
public:
    BasePBR();
    ~BasePBR();

    const Texture* GetBaseColorTexture() const { return m_baseColorTexture; }
    const Texture* GetMetalRoughnessTexture() const { return m_metalRoughness; }

    void SetBaseColorTexture(const Texture* _texture) { m_baseColorTexture = _texture; }
    void SetMetalRoughnessTexture(const Texture* _texture) { m_metalRoughness = _texture; }

    void GetColor(ionFloat& _r, ionFloat& _g, ionFloat& _b, ionFloat& _a) const 
    {
        _r = m_baseColor[0];
        _g = m_baseColor[1];
        _b = m_baseColor[2];
        _a = m_baseColor[3];
    }
    const ionFloat* GetColor() const { return &m_baseColor[0]; }
    const ionFloat GetMetallicFactor() const { return m_metallicFactor; }
    const ionFloat GetRoughnessFactor() const { return m_roughnessFactor; }

    void SetBaseColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a) 
    {
        m_baseColor[0] = _r;
        m_baseColor[1] = _g;
        m_baseColor[2] = _b;
        m_baseColor[3] = _a;
    }
    void SetMetallicFactor(ionFloat _value) { m_metallicFactor = _value; }
    void SetRoughnessFactor(ionFloat _value) { m_roughnessFactor = _value; }

private:
    const Texture*    m_baseColorTexture;
    const Texture*    m_metalRoughness;

    ionFloat    m_baseColor[4];
    ionFloat    m_metallicFactor;
    ionFloat    m_roughnessFactor;
};

//////////////////////////////////////////////////////////////////////////

class ION_DLL AdvancePBR
{
public:
    AdvancePBR();
    ~AdvancePBR();

    const Texture* GetNormalTexture() const { return m_normalTexture; }
    const Texture* GetOcclusionTexture() const { return m_occlusionTexture; }
    const Texture* GetEmissiveTexture() const { return m_emissiveTexture; }

    void SetNormalTexture(const Texture* _texture) { m_normalTexture = _texture; }
    void SetOcclusionTexture(const Texture* _texture) { m_occlusionTexture = _texture; }
    void SetEmissiveTexture(const Texture* _texture) { m_emissiveTexture = _texture; }

    const ionFloat GetAlphaCutoff() const { return m_alphaCutoff; }
    void GetEmissiveColor(ionFloat& _r, ionFloat& _g, ionFloat& _b) const
    {
        _r = m_emissiveColor[0];
        _g = m_emissiveColor[1];
        _b = m_emissiveColor[2];
    }
    const ionFloat* GetEmissiveColor() const { return &m_emissiveColor[0]; }

    void SetAlphaCutoff(ionFloat _value) { m_alphaCutoff = _value; }
    void SetEmissiveColor(ionFloat _r, ionFloat _g, ionFloat _b)
    {
        m_emissiveColor[0] = _r;
        m_emissiveColor[1] = _g;
        m_emissiveColor[2] = _b;
    }


private:
    const Texture*    m_normalTexture;
    const Texture*    m_occlusionTexture;
    const Texture*    m_emissiveTexture;

    ionFloat    m_emissiveColor[3];
    ionFloat    m_alphaCutoff;
};

//////////////////////////////////////////////////////////////////////////

enum EAlphaMode
{
    EAlphaMode_Opaque = 0,
    EAlphaMode_Blend,
    EAlphaMode_Mask
};

class ION_DLL Material
{
public:
    Material(const eosString& _name);
    ~Material();

    ionBool Create();
    void Destroy();

    MaterialState& GetState() { return m_state; }
    const MaterialState& GetState() const { return m_state; }

    BasePBR& GetBasePBR() { return m_basePBR; }
    const BasePBR& GetBasePBR() const { return m_basePBR; }

    AdvancePBR& GetAdvancePBR() { return m_advancePBR; }
    const AdvancePBR& GetAdvancePBR() const { return m_advancePBR; }

    void SetAlphaMode(EAlphaMode _mode) { m_alphaMode = _mode; }
    EAlphaMode GetAlphaMode() { return m_alphaMode; }

    void SetShaders(const ionS32 _vertexIndex, const ionS32 _fragmentIndex = -1, const ionS32 _tessellationControlIndex = -1, const ionS32 _tessellationEvaluationIndex = -1, const ionS32 _geometryIndex = -1, const ionBool _useJoint = false, const ionBool _useSkinning = false);
    void GetShaders(ionS32& _vertexIndex, ionS32& _fragmentIndex, ionS32& _tessellationControlIndex, ionS32& _tessellationEvaluationIndex, ionS32& _geometryIndex, ionBool& _useJoint, ionBool& _useSkinning) const;

    void SetVertexLayout(EVertexLayout _layout) { m_vertexLayout = _layout; }
    EVertexLayout GetVertexLayout() const { return m_vertexLayout; }

    void SetShaderProgramName(const eosString& _name) { m_shaderProgramName = _name; }
    const eosString& GetShaderProgramName() const { return m_shaderProgramName; }

    void SetConstantsShaders(const ConstantsBindingDef& _constants) { m_constants = _constants; m_constants.m_runtimeStages = (VkShaderStageFlagBits)m_constants.m_shaderStages; }
    const ConstantsBindingDef& GetConstantsShaders() const { return m_constants; }
    ConstantsBindingDef& GetConstantsShaders() { return m_constants; }

private:
    eosString       m_name;
    eosString       m_shaderProgramName;

    BasePBR         m_basePBR;
    AdvancePBR      m_advancePBR;

    ConstantsBindingDef m_constants;

    EVertexLayout   m_vertexLayout;
    EAlphaMode      m_alphaMode;

    MaterialState   m_state;

    ionS32          m_vertexShaderIndex;
    ionS32          m_fragmentShaderIndex;
    ionS32          m_tessellationControlIndex;
    ionS32          m_tessellationEvaluationIndex;
    ionS32          m_geometryIndex;

    ionBool         m_useJoint;
    ionBool         m_useSkinning;
};

ION_NAMESPACE_END