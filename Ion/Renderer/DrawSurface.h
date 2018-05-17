#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "RenderCommon.h"

#include "../Material/Material.h"

ION_NAMESPACE_BEGIN


struct DrawSurface final
{
    ionSize             m_indexCount;
    VertexCacheHandler  m_vertexCache;
    VertexCacheHandler  m_indexCache;
    const Material*     m_material;

    // This need to be filled with more info!
};



ION_NAMESPACE_END