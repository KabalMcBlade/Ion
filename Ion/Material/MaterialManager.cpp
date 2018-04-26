#include "MaterialManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Texture/TextureManager.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


MaterialManager *MaterialManager::s_instance = nullptr;

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(MaterialManager, EOS_MEMORY_ALIGNMENT_SIZE);
    }
}

void MaterialManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

MaterialManager& MaterialManager::Instance()
{
    return *s_instance;
}

void MaterialManager::Init()
{

}

void MaterialManager::Shutdown()
{
    auto begin = m_hashMaterial.begin(), end = m_hashMaterial.end();
    std::map<ionSize, Material*, StlAllocator<ionSize, HeapAllocPolicy<ionSize> > >::iterator it = begin;
    for (; it != end; ++it)
    {
        DestroyMaterial(it->second);
    }
    m_hashMaterial.clear();
}

Material* MaterialManager::CreateMaterial(const eosString& _name, ionU64 _stateBits, const eosString& _albedoMap /*= ""*/, const eosString& _normalMap /*= ""*/, const eosString& _roughnessMap /*= ""*/, const eosString& _metalnessMap /*= ""*/, const eosString& _ambientOcclusionMap /*= ""*/, const eosString& _emissiveMap /*= ""*/)
{
    if (_name.empty())
    {
        return nullptr;
    }

    Material* material = GetMaterial(_name);
    if (material == nullptr)
    {
        material = CreateMaterial(_name);
    }
    else
    {
        DestroyMaterial(material);
    }

    Texture* albedoMap = ionTextureManger().GetTexture(_albedoMap);
    Texture* normalMap = ionTextureManger().GetTexture(_normalMap);
    Texture* roughnessMap = ionTextureManger().GetTexture(_roughnessMap);
    Texture* metalnessMap = ionTextureManger().GetTexture(_metalnessMap);
    Texture* ambientOcclusionMap = ionTextureManger().GetTexture(_ambientOcclusionMap);
    Texture* emissiveMap = ionTextureManger().GetTexture(_emissiveMap);
    
    material->SetStateBits(_stateBits);
    if (material->Create(albedoMap, normalMap, roughnessMap, metalnessMap, ambientOcclusionMap, emissiveMap))
    {
        return material;
    }
    else
    {
        return nullptr;
    }
}

Material* MaterialManager::GetMaterial(const eosString& _name) const
{
    if (_name.empty())
    {
        return nullptr;
    }

    ionSize hash = std::hash<eosString>{}(_name);

    auto search = m_hashMaterial.find(hash);
    if (search != m_hashMaterial.end())
    {
        return search->second;
    }
    else
    {
        return nullptr;
    }
}

Material* MaterialManager::CreateMaterial(const eosString& _name)
{
    if (_name.empty())
    {
        return nullptr;
    }

    ionSize hash = std::hash<eosString>{}(_name);

    // just to inform the user
    auto search = m_hashMaterial.find(hash);
    ionAssert(!(search != m_hashMaterial.end()), "A material with the same name has already added!");

    Material* material = eosNew(Material, EOS_MEMORY_ALIGNMENT_SIZE, _name);

    m_hashMaterial[hash] = material;

    return material;
}

void MaterialManager::DestroyMaterial(ionSize _hash)
{
    auto search = m_hashMaterial.find(_hash);
    if (search != m_hashMaterial.end())
    {
        DestroyMaterial(search->second);
    }
}

void MaterialManager::DestroyMaterial(Material* _material)
{
    if (_material != nullptr)
    {
        _material->Destroy();
        eosDelete(_material);
    }
}

ION_NAMESPACE_END