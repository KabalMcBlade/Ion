#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Material.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL MaterialManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static MaterialManager& Instance();

    MaterialManager();
    ~MaterialManager();

    void        Init();
    void        Shutdown();

    Material*   CreateMaterial(const eosString& _name, ionU64 _stateBits = 0);
    Material*   GetMaterial(const eosString& _name) const;

    // This call actually destroy/delete the material
    void        DestroyMaterial(const eosString& _name);

private:
    Material*   InternalCreateMaterial(const eosString& _name);
    void        DestroyMaterial(Material* _material);
    void        DestroyMaterial(ionSize _hash);         // This call actually destroy/delete the material

private:
    eosMap(ionSize, Material*) m_hashMaterial;

    static MaterialManager *s_instance;
};

ION_NAMESPACE_END


#define ionMaterialManger() ion::MaterialManager::Instance()