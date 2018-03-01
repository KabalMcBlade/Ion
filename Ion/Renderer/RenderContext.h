#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/CoreDefs.h"

#include "GPU.h"

EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL RenderContext
{
public:
    ionBool    Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer);
    void       Shutdown();

    RenderContext();
    ~RenderContext();

private:
    RenderContext(const RenderContext& _Orig) = delete;
    RenderContext& operator = (const RenderContext&) = delete;

private:
    // Utility functions
    VkSurfaceFormatKHR SelectSurfaceFormat(eosVector(VkSurfaceFormatKHR)& _vkFormats) const;
    VkPresentModeKHR SelectPresentMode(eosVector(VkPresentModeKHR)& _vkModes) const;
    VkExtent2D SelectSurfaceExtent(VkSurfaceCapabilitiesKHR& _vkCaps, ionU32 _width, ionU32 _height) const;

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
    void    DestroyPipelineCache();
    ionBool CreateFrameBuffers();
    void    DestroyFrameBuffers();

private:
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

    eosVector(VkSemaphore)	    m_vkAcquiringSemaphores;
    eosVector(VkSemaphore)	    m_vkCompletedSemaphores;
    eosVector(VkQueryPool)	    m_vkQueryPools;
    eosVector(VkCommandBuffer)	m_vkCommandBuffers;
    eosVector(VkFence)			m_vkCommandBufferFences;
    eosVector(VkImage)			m_vkSwapchainImages;
    eosVector(VkImageView)		m_vkSwapchainViews;

    ionS32			            m_vkGraphicsFamilyIndex;
    ionS32			            m_vkPresentFamilyIndex;

    ionBool                     m_vkFullScreen;
    ionBool                     m_vkValidationEnabled;
};

ION_NAMESPACE_END