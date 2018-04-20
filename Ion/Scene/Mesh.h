#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/IndexBufferObject.h"
#include "../Renderer/VertexBufferObject.h"


ION_NAMESPACE_BEGIN

class Texture;

class ION_DLL Mesh 
{
public:
    Mesh();
    ~Mesh();

protected:
    ionBool m_uniformDataChanged;

private:
    VertexBuffer    m_vertexBuffer;
    IndexBuffer     m_indexBuffer;

    Texture*        m_albedoMap;
    Texture*        m_normalMap;
    Texture*        m_roughnessMap;
    Texture*        m_metalnessMap;
    Texture*        m_ambientOcclusionMap;
    Texture*        m_emissiveMap;
};

ION_NAMESPACE_END