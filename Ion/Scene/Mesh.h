#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/IndexBufferObject.h"
#include "../Renderer/VertexBufferObject.h"
#include "../Renderer/Material.h"

ION_NAMESPACE_BEGIN


class ION_DLL Mesh 
{
public:
    Mesh();
    ~Mesh();

    const Material& GetMaterial() const { return m_material; }

    void SetMaterial(Material& _material) { m_material = _material; }

protected:
    ionBool m_uniformDataChanged;

private:
    VertexBuffer    m_vertexBuffer;
    IndexBuffer     m_indexBuffer;
    Material        m_material;
};

ION_NAMESPACE_END