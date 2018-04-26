#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/IndexBufferObject.h"
#include "../Renderer/VertexBufferObject.h"

#include "../Material/Material.h"

ION_NAMESPACE_BEGIN


struct Primitive
{
    ionU32      m_firstIndex;
    ionU32      m_indexCount;
    Material*   m_material;
};

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
    Primitive*      m_primitive;
};

ION_NAMESPACE_END