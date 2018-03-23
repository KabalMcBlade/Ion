#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "TextureCommon.h"
#include "TextureOptions.h"

#include "../Renderer/GPUMemoryManager.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class Texture final
{
public:
    Texture(VkDevice _vkDevice, const eosString& _name);
    ~Texture();

    const eosString& GetName() const { return m_name; }
    const TextureOptions& GetOptions() const { return m_options; }

    const VkImageView& GetView() const { return m_view; }

private:
    friend class TextureManager;

    void SetOptions(const TextureOptions& _options);

    ionBool CreateFromFile(const eosString& _path, ETextureFilter _filter = ETextureFilter_Default, ETextureRepeat _repeat = ETextureRepeat_Clamp, ETextureUsage _usage = ETextureUsage_Default, ETextureType _type = ETextureType_2D);
    ionBool Create();

    void Destroy();

    ionBool CreateSampler();
    VkFormat GetVulkanFormatFromTextureFormat(ETextureFormat _format);
    VkComponentMapping GetVulkanComponentMappingFromTextureFormat(ETextureFormat _format, ETextureColor _color);

private:
    eosString               m_name;
    VkDevice                m_vkDevice;
    vkGpuMemoryAllocation	m_allocation;
    TextureOptions	        m_options;
    VkImageView		        m_view;
    VkFormat		        m_format;
    VkImage			        m_image;
    VkImageLayout	        m_layout;
    VkSampler		        m_sampler;
    ETextureUsage	        m_usage;
    ETextureFilter	        m_filter;
    ETextureRepeat	        m_repeat;
    ionBool                 m_isCubeMap;
    ionBool			        m_isSwapChainImage;
    ionBool			        m_isProvedurallyGenerated;
};


ION_NAMESPACE_END