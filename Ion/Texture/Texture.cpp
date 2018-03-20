#include "Texture.h"

#include "TextureManager.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Texture::Texture(VkDevice _vkDevice, const eosString& _name) :
    m_vkDevice(_vkDevice),
    m_name(_name)
{
    m_isSwapChainImage = false;
    m_format = VK_FORMAT_UNDEFINED;
    m_image = VK_NULL_HANDLE;
    m_view = VK_NULL_HANDLE;
    m_layout = VK_IMAGE_LAYOUT_GENERAL;
    m_sampler = VK_NULL_HANDLE;
    m_filter = ETextureFilter_Default;
    m_repeat = ETextureRepeat_Repeat;
    m_usage = ETextureUsage_Default;
    m_isCubeMap = false;
    m_isProvedurallyGenerated = false;
}

Texture::~Texture()
{
    if (!m_isSwapChainImage)
    {
        Destroy();
    }
}

void Texture::SetOptions(const TextureOptions& _options)
{
    m_options = _options;
}

void Texture::Create()
{
 
}

void Texture::Destroy()
{

}

ION_NAMESPACE_END