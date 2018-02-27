#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "GPU.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderManager
{
public:
    ION_NO_INLINE static ION_DLL RenderManager& Instance();

    ionBool    Init(HINSTANCE _instance, HWND _handle, ionBool _enableValidationLayer);
    void       Shutdown();

private:
    RenderManager();
    ~RenderManager();

    RenderManager(const RenderManager& _Orig) = delete; 
    RenderManager& operator = (const RenderManager&) = delete;

private:
    // They are in order to call
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
    ionBool CreateFrameBuffers();
    void    DestroyFrameBuffers();

private:
    GPU                 m_vkGPU;                  //  access through this component to get value such m_vkPhysicalDevice
    VkDevice            m_vkDevice;
    VkSurfaceKHR        m_vkSurface;
    VkInstance          m_vkInstance;
    ionS32			    m_vkGraphicsFamilyIndex;
    ionS32			    m_vkPresentFamilyIndex;

    VkQueue			    m_vkGraphicsQueue;
    VkQueue			    m_vkPresentQueue;
    // array of max images


    ionBool             m_vkValidationEnabled;
};


#define IonRenderManager() RenderManager::Instance()


ION_NAMESPACE_END