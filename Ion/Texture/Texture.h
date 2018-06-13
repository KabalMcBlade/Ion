#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "TextureCommon.h"

#include "../Renderer/GPUMemoryManager.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class Texture final
{
public:
    Texture(VkDevice _vkDevice, const eosString& _name);
    ~Texture();

    const eosString& GetName() const { return m_name; }
    const VkImageView& GetView() const { return m_view; }
    const VkImageLayout& GetLayout() const { return m_layout; }
    const VkSampler& GetSampler() const { return m_sampler; }
    const VkImage& GetImage() const { return m_image; }

private:
    friend class TextureManager;

    ionBool CreateFromFile(const eosString& _path);
    ionBool CreateFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _buffer, VkDeviceSize _bufferSize);
    ionBool Create();

    // this needed because Vulkan limitation
    void ConvertFrom3ChannelTo4Channel(ionU32 _width, ionU32 _height, const ionU8* _inBuffer, ionU8* _outBuffer);

    void Destroy();

    ionBool CreateSampler();
    VkFormat GetVulkanFormatFromTextureFormat(ETextureFormat _format);
    VkComponentMapping GetVulkanComponentMappingFromTextureFormat(ETextureFormat _format);

    ionBool LoadTextureFromFile(const eosString& _path);
    ionBool LoadCubeTextureFromFile(const eosString& _path);

    ionBool LoadTextureFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _buffer);

    ionU32 BitsPerFormat(ETextureFormat _format);
    void GenerateOptions();

    void UploadTextureToMemory(ionU32 _mipMapLevel, ionU32 _width, ionU32 _height, const ionU8* _buffer, ionU32 _index /* = 0 // index of texture for cube-map, 0 by default */);

    void GenerateMipMaps();

private:
    eosString               m_name;
    VkDevice                m_vkDevice;
    vkGpuMemoryAllocation	m_allocation;

    VkFormat                m_format;
    VkImageView		        m_view;
    VkImage			        m_image;
    VkImageLayout	        m_layout;
    VkSampler		        m_sampler;
    VkSampleCountFlagBits	m_sampleCount;

    ETextureUsage	        m_optUsage;
    ETextureFilter	        m_optFilter;
    ETextureRepeat	        m_optRepeat;
    ETextureType		    m_optTextureType;
    ETextureFormat		    m_optFormat;

    ionU32					m_width;
    ionU32					m_height;			// not needed for cube maps, actually.. it is a cube and so.. all same width :)
    ionU32					m_numLevels;		// if this is set to 0, during generation it will be 1 for ETextureFilter_Nearest or ETextureFilter_Linear filters, otherwise will be based on the size

    ionU32					m_maxAnisotropy;    // 1 means DISABLED anisotropy
};


ION_NAMESPACE_END