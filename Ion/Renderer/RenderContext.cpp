#include "RenderContext.h"

#include "RenderDefs.h"

#define VK_NAME                     "Ion"
#define VK_LUNAR_VALIDATION_LAYER   "VK_LAYER_LUNARG_standard_validation"

VK_ALLOCATOR_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

VkSurfaceFormatKHR RenderContext::SelectSurfaceFormat(eosVector(VkSurfaceFormatKHR)& _vkFormats) const
{
    VkSurfaceFormatKHR result;

    if (_vkFormats.size() == 1 && _vkFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        result.format = VK_FORMAT_B8G8R8A8_UNORM;
        result.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        return result;
    }

    for (ionSize i = 0; i < _vkFormats.size(); ++i)
    {
        VkSurfaceFormatKHR& surfaceFormat = _vkFormats[i];
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return surfaceFormat;
        }
    }

    return _vkFormats[0];
}

VkPresentModeKHR RenderContext::SelectPresentMode(eosVector(VkPresentModeKHR)& _vkModes) const
{
    for (ionSize i = 0; i < _vkModes.size(); i++)
    {
        if (_vkModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) // I WANT THIS FOR TRIPLE BUFFERING!, BUT IF NOT POSSIBLE....
        {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }

        if ((_vkModes[i] != VK_PRESENT_MODE_MAILBOX_KHR) && (_vkModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D RenderContext::SelectSurfaceExtent(VkSurfaceCapabilitiesKHR& _vkCaps, ionU32 _width, ionU32 _height) const
{
    VkExtent2D extent;

    if (_vkCaps.currentExtent.width != std::numeric_limits<ionU32>::max())
    {
        extent = _vkCaps.currentExtent;
    }
    else 
    {
        extent.width = std::max(_vkCaps.minImageExtent.width, std::min(_vkCaps.maxImageExtent.width, _width));
        extent.height = std::max(_vkCaps.minImageExtent.height, std::min(_vkCaps.maxImageExtent.height, _height));
    }

    return extent;
}




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
    {
        VkCommandBufferAllocateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        createInfo.commandPool = m_vkCommandPool;
        createInfo.commandBufferCount = ION_RENDER_TRIPLE_BUFFER;

        VkResult result = vkAllocateCommandBuffers(m_vkDevice, &createInfo, m_vkCommandBuffers.data());
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create command buffer!", false);
    }

    {
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i)
        {
            VkResult result = vkCreateFence(m_vkDevice, &createInfo, vkMemory, &m_vkCommandBufferFences[i]);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create fence!", false);
        }
    }

    return true;
}

ionBool RenderContext::CreateSwapChain(ionU32 _width, ionU32 _height, ionBool _fullScreen)
{
    VkSurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(m_vkGPU.m_vkSurfaceFormats);
    VkPresentModeKHR presentMode = SelectPresentMode(m_vkGPU.m_vkPresentModes);
    VkExtent2D extent = SelectSurfaceExtent(m_vkGPU.m_vkSurfaceCaps, _width, _height);

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = m_vkSurface;
    info.minImageCount = ION_RENDER_TRIPLE_BUFFER;
    info.imageFormat = surfaceFormat.format;
    info.imageColorSpace = surfaceFormat.colorSpace;
    info.imageExtent = extent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (m_vkGraphicsFamilyIndex != m_vkPresentFamilyIndex)
    {
        ionU32 indices[] = { (ionU32)m_vkGraphicsFamilyIndex, (ionU32)m_vkPresentFamilyIndex };

        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = indices;
    }
    else 
    {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = presentMode;
    info.clipped = VK_TRUE;

    VkResult result = vkCreateSwapchainKHR(m_vkDevice, &info, vkMemory, &m_vkSwapchain);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create swap chain!", false);

    m_vkSwapchainFormat = surfaceFormat.format;
    m_vkPresentMode = presentMode;
    m_vkSwapchainExtent = extent;
    m_vkFullScreen = _fullScreen;

    ionU32 numImages = 0;
    result = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &numImages, nullptr);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot get swap chain image!", false);
    ionAssertReturnValue(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.", false);

    result = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &numImages, m_vkSwapchainImages.data());
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot get swap chain image!", false);
    ionAssertReturnValue(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.", false);

    // Triple buffer so I've 3 images
    for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_vkSwapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_vkSwapchainFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.flags = 0;

        result = vkCreateImageView(m_vkDevice, &createInfo, vkMemory, &m_vkSwapchainViews[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image!", false);
    }

    return true;
}

void RenderContext::DestroySwapChain()
{
    for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i)
    {
        if (m_vkSwapchainViews[i] != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_vkDevice, m_vkSwapchainViews[i], vkMemory);
        }
    }
    m_vkSwapchainViews.clear();

    if (m_vkSwapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_vkDevice, m_vkSwapchain, vkMemory);
    }
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
    m_vkSwapchain(VK_NULL_HANDLE),
    //m_vkSwapchainFormat(VK_NULL_HANDLE),
    //m_vkPresentMode(VK_NULL_HANDLE),
    //m_vkSwapchainExtent(VK_NULL_HANDLE),
    m_vkGraphicsFamilyIndex(-1),
    m_vkPresentFamilyIndex(-1),
    m_vkFullScreen(false),
    m_vkValidationEnabled(false)
{
    m_vkAcquiringSemaphores.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkCompletedSemaphores.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkQueryPools.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkCommandBuffers.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkCommandBufferFences.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkSwapchainImages.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
    m_vkSwapchainViews.resize(ION_RENDER_TRIPLE_BUFFER, VK_NULL_HANDLE);
}

RenderContext::~RenderContext()
{
    m_vkSwapchainViews.clear();
    m_vkSwapchainImages.clear();
    m_vkCommandBufferFences.clear();
    m_vkCommandBuffers.clear();
    m_vkQueryPools.clear();
    m_vkCompletedSemaphores.clear();
    m_vkAcquiringSemaphores.clear();
}


ionBool RenderContext::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer)
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

    if (!CreateCommandPool())
    {
        return false;
    }

    if(!CreateCommandBuffer())
    {
        return false;
    }

    if (!CreateSwapChain(_width, _height, _fullScreen))
    {
        return false;
    }
    return true;
}

void RenderContext::Shutdown()
{
    DestroySwapChain();

    for (ionU32 i = 0; i < ION_RENDER_TRIPLE_BUFFER; ++i)
    {
        if (m_vkCommandBufferFences[i] != VK_NULL_HANDLE)
        {
            vkDestroyFence(m_vkDevice, m_vkCommandBufferFences[i], vkMemory);
        }
    }

    // Deallocate command buffer?

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