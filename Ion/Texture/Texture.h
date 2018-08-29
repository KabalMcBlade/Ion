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
    const VkDevice& GetDevice() const { return m_vkDevice; }
    const VkFormat& GetFormat() const { return m_format; }

    ionS32 GetWidth() const { return static_cast<ionS32>(m_width); }
    ionS32 GetHeight() const { return static_cast<ionS32>(m_height); }
    ionU32 GetComponent() const { return BitsPerFormat(m_optFormat) / 8; }
    ionU32 GetSize() const { return m_width * m_height * GetComponent(); }

    static ionU32 BitsPerFormat(ETextureFormat _format);

private:
    friend class TextureManager;

    ionBool CreateFromFile(const eosString& _path);
    ionBool CreateFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer, VkDeviceSize _bufferSize);
    ionBool Create();

    ionBool Save(const eosString& _path) const;

    ionBool GenerateTexture(ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureRepeat _repeat, ETextureType _type = ETextureType_2D, ionU32 _numLevel = 1);

    // this needed because Vulkan limitation
    void ConvertFrom3ChannelTo4Channel(ionU32 _width, ionU32 _height, const ionU8* _inBuffer, ionU8* _outBuffer);

    void Destroy();

    ionBool CreateSampler();
    VkFormat GetVulkanFormatFromTextureFormat(ETextureFormat _format);
    VkComponentMapping GetVulkanComponentMappingFromTextureFormat(ETextureFormat _format);

    ionBool LoadTextureFromFile(const eosString& _path);
    ionBool LoadCubeTextureFromFile(const eosString& _path);
    ionBool LoadCubeTextureFromFiles(const eosVector(eosString)& paths);

    ionBool LoadTextureFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer);

    void GenerateOptions();

    void UploadTextureToMemory(ionU32 _mipMapLevel, ionU32 _width, ionU32 _height, const ionU8* _buffer, ionU32 _index = 0 /* index of texture for cube-map, 0 by default */);

    void GenerateMipMaps();

    void CopyBufferRegion(const ionU8* _source, ionU32 _sourceWidth, ionU32 _sourceHeight, ionU32 _sourceComponentCount, ionU8* _dest, ionU32 _destWidth, ionU32 _destHeight, ionU32 _x, ionU32 _y);
    ionBool GenerateCubemapFromCross(const ionU8* _buffer, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _outBuffers[6]);
    void GenerateCubemapFromCrossVertical(const ionU8* _buffer, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _outBuffers[6]);
    void GenerateCubemapFromCrossHorizontal(const ionU8* _buffer, ionU32 _width, ionU32 _height, ionU32 _component, ionU8* _outBuffers[6]);

    void UploadTextureBuffer(const ionU8* _buffer, ionU32 _component, ionU32 _index = 0 /* index of texture for cube-map, 0 by default */);

private:
    eosString               m_name;
    VkDevice                m_vkDevice;
    vkGpuMemoryAllocation   m_allocation;

    VkFormat                m_format;
    VkImageView             m_view;
    VkImage                 m_image;
    VkImageLayout           m_layout;
    VkSampler               m_sampler;

    ETextureUsage           m_optUsage;
    ETextureFilter          m_optFilter;
    ETextureRepeat          m_optRepeat;
    ETextureType            m_optTextureType;
    ETextureFormat          m_optFormat;
    VkSamplerAddressMode    m_optCustomRepeat[3]; //u,v,w == s,t,r // custom repeat

    ionU32                  m_width;
    ionU32                  m_height;            // not needed for cube maps, actually.. it is a cube and so.. all same width :)
    ionU32                  m_numLevels;        // if this is set to 0, during generation it will be 1 for ETextureFilter_Nearest or ETextureFilter_Linear filters, otherwise will be based on the size

    ionU32                  m_maxAnisotropy;    // 1 means DISABLED anisotropy
};


ION_NAMESPACE_END