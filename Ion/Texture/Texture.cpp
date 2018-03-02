#include "Texture.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Texture::Texture(const eosString& _name) :
    m_name(_name)
{

}

Texture::~Texture()
{

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