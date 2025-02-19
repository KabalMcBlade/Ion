// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Texture\Texture.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../GPU/GpuDataStructure.h"
#include "../GPU/GpuMemoryAllocator.h"

#include "../Core/MemoryWrapper.h"

#include "TextureCommon.h"

#include "../Core/MemorySettings.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using TextureAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class Texture final
{
public:
	static ION_DLL TextureAllocator* GetAllocator();

public:
    Texture(VkDevice _vkDevice, const ionString& _name);
    ~Texture();

    const ionString& GetName() const { return m_name; }
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

    ionBool CreateFromFile(const ionString& _path);
    ionBool CreateFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer, VkDeviceSize _bufferSize);
    ionBool Create();

    ionBool Save(const ionString& _path) const;

    ionBool GenerateTexture(ionU32 _width, ionU32 _height, ETextureFormat _format, ETextureRepeat _repeat, ETextureType _type = ETextureType_2D, ionU32 _numLevel = 1);

    // this needed because Vulkan limitation
    void ConvertFrom3ChannelTo4Channel(ionU32 _width, ionU32 _height, const ionU8* _inBuffer, ionU8* _outBuffer);

    void Destroy();

    ionBool CreateSampler();
    VkFormat GetVulkanFormatFromTextureFormat(ETextureFormat _format);
    VkComponentMapping GetVulkanComponentMappingFromTextureFormat(ETextureFormat _format);
    void GetVulkanFiltersFromTextureFilters(ETextureFilterMin _min0, ETextureFilterMag _mag0, VkFilter& _min, VkFilter& _mag, VkSamplerMipmapMode& _mipmap);

    ionBool LoadTextureFromFile(const ionString& _path);
    ionBool LoadCubeTextureFromFile(const ionString& _path);
    ionBool LoadCubeTextureFromFiles(const ionVector<ionString, TextureAllocator, GetAllocator>& paths);

    ionBool LoadTextureFromBuffer(ionU32 _width, ionU32 _height, ionU32 _component, const ionU8* _buffer);

    void GenerateOptions();

    void UploadTextureToMemory(ionU32 _mipMapLevel, ionU32 _width, ionU32 _height, const ionU8* _buffer, ionU32 _index = 0 /* index of texture for cube-map, 0 by default */);

    void GenerateMipMaps();

    void UploadTextureBuffer(const ionU8* _buffer, ionU32 _component, ionU32 _index = 0 /* index of texture for cube-map, 0 by default */);

private:
	ionString               m_name;
    VkDevice                m_vkDevice;
    GpuMemoryAllocation   m_allocation;

    VkFormat                m_format;
    VkImageView             m_view;
    VkImage                 m_image;
    VkImageLayout           m_layout;
    VkSampler               m_sampler;

    ETextureUsage           m_optUsage;
    ETextureFilterMin       m_optFilterMin;
    ETextureFilterMag       m_optFilterMag;
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