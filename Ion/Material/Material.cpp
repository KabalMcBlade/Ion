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
    m_occlusionMap(nullptr),
    m_emissiveMap(nullptr),
    m_alphaCutoff(1.0f),
    m_metallicFactor(1.0f),
    m_roughnessFactor(1.0f)
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
    //m_name.clear();
    //m_stateBits = 0;
    m_albedoMap = nullptr;
    m_normalMap = nullptr;
    m_roughnessMap = nullptr;
    m_metalnessMap = nullptr;
    m_occlusionMap = nullptr;
    m_emissiveMap = nullptr;
}

ION_NAMESPACE_END
