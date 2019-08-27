#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Material.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL MaterialManager final
{
public:
    static MaterialManager& Instance();

    MaterialManager();
    ~MaterialManager();

    void        Init();
    void        Shutdown();

    Material*   CreateMaterial(const ionString& _name, ionU64 _stateBits = 0);
    Material*   GetMaterial(const ionString& _name) const;

    // This call actually destroy/delete the material
    void        DestroyMaterial(const ionString& _name);

private:
    Material*   InternalCreateMaterial(const ionString& _name);
    void        DestroyMaterial(Material* _material);
    void        DestroyMaterial(ionSize _hash);         // This call actually destroy/delete the material

private:
    ionMap<ionSize, Material*> m_hashMaterial;
};

ION_NAMESPACE_END


#define ionMaterialManger() ion::MaterialManager::Instance()