#include "MaterialManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Texture/TextureManager.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

MaterialManager& MaterialManager::Instance()
{
    static MaterialManager instance;
    return instance;
}

void MaterialManager::Init()
{

}

void MaterialManager::Shutdown()
{
    auto begin = m_hashMaterial->begin(), end = m_hashMaterial->end();
    std::map<ionSize, Material*>::iterator it = begin;
    for (; it != end; ++it)
    {
        DestroyMaterial(it->second);
        ionDelete(it->second);
    }
    m_hashMaterial->clear();
}

Material* MaterialManager::CreateMaterial(const ionString& _name, ionU64 _stateBits /*= 0*/)
{
    if (_name.empty())
    {
        return nullptr;
    }

    Material* material = GetMaterial(_name);
    if (material == nullptr)
    {
        material = InternalCreateMaterial(_name);
    }
    else
    {
        DestroyMaterial(material);
    }

    material->GetState().SetStateBits(_stateBits);
    if (material->Create())
    {
        return material;
    }
    else
    {
        return nullptr;
    }
}

Material* MaterialManager::GetMaterial(const ionString& _name) const
{
    if (_name.empty())
    {
        return nullptr;
    }

    ionSize hash = std::hash<ionString>{}(_name);

    auto search = m_hashMaterial->find(hash);
    if (search != m_hashMaterial->end())
    {
        return search->second;
    }
    else
    {
        return nullptr;
    }
}

Material* MaterialManager::InternalCreateMaterial(const ionString& _name)
{
    if (_name.empty())
    {
        return nullptr;
    }

    ionSize hash = std::hash<ionString>{}(_name);

    // just to inform the user
    auto search = m_hashMaterial->find(hash);
    ionAssert(!(search != m_hashMaterial->end()), "A material with the same name has already added!");

    Material* material = ionNew(Material, _name);

    m_hashMaterial[hash] = material;

    return material;
}

void MaterialManager::DestroyMaterial(const ionString& _name)
{
    if (_name.empty())
    {
        return;
    }

    ionSize hash = std::hash<ionString>{}(_name);
    DestroyMaterial(hash);
}

void MaterialManager::DestroyMaterial(ionSize _hash)
{
    auto search = m_hashMaterial->find(_hash);
    if (search != m_hashMaterial->end())
    {
        DestroyMaterial(search->second);
        ionDelete(search->second);
        m_hashMaterial->erase(_hash);
    }
}

void MaterialManager::DestroyMaterial(Material* _material)
{
    if (_material != nullptr)
    {
        _material->Destroy();
    }
}

ION_NAMESPACE_END