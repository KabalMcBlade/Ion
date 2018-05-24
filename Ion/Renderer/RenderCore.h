#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/CoreDefs.h"

#include "../Texture/TextureCommon.h"

#include "RenderCommon.h"
#include "DrawRenderCommon.h"

#include "GPU.h"

EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class Texture;
struct DrawSurface;

class ION_DLL RenderCore final
{
public:
    RenderCore();
    ~RenderCore();

    ionBool Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize);
    void    Shutdown();
	void	Recreate();
    void    Clear();

    void    BlockingSwapBuffers();
    ionBool StartFrame();
    void    EndFrame();
    void    BindTexture(ionS32 _index, Texture* _image);
    void    SetDefaultState();
    void    SetState(ionU64 _stateBits);
    void    SetScissor(ionS32 _leftX, ionS32 _bottomY, ionS32 _width, ionS32 _height);
    void    SetViewport(ionS32 _leftX, ionS32 _bottomY, ionS32 _width, ionS32 _height);
    void    SetPolygonOffset(ionFloat _scale, ionFloat _bias);
    void    SetDepthBoundsTest(ionFloat _zMin, ionFloat _zMax);
    void    SetClear(ionBool _color, ionBool _depth, ionBool _stencil, ionU8 _stencilValue, ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a);
    void    CopyFrameBuffer(Texture* _texture, ionS32 _x, ionS32 _y, ionS32 _textureWidth, ionS32 _textureHeight);
    void    SetColor(const eosString& _param, ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a);
    void    Draw(const DrawSurface& _surface);

    void    Execute(const ionU32 _commandCount, const eosVector(RenderCommand)& _renderCommands);


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

    const VertexCacheHandler& GetJointCacheHandler() const { return m_jointCacheHandler; }

    const Texture* GetTextureParam(ionS32 _imageIndex) const { ionAssertReturnValue(_imageIndex >= 0 && _imageIndex < m_textureParams.size(), "Index out of bound", nullptr); return m_textureParams[_imageIndex]; }
    
    ionU32 GetWidth() const { return m_width; }
    ionU32 GetHeight() const { return m_height; }

private:
    RenderCore(const RenderCore& _Orig) = delete;
    RenderCore& operator = (const RenderCore&) = delete;

private:
    // Utility functions
    VkSurfaceFormatKHR SelectSurfaceFormat(eosVector(VkSurfaceFormatKHR)& _vkFormats) const;
    VkPresentModeKHR SelectPresentMode(eosVector(VkPresentModeKHR)& _vkModes) const;
    VkExtent2D SelectSurfaceExtent(VkSurfaceCapabilitiesKHR& _vkCaps, ionU32& _width, ionU32& _height) const;
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
    ionBool CreateSwapChain();
    void    DestroySwapChain();
    ionBool CreateRenderTargets();
    void    DestroyRenderTargets();
    ionBool CreateRenderPass();
    ionBool CreatePipelineCache();
    ionBool CreateFrameBuffers();
    void    DestroyFrameBuffers();
    void    CreateDebugReport();
    void    DestroyDebugReport();

private:
    HINSTANCE                   m_instance;
    HWND                        m_window;
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
    VkDebugReportCallbackEXT    m_vkDebugCallback;

    eosVector(VkSemaphore)	    m_vkAcquiringSemaphores;
    eosVector(VkSemaphore)	    m_vkCompletedSemaphores;
    eosVector(VkQueryPool)	    m_vkQueryPools;
    eosVector(VkCommandBuffer)	m_vkCommandBuffers;
    eosVector(VkFence)			m_vkCommandBufferFences;
    eosVector(ionBool)          m_vkCommandBufferRecorded;
    eosVector(VkImage)			m_vkSwapchainImages;
    eosVector(VkImageView)		m_vkSwapchainViews;
    eosVector(VkFramebuffer)	m_vkFrameBuffers;
    eosVector(Texture*)         m_textureParams;

    eosVector(ionU32)           m_queryIndex;
    eosVector(eosVector(ionU64))m_queryResults;

    VertexCacheHandler          m_jointCacheHandler;

    ionU64				        m_stateBits;
    ionU64                      m_microSeconds;

    ionU64						m_counter;
    ionU32						m_currentFrameData;
    ionU32						m_currentSwapIndex;

    ionU32                      m_width;
    ionU32                      m_height;

    ionS32			            m_vkGraphicsFamilyIndex;
    ionS32			            m_vkPresentFamilyIndex;

    ionU32                      m_vkCurrentSwapIndex;
    
    ionBool						m_vkSupersampling;
    ionBool                     m_vkFullScreen;
    ionBool                     m_vkValidationEnabled;
};

ION_NAMESPACE_END