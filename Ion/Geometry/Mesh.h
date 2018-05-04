#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/IndexBufferObject.h"
#include "../Renderer/VertexBufferObject.h"

#include "../Material/Material.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL Mesh 
{
public:
    Mesh();
    ~Mesh();

    void PushBackVertex(const Vertex& _vertex);
    void PushBackIndex(const Index& _index);

protected:
    ionBool m_uniformDataChanged;

private:
    eosVector(Vertex)   m_vertexes;
    eosVector(Index)    m_indexes;
    Material*           m_material;
};

ION_NAMESPACE_END