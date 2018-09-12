#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Material/Material.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class ION_DLL Mesh final
{
public:
    Mesh();
    ~Mesh();

    ionU32 GetIndexStart() const;
    ionU32 GetIndexCount() const;
    Material* GetMaterial() const;

    void SetIndexStart(ionU32 _start);
    void SetIndexCount(ionU32 _count);
    void SetMaterial(Material* _material);

private:
    ionU32                  m_indexStart;
    ionU32                  m_indexCount;
    Material*               m_material;
};


ION_NAMESPACE_END