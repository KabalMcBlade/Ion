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

        createInfo.enabledLayerCount = (ionU32)enabledLayers.size();
        createInfo.ppEnabledLayerNames = enabledLayers.data();
    }

    createInfo.enabledExtensionCount = (ionU32)enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkResult result = vkCreateInstance(&createInfo, vkMemory, &m_vkInstance);

    return (result == VK_SUCCESS);
}

ionBool RenderContext::CreatePresentationSurface(HINSTANCE _instance, HWND _handle)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.hinstance = _instance;
    createInfo.hwnd = _handle;

    return (vkCreateWin32SurfaceKHR(m_vkInstance, &createInfo, vkMemory, &m_vkSurface) == VK_SUCCESS);
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
                 graphicsIdx = (ionS32)j;
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
             vkGetPhysicalDeviceSurfaceSupportKHR(gpu.m_vkPhysicalDevice, (ionS32)j, m_vkSurface, &supportsPresent);
             if (supportsPresent) 
             {
                 presentIdx = (ionS32)j;
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
            VkDeviceQueueCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = m_vkGraphicsFamilyIndex;
            createInfo.queueCount = 1;
            createInfo.pQueuePriorities = &priority;
            createInfo.pNext = nullptr;

            deviceQueueInfo.push_back(createInfo);
        }

        {
            VkDeviceQueueCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = m_vkPresentFamilyIndex;
            createInfo.queueCount = 1;
            createInfo.pQueuePriorities = &priority;
            createInfo.pNext = nullptr;

            deviceQueueInfo.push_back(createInfo);
        }
    }
    else
    {
        VkDeviceQueueCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.queueFamilyIndex = m_vkGraphicsFamilyIndex;        // Just one queue! because they come from the same family and Vulkan do not need 2 identical queue!
        createInfo.queueCount = 1;
        createInfo.pQueuePriorities = &priority;
        createInfo.pNext = nullptr;

        deviceQueueInfo.push_back(createInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.textureCompressionBC = VK_TRUE;
    deviceFeatures.imageCubeArray = VK_TRUE;
    deviceFeatures.depthClamp = VK_TRUE;
    deviceFeatures.depthBiasClamp = VK_TRUE;
    deviceFeatures.depthBounds = m_vkGPU.m_vkPhysicalDevFeatures.depthBounds;
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = (ionU32)deviceQueueInfo.size();
    createInfo.pQueueCreateInfos = deviceQueueInfo.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = (ionU32)enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    if (m_vkValidationEnabled)
    {
        eosVector(const char*) enabledLayers;
        enabledLayers.push_back(VK_LUNAR_VALIDATION_LAYER);

        createInfo.enabledLayerCount = (ionU32)enabledLayers.size();
        createInfo.ppEnabledLayerNames = enabledLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(m_vkGPU.m_vkPhysicalDevice, &createInfo, vkMemory, &m_vkDevice);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create logical device!", false);

    vkGetDeviceQueue(m_vkDevice, m_vkGraphicsFamilyIndex, 0, &m_vkGraphicsQueue);
    vkGetDeviceQueue(m_vkDevice, m_vkPresentFamilyIndex, 0, &m_vkPresentQueue);

    return true;
}

ionBool RenderContext::CreateSemaphores()
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i)
    {
        VkResult result = vkCreateSemaphore(m_vkDevice, &createInfo, vkMemory, &m_vkAcquiringSemaphores[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for locking!", false);

        result = vkCreateSemaphore(m_vkDevice, &createInfo, vkMemory, &m_vkCompletedSemaphores[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for unlocking!", false);
    }
    return true;
}

ionBool RenderContext::CreateQueryPool()
{
    VkQueryPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    createInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    createInfo.queryCount = ION_QUERY_COUNT;

    for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i) 
    {
        VkResult result = vkCreateQueryPool(m_vkDevice, &createInfo, vkMemory, &m_vkQueryPools[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create query pool!", false);
    }

    return true;
}

ionBool RenderContext::CreateCommandPool()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = m_vkGraphicsFamilyIndex;

    VkResult result = vkCreateCommandPool(m_vkDevice, &createInfo, vkMemory, &m_vkCommandPool);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create query pool!", false);

    return true;
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

ionBool RenderContext::CreatePipelineCache()
{
    return false;   // TO DO
}

void RenderContext::DestroyPipelineCache()
{

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
    m_vkCommandPool(VK_NULL_HANDLE),
    m_vkGraphicsFamilyIndex(-1),
    m_vkPresentFamilyIndex(-1),
    m_vkValidationEnabled(false)
{
    m_vkAcquiringSemaphores.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkCompletedSemaphores.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkQueryPools.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
}

RenderContext::~RenderContext()
{
    m_vkQueryPools.clear();
    m_vkCompletedSemaphores.clear();
    m_vkAcquiringSemaphores.clear();
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

    if (!CreateSemaphores())
    {
        return false;
    }

    if (!CreateQueryPool())
    {
        return false;
    }

    return true;
}

void RenderContext::Shutdown()
{
    if (m_vkCommandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, vkMemory);
    }

    for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i)
    {
        if (m_vkQueryPools[i] != VK_NULL_HANDLE)
        {
            vkDestroyQueryPool(m_vkDevice, m_vkQueryPools[i], vkMemory);
        }
    }

    for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i)
    {
        if (m_vkAcquiringSemaphores[i] != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(m_vkDevice, m_vkAcquiringSemaphores[i], vkMemory);
        }
        if (m_vkCompletedSemaphores[i] != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(m_vkDevice, m_vkCompletedSemaphores[i], vkMemory);
        }
    }

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