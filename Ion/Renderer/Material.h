#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"

ION_NAMESPACE_BEGIN

class Texture;

class ION_DLL Material
{
public:
    Material();
    ~Material();

    ionU64 GetStateBits() const { return m_stateBits; }
    void GetStateBits(ionU64 _stateBits) { m_stateBits = _stateBits; }

    Texture* GetAlbedoMap() { return m_albedoMap; }
    Texture* GetNormalMap() { return m_normalMap; }
    Texture* GetRoughnessMap() { return m_roughnessMap; }
    Texture* GetMetalnessMap() { return m_metalnessMap; }
    Texture* GetAmbientOcclusionMap() { return m_ambientOcclusionMap; }
    Texture* GetEmissiveMap() { return m_emissiveMap; }

    const Texture* GetAlbedoMap() const { return m_albedoMap; }
    const  Texture* GetNormalMap() const { return m_normalMap; }
    const Texture* GetRoughnessMap() const { return m_roughnessMap; }
    const Texture* GetMetalnessMap() const { return m_metalnessMap; }
    const Texture* GetAmbientOcclusionMap() const { return m_ambientOcclusionMap; }
    const Texture* GetEmissiveMap() const { return m_emissiveMap; }

private:
    ionU64          m_stateBits;
    Texture*        m_albedoMap;
    Texture*        m_normalMap;
    Texture*        m_roughnessMap;
    Texture*        m_metalnessMap;
    Texture*        m_ambientOcclusionMap;
    Texture*        m_emissiveMap;
};

ION_NAMESPACE_END