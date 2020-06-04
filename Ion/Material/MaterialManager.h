#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemorySettings.h"

#include "Material.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using MaterialManagerAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class ION_DLL MaterialManager final
{
public:
	static MaterialManagerAllocator* GetAllocator();

public:
    static MaterialManager& Instance();

    MaterialManager();
    ~MaterialManager();

    void        Init();
    void        Shutdown();

    Material*   CreateMaterial(const ionString& _name, ionU64 _stateBits = 0u);
    Material*   GetMaterial(const ionString& _name);

    // This call actually destroy/delete the material
    void        DestroyMaterial(const ionString& _name);

private:
    Material*   InternalCreateMaterial(const ionString& _name);
    void        DestroyMaterial(Material* _material);
    void        DestroyMaterial(ionSize _hash);         // This call actually destroy/delete the material

private:
    ionMap<ionSize, Material, MaterialManagerAllocator, GetAllocator> m_hashMaterial;
};

ION_NAMESPACE_END


#define ionMaterialManger() ion::MaterialManager::Instance()