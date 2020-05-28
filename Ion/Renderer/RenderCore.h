#pragma once

#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../GPU/GpuDataStructure.h"
#include "../GPU/GpuMemoryAllocator.h"

#include "../Core/CoreDefs.h"
#include "../Core/MemoryWrapper.h"

#include "../Texture/TextureCommon.h"

#include "../Core/MemorySettings.h"

#include "RenderCommon.h"

#include "GPU.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using RenderCoreAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class Texture;
class ION_DLL RenderCore final
{
public:
	static RenderCoreAllocator* GetAllocator();

public:
    RenderCore();
    ~RenderCore();

    ionBool Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize);
    void    Shutdown();
    void    DestroyCommandBuffers();
    void    Recreate();
    void    Clear();

    EFrameStatus StartFrame();
    EFrameStatus EndFrame();
    void    StartRenderPass(VkRenderPass _renderPass, VkFramebuffer _frameBuffer, ionFloat _clearDepthValue, ionU8 _clearStencilValue, ionFloat _clearRed, ionFloat _clearGreen, ionFloat _clearBlue, const VkRect2D& _renderArea);
    void    EndRenderPass();
    void    SetDefaultState();
    void    SetState(ionU64 _stateBits);
    void    SetScissor(ionS32 _leftX, ionS32 _bottomY, ionU32 _width, ionU32 _height);
    void    SetViewport(ionFloat _leftX, ionFloat _bottomY, ionFloat _width, ionFloat _height, ionFloat _minDepth, ionFloat _maxDepth);
    void    SetScissor(const VkRect2D& _scissor);
    void    SetViewport(const VkViewport& _viewport);
    void    SetPolygonOffset(ionFloat _scale, ionFloat _bias);
    void    SetDepthBoundsTest(ionFloat _zMin, ionFloat _zMax);
    void    CopyFrameBuffer(Texture* _texture, VkImage _srcImage);
    void    CopyFrameBuffer(Texture* _texture);
    void    Draw(VkRenderPass _renderPass, const DrawSurface& _surface);
    
    //////////////////////////////////////////////////////////////////////////
    // Support function for custom purpose

    VkCommandBuffer CreateCustomCommandBuffer(VkCommandBufferLevel _level);
    ionBool BeginCustomCommandBuffer(VkCommandBuffer _commandBuffer);
    void EndCustomCommandBuffer(VkCommandBuffer _commandBuffer);
    void FlushCustomCommandBuffer(VkCommandBuffer _commandBuffer);

    void Draw(VkCommandBuffer _commandBuffer, VkRenderPass _renderPass, const DrawSurface& _surface);
    void DrawNoBinding(VkCommandBuffer _commandBuffer, VkRenderPass _renderPass, const DrawSurface& _surface, ionU32 _vertexCount, ionU32 _instanceCount, ionU32 _firstVertex, ionU32 _firstInstance);

    VkRenderPass CreateTexturedRenderPass(Texture* _texture, VkImageLayout _finalLayout);
    VkFramebuffer CreateTexturedFrameBuffer(VkRenderPass _renderPass, Texture* _texture);

    void StartRenderPass(VkRenderPass _renderPass, VkFramebuffer _frameBuffer, VkCommandBuffer _commandBuffer, const ionVector<VkClearValue, RenderCoreAllocator, GetAllocator>& _clearValues, const VkRect2D& _renderArea);
    void EndRenderPass(VkCommandBuffer _commandBuffer);

    void SetScissor(VkCommandBuffer _commandBuffer, const VkRect2D& _scissor);
    void SetViewport(VkCommandBuffer _commandBuffer, const VkViewport& _viewport);
    void SetScissor(VkCommandBuffer _commandBuffer, ionS32 _leftX, ionS32 _bottomY, ionU32 _width, ionU32 _height);
    void SetViewport(VkCommandBuffer _commandBuffer, ionFloat _leftX, ionFloat _bottomY, ionFloat _width, ionFloat _height, ionFloat _minDepth, ionFloat _maxDepth);

    void DestroyFrameBuffer(VkFramebuffer _frameBuffer);
    void DestroyRenderPass(VkRenderPass _renderPass);

    //////////////////////////////////////////////////////////////////////////
    // Getter
    VkDevice& GetDevice() { return m_vkDevice; }
    const VkDevice& GetDevice() const { return m_vkDevice; }

    VkQueue& GetGraphicQueue() { return m_vkGraphicsQueue; }
    const VkQueue& GetGraphicQueue() const { return m_vkGraphicsQueue; }

    VkQueue& GetPresentQueue() { return m_vkPresentQueue; }
    const VkQueue& GetPresentQueue() const { return m_vkPresentQueue; }

    const VkCommandPool& GetCommandPool() const { return m_vkCommandPool; }

    VkFormat GetDepthFormat() const { return m_vkDepthFormat; }

    ionU32 GetCurrentSwapIndex() const { return m_currentSwapIndex; }

    ionS32 GetGraphicFamilyIndex() const { return m_vkGraphicsFamilyIndex; }
    ionS32 GetPresentFamilyIndex() const { return m_vkPresentFamilyIndex; }

    const GPU& GetGPU() const { return m_vkGPU; }
    const VkSampleCountFlagBits& GetSampleCount() const { return m_vkSampleCount; }
    ionBool GetUsesSuperSampling() const { return m_vkSupersampling; }
    const VkPipelineCache& GetPipelineCache() const { return m_vkPipelineCache; }

    const VertexCacheHandler& GetJointCacheHandler() const { return m_jointCacheHandler; }

    ionU32 GetWidth() const { return m_width; }
    ionU32 GetHeight() const { return m_height; }

    ionBool CreateRenderPass(VkRenderPass& _vkRenderPass, EFramebufferLoad _load = EFramebufferLoad_Clear);
    ionBool CreateFrameBuffers(VkRenderPass _vkRenderPass, ionVector<VkFramebuffer, RenderCoreAllocator, GetAllocator>& _vkFrameBuffers);
    void    DestroyFrameBuffers(ionVector<VkFramebuffer, RenderCoreAllocator, GetAllocator>& _vkFrameBuffers);

private:
    RenderCore(const RenderCore& _Orig) = delete;
    RenderCore& operator = (const RenderCore&) = delete;

private:
    // Utility functions
    VkSurfaceFormatKHR SelectSurfaceFormat(ionVector<VkSurfaceFormatKHR, GPUAllocator, GPU::GetAllocator>& _vkFormats) const;
    VkPresentModeKHR SelectPresentMode(ionVector<VkPresentModeKHR, GPUAllocator, GPU::GetAllocator>& _vkModes) const;
    VkExtent2D SelectSurfaceExtent(VkSurfaceCapabilitiesKHR& _vkCaps, ionU32& _width, ionU32& _height) const;
    VkFormat SelectSupportedFormat(VkPhysicalDevice _vkPhysicalDevice, VkFormat* _vkFormats, ionU32 _vkNumFormats, VkImageTiling _vkTiling, VkFormatFeatureFlags _vkFeatures) const;

    // They are in order to call
    // No matter about the "destroy" functions. NOTE: destroy functions are added for the Vulkan type worth to have own destroy function, because more complex of a single call
    ionBool CreateInstance(ionBool _enableValidationLayer);
    ionBool CreatePresentationSurface(HINSTANCE _instance, HWND _handle);
    ionBool CreatePhysicalDevice();
    ionBool CreateLogicalDeviceAndQueues();
    ionBool CreateSemaphores();
    ionBool CreateCommandPool();
    ionBool CreateCommandBuffer();
    ionBool CreateSwapChain();
    void    DestroySwapChain();
    ionBool CreateRenderTargets();
    void    DestroyRenderTargets();
    ionBool CreatePipelineCache();

    void    CreateDebugReport(const VkDebugReportCallbackCreateInfoEXT& createInfo);
    void    DestroyDebugReport();
	void    CreateDebugUtilMessanger(const VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void    DestroyDebugUtilMessanger();


private:
    HINSTANCE                   m_instance;
    HWND                        m_window;
    GPU                         m_vkGPU;                  //  access through this component to get value such m_vkPhysicalDevice
    VkDevice                    m_vkDevice;
    VkSurfaceKHR                m_vkSurface;
    VkInstance                  m_vkInstance;
    VkQueue                     m_vkGraphicsQueue;
    VkQueue                     m_vkPresentQueue;
    VkCommandPool               m_vkCommandPool;
    VkSwapchainKHR              m_vkSwapchain;
    VkFormat                    m_vkSwapchainFormat;
    VkExtent2D                  m_vkSwapchainExtent;
    VkPresentModeKHR            m_vkPresentMode;
    VkSampleCountFlagBits       m_vkSampleCount;
    VkFormat                    m_vkDepthFormat;
    VkPipelineCache             m_vkPipelineCache;
    VkDebugReportCallbackEXT    m_vkDebugCallback;
    VkSemaphore                 m_vkAcquiringSemaphore;
    VkSemaphore                 m_vkCompletedSemaphore;

	// report message only
	VkDebugUtilsMessengerEXT	m_debugUtilsMessenger;

    GpuMemoryAllocation       m_vkMSAAAllocation;
    VkImage                     m_vkMSAAImage;
    VkImageView                 m_vkMSAAImageView;

    GpuMemoryAllocation       m_vkDepthAllocation;
    VkImage                     m_vkDepthImage;
    VkImageView                 m_vkDepthImageView;

    GpuMemoryAllocation       m_vkDepthStencilAllocation;
    VkImage                     m_vkDepthStencilImage;
    VkImageView                 m_vkDepthStencilImageView;

    ionVector<VkCommandBuffer, RenderCoreAllocator, GetAllocator>  m_vkCommandBuffers;
    ionVector<VkFence, RenderCoreAllocator, GetAllocator>          m_vkCommandBufferFences;
    ionVector<VkImage, RenderCoreAllocator, GetAllocator>          m_vkSwapchainImages;
    ionVector<VkImageView, RenderCoreAllocator, GetAllocator>      m_vkSwapchainViews;

    VertexCacheHandler          m_jointCacheHandler;

    ionU64                      m_stateBits;
    ionU64                      m_microSeconds;

    ionU64                      m_counter;
    ionU32                      m_swapChainImageCount;
    ionU32                      m_currentSwapIndex;

    ionU32                      m_width;
    ionU32                      m_height;

    ionS32                      m_vkGraphicsFamilyIndex;
    ionS32                      m_vkPresentFamilyIndex;

    ionU32                      m_vkCurrentSwapIndex;
    
    ionBool                     m_vkSupersampling;
    ionBool                     m_vkFullScreen;
    ionBool                     m_vkValidationEnabled;
    ionBool                     m_vkSupportBlit;
};

ION_NAMESPACE_END