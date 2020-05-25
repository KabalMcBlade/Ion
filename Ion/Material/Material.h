#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"


#include "MaterialState.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/RenderState.h"

#include "../Shader/ShaderProgram.h"

#include "../Core/MemorySettings.h"


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

class ION_DLL SpecularGlossiness
{
public:
    SpecularGlossiness();
    ~SpecularGlossiness();

    const Texture* GetBaseColorTexture() const { return m_baseColorTexture; }
    const Texture* GetSpecularGlossinessTexture() const { return m_specularGlossiness; }

    void SetBaseColorTexture(const Texture* _texture) { m_baseColorTexture = _texture; }
    void SetSpecularGlossinessTexture(const Texture* _texture) { m_specularGlossiness = _texture; }

    void GetBaseColor(ionFloat& _r, ionFloat& _g, ionFloat& _b, ionFloat& _a) const
    {
        _r = m_baseColor[0];
        _g = m_baseColor[1];
        _b = m_baseColor[2];
        _a = m_baseColor[3];
    }
    const ionFloat* GetBaseColor() const { return &m_baseColor[0]; }

    void SetBaseColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        m_baseColor[0] = _r;
        m_baseColor[1] = _g;
        m_baseColor[2] = _b;
        m_baseColor[3] = _a;
    }


    void GetGlossinessColor(ionFloat& _r, ionFloat& _g, ionFloat& _b, ionFloat& _a) const
    {
        _r = m_glossinessColor[0];
        _g = m_glossinessColor[1];
        _b = m_glossinessColor[2];
        _a = m_glossinessColor[3];
    }
    const ionFloat* GetGlossinessColor() const { return &m_glossinessColor[0]; }

    void SetGlossinessColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        m_glossinessColor[0] = _r;
        m_glossinessColor[1] = _g;
        m_glossinessColor[2] = _b;
        m_glossinessColor[3] = _a;
    }


    void GetSpecularColor(ionFloat& _r, ionFloat& _g, ionFloat& _b, ionFloat& _a) const
    {
        _r = m_specularColor[0];
        _g = m_specularColor[1];
        _b = m_specularColor[2];
        _a = m_specularColor[3];
    }
    const ionFloat* GetSpecularColor() const { return &m_specularColor[0]; }

    void SetSpecularColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        m_specularColor[0] = _r;
        m_specularColor[1] = _g;
        m_specularColor[2] = _b;
        m_specularColor[3] = _a;
    }


private:
    const Texture*    m_baseColorTexture;
    const Texture*    m_specularGlossiness;

    ionFloat    m_baseColor[4];
    ionFloat    m_glossinessColor[4];
    ionFloat    m_specularColor[4];
};


//////////////////////////////////////////////////////////////////////////

// order is important, check the m_sortingIndex in the DrawSurface 
enum EAlphaMode : ionU8
{
    EAlphaMode_Opaque = 0,
    EAlphaMode_Mask,
    EAlphaMode_Blend
};

struct DrawSurface;
class ION_DLL Material
{
public:
	Material();
    Material(const ionString& _name);
    ~Material();

	void SetName(const ionString& _name) { m_name = _name; }
    const ionString& GetName() const { return m_name; }

    ionBool Create();
    void Destroy();

    void SetDiffuseLight(ionBool _isDiffuse) { m_isDiffuseLight = _isDiffuse; }
    const ionBool IsDiffuseLight() const { return m_isDiffuseLight; }

    void SetUnlit(ionBool _isUnlit) { m_isUnlit = _isUnlit; }
    const ionBool IsUnlit() const { return m_isUnlit; }

    void SetSpecularGloss(ionBool _specularGlossiness) { m_useGlossiness = _specularGlossiness; }
    const ionBool IsSpecularGlossiness() const { return m_useGlossiness; }

    MaterialState& GetState() { return m_state; }
    const MaterialState& GetState() const { return m_state; }

    BasePBR& GetBasePBR() { return m_basePBR; }
    const BasePBR& GetBasePBR() const { return m_basePBR; }

    AdvancePBR& GetAdvancePBR() { return m_advancePBR; }
    const AdvancePBR& GetAdvancePBR() const { return m_advancePBR; }

    SpecularGlossiness& GetSpecularGlossiness() { return m_specularGlossiness; }
    const SpecularGlossiness& GetSpecularGlossiness() const { return m_specularGlossiness; }

    void SetAlphaMode(EAlphaMode _mode) { m_alphaMode = _mode; }
    EAlphaMode GetAlphaMode() const { return m_alphaMode; }

    void SetShaders(const ionS32 _vertexIndex, const ionS32 _fragmentIndex = -1, const ionS32 _tessellationControlIndex = -1, const ionS32 _tessellationEvaluationIndex = -1, const ionS32 _geometryIndex = -1, const ionBool _useJoint = false, const ionBool _useSkinning = false);
    void GetShaders(ionS32& _vertexIndex, ionS32& _fragmentIndex, ionS32& _tessellationControlIndex, ionS32& _tessellationEvaluationIndex, ionS32& _geometryIndex, ionBool& _useJoint, ionBool& _useSkinning) const;

    void SetVertexLayout(EVertexLayout _layout) { m_vertexLayout = _layout; }
    EVertexLayout GetVertexLayout() const { return m_vertexLayout; }

    ionBool IsValidPBR() const;
    ionBool IsValidSpecularGlossiness() const;

    //////////////////////////////////////////////////////////////////////////
    // Shader Specific
    void SetConstantsShaders(const ConstantsBindingDef& _constants) { m_constants = _constants; m_constants.m_runtimeStages = (VkShaderStageFlagBits)m_constants.m_shaderStages; }
    const ConstantsBindingDef& GetConstantsShaders() const { return m_constants; }

    void SetVertexShaderLayout(const ShaderLayoutDef& _defines);
    void SetTessellationControlShaderLayout(const ShaderLayoutDef& _defines);
    void SetTessellationEvaluatorShaderLayout(const ShaderLayoutDef& _defines);
    void SetGeometryShaderLayout(const ShaderLayoutDef& _defines);
    void SetFragmentShaderLayout(const ShaderLayoutDef& _defines);
    void SetComputeShaderLayout(const ShaderLayoutDef& _defines);

    const ShaderLayoutDef& GetVertexShaderLayout() const { return m_vertexShaderLayout; }
    const ShaderLayoutDef& GetTessellationControlShaderLayout() const { return m_tessCtrlShaderLayout; }
    const ShaderLayoutDef& GetTessellationEvaluatorShaderLayout() const { return m_tessEvalShaderLayout; }
    const ShaderLayoutDef& GetGeometryShaderLayout() const { return m_geomtryShaderLayout; }
    const ShaderLayoutDef& GetFragmentShaderLayout() const { return m_fragmentShaderLayout; }
    const ShaderLayoutDef& GetComputeShaderLayout() const { return m_computeShaderLayout; }

    //
    // Special
    void SetTopology(VkPrimitiveTopology _topology) { m_topology = _topology; }
    VkPrimitiveTopology GetTopology() const { return m_topology; }

private:
    ionString m_name;

    std::function<void(const DrawSurface& _surface)> m_customDrawFunction;

    ShaderLayoutDef m_vertexShaderLayout;
    ShaderLayoutDef m_tessCtrlShaderLayout;
    ShaderLayoutDef m_tessEvalShaderLayout;
    ShaderLayoutDef m_geomtryShaderLayout;
    ShaderLayoutDef m_fragmentShaderLayout;
    ShaderLayoutDef m_computeShaderLayout;

    BasePBR         m_basePBR;
    AdvancePBR      m_advancePBR;
    SpecularGlossiness m_specularGlossiness;

    ConstantsBindingDef m_constants;

    VkPrimitiveTopology m_topology;

    EVertexLayout   m_vertexLayout;
    EAlphaMode      m_alphaMode;

    MaterialState   m_state;

    ionS32          m_vertexShaderIndex;
    ionS32          m_fragmentShaderIndex;
    ionS32          m_tessellationControlIndex;
    ionS32          m_tessellationEvaluationIndex;
    ionS32          m_geometryIndex;

    ionBool         m_isUnlit;
    ionBool         m_isDiffuseLight;
    ionBool         m_useGlossiness;       // the default render pipeline is PBR, but if the texture are not found, this became true and use the specular glossiness, so it is just a fallback!
    ionBool         m_useJoint;
    ionBool         m_useSkinning;
};

ION_NAMESPACE_END