#include "RenderContext.h"

#include "RenderDefs.h"

#define VK_NAME                     "Ion"
#define VK_LUNAR_VALIDATION_LAYER   "VK_LAYER_LUNARG_standard_validation"

VK_ALLOCATOR_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


ionBool RenderContext::CreateInstance(ionBool _enableValidationLayer)
{
    m_vkValidationEnabled = _enableValidationLayer;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = VK_NAME;
    appInfo.pEngineName = VK_NAME;
    appInfo.apiVersion = VK_API_VERSION_1_0;


    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.pApplicationInfo = &appInfo;

    eosVector(const char*) enabledExtensions;
    enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    if (m_vkValidationEnabled)
    {
        enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        eosVector(const char*) enabledLayers;
        enabledLayers.push_back(VK_LUNAR_VALIDATION_LAYER);

        ionU32 layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        eosVector(VkLayerProperties) layers;
        layers.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        bool found = false;
        for (ionU32 i = 0; i < layerCount; ++i)
        {
            if (std::strcmp(VK_LUNAR_VALIDATION_LAYER, layers[i].layerName) == 0)
            {
                found = true;
                break;
            }
        }
        ionAssertReturnValue(found, "Cannot find validation layer", false);
        // here break

        createInfo.enabledLayerCount = enabledLayers.size();
        createInfo.ppEnabledLayerNames = enabledLayers.data();
    }

    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkResult result = vkCreateInstance(&createInfo, vkMemory, &m_vkInstance);

    return (result == VK_SUCCESS);
}

ionBool RenderContext::CreatePresentationSurface(HINSTANCE _instance, HWND _handle)
{
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.hinstance = _instance;
    surfaceCreateInfo.hwnd = _handle;

    return (vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceCreateInfo, vkMemory, &m_vkSurface) == VK_SUCCESS);
}

ionBool RenderContext::CreatePhysicalDevice()
{
    ionU32 numDevices = 0;

    VkResult result = vkEnumeratePhysicalDevices(m_vkInstance, &numDevices, nullptr);
    ionAssertReturnValue(result == VK_SUCCESS, "vkEnumeratePhysicalDevices returned zero devices.!", false);
    ionAssertReturnValue(numDevices > 0, "vkEnumeratePhysicalDevices returned zero devices.", false);

    eosVector(VkPhysicalDevice) devices;
    devices.resize(numDevices);

    result = vkEnumeratePhysicalDevices(m_vkInstance, &numDevices, devices.data());
    ionAssertReturnValue(result == VK_SUCCESS, "vkEnumeratePhysicalDevices returned zero devices.!", false);
    ionAssertReturnValue(numDevices > 0, "vkEnumeratePhysicalDevices returned zero devices.", false);

    eosVector(GPU) gpuList;
    gpuList.resize(numDevices);
    for (ionU32 i = 0; i < numDevices; ++i)
    {
        gpuList[i].Set(m_vkInstance, m_vkSurface, devices[i]);
    }

    const ionSize& gpuSize = gpuList.size();
    for(ionSize i = 0; i < gpuSize; ++i)
    {
         const GPU& gpu = gpuList[i];

         ionS32 graphicsIdx = -1;
         ionS32 presentIdx = -1;

         //////////////////////////////////////////////////////////////////////////
         bool found = false;
         const ionSize& extensionPropsCount = gpu.m_vkExtensionProps.size();
         for (ionSize j = 0; j < extensionPropsCount; ++j)
         {
             if (std::strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, gpu.m_vkExtensionProps[j].extensionName) == 0)
             {
                 found = true;
                 break;
             }
         }
         if (!found)
         {
             continue;
         }

         if (gpu.m_vkSurfaceFormats.size() == 0)
         {
             continue;
         }

         if (gpu.m_vkPresentModes.size() == 0)
         {
             continue;
         }

         //////////////////////////////////////////////////////////////////////////
         const ionSize& queueFamilySize = gpu.m_vkQueueFamilyProps.size();
         for (ionSize j = 0; j < queueFamilySize; ++j)
         {
             const VkQueueFamilyProperties& props = gpu.m_vkQueueFamilyProps[j];

             if (props.queueCount == 0) 
             {
                 continue;
             }

             if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
             {
                 graphicsIdx = j;
                 break;
             }
         }

         for (ionSize j = 0; j < queueFamilySize; ++j)
         {
             const VkQueueFamilyProperties& props = gpu.m_vkQueueFamilyProps[j];

             if (props.queueCount == 0) 
             {
                 continue;
             }

             VkBool32 supportsPresent = VK_FALSE;
             vkGetPhysicalDeviceSurfaceSupportKHR(gpu.m_vkPhysicalDevice, j, m_vkSurface, &supportsPresent);
             if (supportsPresent) 
             {
                 presentIdx = j;
                 break;
             }
         }

         if (graphicsIdx > -1 && presentIdx > -1)
         {
             m_vkGPU = gpu;
             m_vkGraphicsFamilyIndex = graphicsIdx;
             m_vkPresentFamilyIndex = presentIdx;

             break;
         }
    }

    return (m_vkGraphicsFamilyIndex > -1 && m_vkPresentFamilyIndex > -1);
}

ionBool RenderContext::CreateLogicalDeviceAndQueues()
{
    const ionFloat priority = 1.0f;

    eosVector(const char*) enabledExtensions;
    enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);


    eosVector(VkDeviceQueueCreateInfo) deviceQueueInfo;

    if (m_vkGraphicsFamilyIndex != m_vkPresentFamilyIndex)
    {
        {
            VkDeviceQueueCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = m_vkGraphicsFamilyIndex;
            info.queueCount = 1;
            info.pQueuePriorities = &priority;
            info.pNext = nullptr;

            deviceQueueInfo.push_back(info);
        }

        {
            VkDeviceQueueCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = m_vkPresentFamilyIndex;
            info.queueCount = 1;
            info.pQueuePriorities = &priority;
            info.pNext = nullptr;

            deviceQueueInfo.push_back(info);
        }
    }
    else
    {
        VkDeviceQueueCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = m_vkGraphicsFamilyIndex;        // Just one queue! because they come from the same family and Vulkan do not need 2 identical queue!
        info.queueCount = 1;
        info.pQueuePriorities = &priority;
        info.pNext = nullptr;

        deviceQueueInfo.push_back(info);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.textureCompressionBC = VK_TRUE;
    deviceFeatures.imageCubeArray = VK_TRUE;
    deviceFeatures.depthClamp = VK_TRUE;
    deviceFeatures.depthBiasClamp = VK_TRUE;
    deviceFeatures.depthBounds = m_vkGPU.m_vkPhysicalDevFeatures.depthBounds;
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = deviceQueueInfo.size();
    info.pQueueCreateInfos = deviceQueueInfo.data();
    info.pEnabledFeatures = &deviceFeatures;
    info.enabledExtensionCount = enabledExtensions.size();
    info.ppEnabledExtensionNames = enabledExtensions.data();

    if (m_vkValidationEnabled)
    {
        eosVector(const char*) enabledLayers;
        enabledLayers.push_back(VK_LUNAR_VALIDATION_LAYER);

        info.enabledLayerCount = enabledLayers.size();
        info.ppEnabledLayerNames = enabledLayers.data();
    }
    else
    {
        info.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(m_vkGPU.m_vkPhysicalDevice, &info, vkMemory, &m_vkDevice);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create logical device!", false);

    vkGetDeviceQueue(m_vkDevice, m_vkGraphicsFamilyIndex, 0, &m_vkGraphicsQueue);
    vkGetDeviceQueue(m_vkDevice, m_vkPresentFamilyIndex, 0, &m_vkPresentQueue);

    return true;
}

ionBool RenderContext::CreateSemaphores()
{
    return false;   // TO DO
}

ionBool RenderContext::CreateQueryPool()
{
    return false;   // TO DO
}

ionBool RenderContext::CreateCommandPool()
{
    return false;   // TO DO
}

ionBool RenderContext::CreateCommandBuffer()
{
    return false;   // TO DO
}

ionBool RenderContext::CreateSwapChain()
{
    return false;   // TO DO
}

void RenderContext::DestroySwapChain()
{

}

ionBool RenderContext::CreateRenderTargets()
{
    return false;   // TO DO
}

void RenderContext::DestroyRenderTargets()
{

}

ionBool RenderContext::CreateRenderPass()
{
    return false;   // TO DO
}

ionBool RenderContext::CreateFrameBuffers()
{
    return false;   // TO DO
}

void RenderContext::DestroyFrameBuffers()
{

}


RenderContext::RenderContext() :
    m_vkDevice(VK_NULL_HANDLE),
    m_vkSurface(VK_NULL_HANDLE),
    m_vkInstance(VK_NULL_HANDLE),
    m_vkGraphicsQueue(VK_NULL_HANDLE),
    m_vkPresentQueue(VK_NULL_HANDLE),
    m_vkGraphicsFamilyIndex(-1),
    m_vkPresentFamilyIndex(-1),
    m_vkValidationEnabled(false)
{

}

RenderContext::~RenderContext()
{

}


ionBool RenderContext::Init(HINSTANCE _instance, HWND _handle, ionBool _enableValidationLayer)
{
    if (!CreateInstance(_enableValidationLayer))
    {
        return false;
    }

    if (!CreatePresentationSurface(_instance, _handle))
    {
        return false;
    }

    if (!CreatePhysicalDevice())
    {
        return false;
    }
    
    if (!CreateLogicalDeviceAndQueues())
    {
        return false;
    }

    return true;
}

void RenderContext::Shutdown()
{
    if (m_vkDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_vkDevice, vkMemory);
    }

    // Create physical device has not destruction because is more as "selected" 

    if (m_vkSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, vkMemory);
    }

    if (m_vkInstance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_vkInstance, vkMemory);
    }
}

ION_NAMESPACE_END