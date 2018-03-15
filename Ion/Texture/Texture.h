#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "TextureCommon.h"
#include "TextureOptions.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class Texture final
{
public:
    Texture(const eosString& _name);
    ~Texture();

    const eosString& GetName() const { return m_name; }
    const TextureOptions& GetOptions() const { return m_options; }

    const VkImageView& GetView() const { return m_view; }

    void SetOptions(const TextureOptions& _options);

    void Create();
    void Destroy();

private:
    eosString       m_name;
    TextureOptions	m_options;
    VkImageView		m_view;
};


ION_NAMESPACE_END