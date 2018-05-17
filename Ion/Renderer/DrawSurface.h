#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "RenderCommon.h"

#include "../Material/Material.h"

ION_NAMESPACE_BEGIN


struct DrawSurface final
{
    DrawSurface*        m_subSurface;
    ionSize             m_indexCount;
    VertexCacheHandler  m_vertexCache;
    VertexCacheHandler  m_indexCache;
    ionFloat            m_modelMatrix[16];
    ionFloat            m_viewMatrix[16];
    ionFloat            m_projectionMatrix[16];
    const Material*     m_material;

    DrawSurface()
    {
        m_subSurface = nullptr;
        m_indexCount = 0;
        m_vertexCache = 0;
        m_indexCache = 0;
        memset(&m_modelMatrix, 0, sizeof(m_modelMatrix));
        memset(&m_viewMatrix, 0, sizeof(m_viewMatrix));
        memset(&m_projectionMatrix, 0, sizeof(m_projectionMatrix));
        m_material = nullptr;
    }
    // This need to be filled with more info!
};



ION_NAMESPACE_END