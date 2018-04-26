#include "Material.h"

#include "../Texture/Texture.h"

ION_NAMESPACE_BEGIN

Material::Material(const eosString& _name) : 
    m_name(_name),
    m_stateBits(0),
    m_albedoMap(nullptr),
    m_normalMap(nullptr),
    m_roughnessMap(nullptr),
    m_metalnessMap(nullptr),
    m_ambientOcclusionMap(nullptr),
    m_emissiveMap(nullptr)
{
}

Material::~Material()
{

}

ionBool Material::Create(Texture* _albedoMap /*= nullptr*/, Texture* _normalMap /*= nullptr*/, Texture* _roughnessMap /*= nullptr*/, Texture* _metalnessMap /*= nullptr*/, Texture* _ambientOcclusionMap /*= nullptr*/, Texture* _emissiveMap /*= nullptr*/)
{
    m_albedoMap = _albedoMap;
    m_normalMap = _normalMap;
    m_roughnessMap = _roughnessMap;
    m_metalnessMap = _metalnessMap;
    m_ambientOcclusionMap = _ambientOcclusionMap;
    m_emissiveMap = _emissiveMap;

    return true;
}

void Material::Destroy()
{
    //m_name.clear();
    //m_stateBits = 0;
    m_albedoMap = nullptr;
    m_normalMap = nullptr;
    m_roughnessMap = nullptr;
    m_metalnessMap = nullptr;
    m_ambientOcclusionMap = nullptr;
    m_emissiveMap = nullptr;
}

ION_NAMESPACE_END
