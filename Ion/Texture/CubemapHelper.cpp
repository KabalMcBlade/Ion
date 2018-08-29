#include "CubemapHelper.h"

#include "TextureManager.h"

//#include "../Renderer/StagingBufferManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Dependencies/Miscellaneous/stb_image.h"
/*
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Dependencies/Miscellaneous/stb_image_write.h"
*/

EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN


CubemapHelper::CubemapHelper()
{
    Clear();
}

CubemapHelper::~CubemapHelper()
{
    Clear();
}

ionBool CubemapHelper::Load(const eosString& _path, ETextureFormat _format)
{
    m_format = _format;

    if (stbi_is_hdr(_path.c_str()))
    {
        m_originalBufferFloats = stbi_loadf(_path.c_str(), &m_width, &m_height, &m_component, 0);
    }
    else
    {
        m_originalBufferBytes = stbi_load(_path.c_str(), &m_width, &m_height, &m_component, 0);
    }
    
    m_numLevels = 1;
    ionS32 width = m_width;
    ionS32 height = m_height;
    while (width > 1 || height > 1)
    {
        width >>= 1;
        height >>= 1;

        m_numLevels++;
    }

    return (m_originalBufferFloats != nullptr || m_originalBufferBytes != nullptr);
}

void CubemapHelper::Unload()
{
    if (m_originalBufferFloats != nullptr)
    {
        stbi_image_free(m_originalBufferFloats);
    }
    else if (m_originalBufferBytes != nullptr)
    {
        stbi_image_free(m_originalBufferBytes);
    }

    Clear();
}

void CubemapHelper::Clear()
{
    m_width = 0;
    m_height = 0;
    m_component = 0;
    m_size = 0;
    m_numLevels = 0;
    m_format = ETextureFormat_None;
    m_buffer = nullptr;
    m_originalBufferBytes = nullptr;
    m_originalBufferShorts = nullptr;
    m_originalBufferFloats = nullptr;
}

ionBool CubemapHelper::IsLatLong()
{
    const ionFloat aspect = (ionFloat)m_width / (ionFloat)m_height;
    return std::fabsf(aspect - 2.0f) < 0.00001f;
}

ionBool CubemapHelper::IsHStrip()
{
    return (m_width == 6 * m_height);
}

ionBool CubemapHelper::IsVStrip()
{
    return (6 * m_width == m_height);
}

ionBool CubemapHelper::IsCubeCross()
{
    const ionFloat aspect = (ionFloat)m_width / (ionFloat)m_height;
    const ionBool isVertical = std::fabsf(aspect - (3.0f / 4.0f)) < 0.0001f;
    const ionBool isHorizontal = std::fabsf(aspect - (4.0f / 3.0f)) < 0.0001f;

    if (!isVertical && !isHorizontal)
    {
        return false;
    }

    return true;
}

ionBool CubemapHelper::ConvertToTexture(Texture* _output)
{
    if (IsCubeCross())
    {
        return CubemapFromCross(_output);
    }
    else if (IsLatLong())
    {
        return CubemapFromLatLong(_output);
    }
    else if (IsHStrip())
    {
        return CubemapFromStrip(_output);
    }
    else if (IsVStrip())
    {
        return CubemapFromStrip(_output);
    }
    else
    {
        return false;
    }
}

ionBool CubemapHelper::CubemapFromCross(Texture* _output)
{
    return false;
}

ionBool CubemapHelper::CubemapFromLatLong(Texture* _output)
{
    return false;
}

ionBool CubemapHelper::CubemapFromStrip(Texture* _output)
{
    return false;
}

ION_NAMESPACE_END