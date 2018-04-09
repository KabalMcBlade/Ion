#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/CoreDefs.h"

#include "../Texture/TextureCommon.h"

#include "RenderCommon.h"

#include "GPU.h"

EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN



class ION_DLL RenderCore final
{
public:
    ionBool    Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize, ERenderType _renderType);
    void       Shutdown();

    RenderCore();
    ~RenderCore();

    VkDevice& GetDevice() { return m_vkDevice; }
    const VkDevice& GetDevice() const { return m_vkDevice; }

    VkQueue& GetGraphicQueue() { return m_vkGraphicsQueue; }
    const VkQueue& GetGraphicQueue() const { return m_vkGraphicsQueue; }

    VkQueue& GetPresentQueue() { return m_vkPresentQueue; }
    const VkQueue& GetPresentQueue() const { return m_vkPresentQueue; }

    const ionS32& GetGraphicFamilyIndex() const { return m_vkGraphicsFamilyIndex; }
    const ionS32& GetPresentFamilyIndex() const { return m_vkPresentFamilyIndex; }

    const GPU& GetGPU() const { return m_vkGPU; }
    const VkSampleCountFlagBits& GetSampleCount() const { return m_vkSampleCount; }
    const ionBool& GetUsesSuperSampling() const { return m_vkSupersampling; }
    const VkRenderPass& GetRenderPass() const { return m_vkRenderPass; }
    const VkPipelineCache& GetPipelineCache() const { return m_vkPipelineCache; }

private:
    RenderCore(const RenderCore& _Orig) = delete;
    RenderCore& operator = (const RenderCore&) = delete;

private:
    // Utility functions
    VkSurfaceFormatKHR SelectSurfaceFormat(eosVector(VkSurfaceFormatKHR)& _vkFormats) const;
    VkPresentModeKHR SelectPresentMode(eosVector(VkPresentModeKHR)& _vkModes) const;
    VkExtent2D SelectSurfaceExtent(VkSurfaceCapabilitiesKHR& _vkCaps, ionU32 _width, ionU32 _height) const;
    VkFormat SelectSupportedFormat(VkPhysicalDevice _vkPhysicalDevice, VkFormat* _vkFormats, ionU32 _vkNumFormats, VkImageTiling _vkTiling, VkFormatFeatureFlags _vkFeatures) const;

    // They are in order to call
    // No matter about the "destroy" functions. NOTE: destroy functions are added for the vulkan type worth to have own destroy function, because more complex of a single call
    ionBool CreateInstance(ionBool _enableValidationLayer);
    ionBool CreatePresentationSurface(HINSTANCE _instance, HWND _handle);
    ionBool CreatePhysicalDevice();
    ionBool CreateLogicalDeviceAndQueues();
    ionBool CreateSemaphores();
    ionBool CreateQueryPool();
    ionBool CreateCommandPool();
    ionBool CreateCommandBuffer();
    ionBool CreateSwapChain(ionU32 _width, ionU32 _height, ionBool _fullScreen);
    void    DestroySwapChain();
    ionBool CreateRenderTargets();
    void    DestroyRenderTargets();
    ionBool CreateRenderPass();
    ionBool CreatePipelineCache();
    ionBool CreateFrameBuffers();
    void    DestroyFrameBuffers();

private:
    vkGpuMemoryAllocation       m_vkMSAAAllocation;
    GPU                         m_vkGPU;                  //  access through this component to get value such m_vkPhysicalDevice
    VkDevice                    m_vkDevice;
    VkSurfaceKHR                m_vkSurface;
    VkInstance                  m_vkInstance;
    VkQueue			            m_vkGraphicsQueue;
    VkQueue			            m_vkPresentQueue;
    VkCommandPool			    m_vkCommandPool;
    VkSwapchainKHR				m_vkSwapchain;
    VkFormat					m_vkSwapchainFormat;
    VkExtent2D					m_vkSwapchainExtent;
    VkPresentModeKHR			m_vkPresentMode;
    VkSampleCountFlagBits		m_vkSampleCount;
    VkFormat					m_vkDepthFormat;
    VkImage						m_vkMSAAImage;
    VkImageView					m_vkMSAAImageView;
    VkRenderPass				m_vkRenderPass;
    VkPipelineCache				m_vkPipelineCache;

    eosVector(VkSemaphore)	    m_vkAcquiringSemaphores;
    eosVector(VkSemaphore)	    m_vkCompletedSemaphores;
    eosVector(VkQueryPool)	    m_vkQueryPools;
    eosVector(VkCommandBuffer)	m_vkCommandBuffers;
    eosVector(VkFence)			m_vkCommandBufferFences;
    eosVector(VkImage)			m_vkSwapchainImages;
    eosVector(VkImageView)		m_vkSwapchainViews;
    eosVector(VkFramebuffer)	m_vkFrameBuffers;

    ERenderType                 m_vkRenderType;

    ionU32                      m_width;
    ionU32                      m_height;

    ionS32			            m_vkGraphicsFamilyIndex;
    ionS32			            m_vkPresentFamilyIndex;
    
    ionBool						m_vkSupersampling;
    ionBool                     m_vkFullScreen;
    ionBool                     m_vkValidationEnabled;
};

ION_NAMESPACE_END