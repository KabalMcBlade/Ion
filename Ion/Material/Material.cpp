#include "Material.h"

#include "../Texture/Texture.h"

#include "../Shader/ShaderProgramManager.h"

ION_NAMESPACE_BEGIN


BasePBR::BasePBR() :
    m_baseColorTexture(nullptr),
    m_metalRoughness(nullptr),
    m_metallicFactor(1.0f),
    m_roughnessFactor(1.0f)
{
    m_baseColor[0] = 1.0f;
    m_baseColor[1] = 1.0f;
    m_baseColor[2] = 1.0f;
    m_baseColor[3] = 1.0f;
}

BasePBR::~BasePBR()
{
    m_baseColorTexture = nullptr;
    m_metalRoughness = nullptr;
}

//////////////////////////////////////////////////////////////////////////

AdvancePBR::AdvancePBR() :
    m_normalTexture(nullptr),
    m_occlusionTexture(nullptr),
    m_emissiveTexture(nullptr),
    m_alphaCutoff(0.5f) // 0 means fully opaque, 1 full transparent
{
    m_emissiveColor[0] = 1.0f;
    m_emissiveColor[1] = 1.0f;
    m_emissiveColor[2] = 1.0f;
}

AdvancePBR::~AdvancePBR()
{
    m_normalTexture = nullptr;
    m_occlusionTexture = nullptr;
    m_emissiveTexture = nullptr;
}

//////////////////////////////////////////////////////////////////////////

SpecularGlossiness::SpecularGlossiness() :
    m_baseColorTexture(nullptr),
    m_specularGlossiness(nullptr)
{
    m_baseColor[0] = 1.0f;
    m_baseColor[1] = 1.0f;
    m_baseColor[2] = 1.0f;
    m_baseColor[3] = 1.0f;

    m_specularGlossinessColor[0] = 1.0f;
    m_specularGlossinessColor[1] = 1.0f;
    m_specularGlossinessColor[2] = 1.0f;
    m_specularGlossinessColor[3] = 1.0f;
}

SpecularGlossiness::~SpecularGlossiness()
{
    m_baseColorTexture = nullptr;
    m_specularGlossiness = nullptr;
}

//////////////////////////////////////////////////////////////////////////

Material::Material(const eosString& _name) :
    m_name(_name),
    m_vertexShaderIndex(-1),
    m_fragmentShaderIndex(-1),
    m_tessellationControlIndex(-1),
    m_tessellationEvaluationIndex(-1),
    m_geometryIndex(-1),
    m_useJoint(false),
    m_useSkinning(false),
    m_useGlossiness(false),
    m_alphaMode(EAlphaMode_Opaque)
{
}

Material::~Material()
{

}

ionBool Material::Create()
{
    return true;
}

void Material::Destroy()
{

}

void Material::SetShaders(const ionS32 _vertexIndex, const ionS32 _fragmentIndex /*= -1*/, const ionS32 _tessellationControlIndex /*= -1*/, const ionS32 _tessellationEvaluationIndex /*= -1*/, const ionS32 _geometryIndex /*= -1*/, const ionBool _useJoint /*= false*/, const ionBool _useSkinning /*= false*/)
{
    m_vertexShaderIndex = _vertexIndex;
    m_fragmentShaderIndex = _fragmentIndex;
    m_tessellationControlIndex = _tessellationControlIndex;
    m_tessellationEvaluationIndex = _tessellationEvaluationIndex;
    m_geometryIndex = _geometryIndex;
    m_useJoint = _useJoint;
    m_useSkinning = _useSkinning;
}

void Material::GetShaders(ionS32& _vertexIndex, ionS32& _fragmentIndex, ionS32& _tessellationControlIndex, ionS32& _tessellationEvaluationIndex, ionS32& _geometryIndex, ionBool& _useJoint, ionBool& _useSkinning) const
{
    _vertexIndex = m_vertexShaderIndex;
    _fragmentIndex = m_fragmentShaderIndex;
    _tessellationControlIndex = m_tessellationControlIndex;
    _tessellationEvaluationIndex = m_tessellationEvaluationIndex;
    _geometryIndex = m_geometryIndex;
    _useJoint = m_useJoint;
    _useSkinning = m_useSkinning;
}

ION_NAMESPACE_END
