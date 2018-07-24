#include "RenderCore.h"

#include "RenderDefs.h"
#include "GPUMemoryManager.h"

#include "../Texture/Texture.h"
#include "../Texture/TextureManager.h"

#include "../Shader/ShaderProgramManager.h"

#include "../Material/Material.h"

#include "StagingBufferManager.h"
#include "VertexCacheManager.h"

#include "RenderState.h"

#define VK_NAME                     "Ion"
#define VK_LUNAR_VALIDATION_LAYER   "VK_LAYER_LUNARG_standard_validation"

VK_ALLOCATOR_USING_NAMESPACE
EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

VkSurfaceFormatKHR RenderCore::SelectSurfaceFormat(eosVector(VkSurfaceFormatKHR)& _vkFormats) const
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

VkPresentModeKHR RenderCore::SelectPresentMode(eosVector(VkPresentModeKHR)& _vkModes) const
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


VkExtent2D RenderCore::SelectSurfaceExtent(VkSurfaceCapabilitiesKHR& _vkCaps, ionU32& _width, ionU32& _height) const
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

    _width = extent.width;
    _height = extent.height;

    return extent;
}

VkFormat RenderCore::SelectSupportedFormat(VkPhysicalDevice _vkPhysicalDevice, VkFormat* _vkFormats, ionU32 _vkNumFormats, VkImageTiling _vkTiling, VkFormatFeatureFlags _vkFeatures) const
{
    for (ionU32 i = 0; i < _vkNumFormats; ++i)
    {
        VkFormat format = _vkFormats[i];

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(_vkPhysicalDevice, format, &props);

        if (_vkTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _vkFeatures) == _vkFeatures)
        {
            return format;
        }
        else if (_vkTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _vkFeatures) == _vkFeatures)
        {
            return format;
        }
    }

    ionAssertReturnValue(false, "Failed to find a supported format.", VK_FORMAT_UNDEFINED);
}


//////////////////////////////////////////////////////////////////////////

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) 
{
    eosString prefix("");
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        prefix += "[ERROR]";
    };
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        prefix += "[WARNING]";
    };
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) 
    {
        prefix += "[DEBUG]";
    }

    std::cerr << prefix << "[" << layerPrefix << "] - " << msg << std::endl;
    return VK_FALSE;
}

void RenderCore::CreateDebugReport()
{
    if (m_vkValidationEnabled)
    {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
        createInfo.pfnCallback = debugCallback;

        auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugReportCallbackEXT");
        if (func != nullptr)
        {
            func(m_vkInstance, &createInfo, vkMemory, &m_vkDebugCallback);
        }
    }
}

void RenderCore::DestroyDebugReport()
{
    if (m_vkValidationEnabled)
    {
        auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugReportCallbackEXT");
        if (func != nullptr)
        {
            func(m_vkInstance, m_vkDebugCallback, vkMemory);
        }
    }
}

ionBool RenderCore::CreateInstance(ionBool _enableValidationLayer)
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

ionBool RenderCore::CreatePresentationSurface(HINSTANCE _instance, HWND _handle)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.hinstance = _instance;
    createInfo.hwnd = _handle;

    return (vkCreateWin32SurfaceKHR(m_vkInstance, &createInfo, vkMemory, &m_vkSurface) == VK_SUCCESS);
}

ionBool RenderCore::CreatePhysicalDevice()
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

ionBool RenderCore::CreateLogicalDeviceAndQueues()
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

ionBool RenderCore::CreateSemaphores()
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(m_vkDevice, &createInfo, vkMemory, &m_vkAcquiringSemaphore);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for locking!", false);

    result = vkCreateSemaphore(m_vkDevice, &createInfo, vkMemory, &m_vkCompletedSemaphore);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for unlocking!", false);
    
    return true;
}


ionBool RenderCore::CreateCommandPool()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = m_vkGraphicsFamilyIndex;

    VkResult result = vkCreateCommandPool(m_vkDevice, &createInfo, vkMemory, &m_vkCommandPool);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create query pool!", false);

    return true;
}

ionBool RenderCore::CreateSwapChain()
{
    VkSwapchainKHR oldSwapchain = m_vkSwapchain;

    VkSurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(m_vkGPU.m_vkSurfaceFormats);
    VkPresentModeKHR presentMode = SelectPresentMode(m_vkGPU.m_vkPresentModes);
    VkExtent2D extent = SelectSurfaceExtent(m_vkGPU.m_vkSurfaceCaps, m_width, m_height);

    ionU32 imageCountDesired = m_vkGPU.m_vkSurfaceCaps.minImageCount + 1;
    if ((m_vkGPU.m_vkSurfaceCaps.maxImageCount > 0) && (imageCountDesired > m_vkGPU.m_vkSurfaceCaps.maxImageCount))
    {
        imageCountDesired = m_vkGPU.m_vkSurfaceCaps.maxImageCount;
    }

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = m_vkSurface;
    info.minImageCount = imageCountDesired;
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
    info.oldSwapchain = oldSwapchain;

    VkResult result = vkCreateSwapchainKHR(m_vkDevice, &info, vkMemory, &m_vkSwapchain);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create swap chain!", false);

    // If an existing swap chain is re-created, destroy the old swap chain
    // This also cleans up all the presentable images
    if (oldSwapchain != VK_NULL_HANDLE)
    {
        for (uint32_t i = 0; i < m_swapChainImageCount; i++)
        {
            vkDestroyImageView(m_vkDevice, m_vkSwapchainViews[i], vkMemory);
        }
        vkDestroySwapchainKHR(m_vkDevice, oldSwapchain, vkMemory);
    }

    m_vkSwapchainFormat = surfaceFormat.format;
    m_vkPresentMode = presentMode;
    m_vkSwapchainExtent = extent;

    result = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &m_swapChainImageCount, nullptr);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot get swap chain image!", false);
    ionAssertReturnValue(m_swapChainImageCount > 0, "vkGetSwapchainImagesKHR returned a zero image count.", false);

    m_vkSwapchainImages.resize(m_swapChainImageCount, VK_NULL_HANDLE);

    result = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &m_swapChainImageCount, m_vkSwapchainImages.data());
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot get swap chain image!", false);
    ionAssertReturnValue(m_swapChainImageCount > 0, "vkGetSwapchainImagesKHR returned a zero image count.", false);

    m_vkSwapchainViews.resize(m_swapChainImageCount, VK_NULL_HANDLE);

    for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
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

void RenderCore::DestroySwapChain()
{
    for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
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
        m_vkSwapchain = VK_NULL_HANDLE;
    }
}

ionBool RenderCore::CreateCommandBuffer()
{
    m_vkCommandBuffers.resize(m_swapChainImageCount, VK_NULL_HANDLE);
    {
        VkCommandBufferAllocateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        createInfo.commandPool = m_vkCommandPool;
        createInfo.commandBufferCount = m_swapChainImageCount;

        VkResult result = vkAllocateCommandBuffers(m_vkDevice, &createInfo, m_vkCommandBuffers.data());
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create command buffer!", false);
    }

    m_vkCommandBufferFences.resize(m_swapChainImageCount, VK_NULL_HANDLE);
    {
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
        {
            VkResult result = vkCreateFence(m_vkDevice, &createInfo, vkMemory, &m_vkCommandBufferFences[i]);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create fence!", false);
        }
    }

    return true;
}

ionBool RenderCore::CreateRenderTargets()
{
    // Determine samples before creating depth
    VkImageFormatProperties formatProperties = {};
    vkGetPhysicalDeviceImageFormatProperties(m_vkGPU.m_vkPhysicalDevice, m_vkSwapchainFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 0, &formatProperties);

    const ETextureSamplesPerBit samples = ionTextureManger().GetMainSamplePerBits();
    if (samples >= ETextureSamplesPerBit_64 && (formatProperties.sampleCounts & VK_SAMPLE_COUNT_64_BIT))
    {
        m_vkSampleCount = VK_SAMPLE_COUNT_64_BIT;
    }
    else if (samples >= ETextureSamplesPerBit_32 && (formatProperties.sampleCounts & VK_SAMPLE_COUNT_32_BIT))
    {
        m_vkSampleCount = VK_SAMPLE_COUNT_32_BIT;
    } 
    else if (samples >= ETextureSamplesPerBit_16 && (formatProperties.sampleCounts & VK_SAMPLE_COUNT_16_BIT))
    {
        m_vkSampleCount = VK_SAMPLE_COUNT_16_BIT;
    }
    else if (samples >= ETextureSamplesPerBit_8 && (formatProperties.sampleCounts & VK_SAMPLE_COUNT_8_BIT))
    {
        m_vkSampleCount = VK_SAMPLE_COUNT_8_BIT;
    }
    else if (samples >= ETextureSamplesPerBit_4 && (formatProperties.sampleCounts & VK_SAMPLE_COUNT_4_BIT))
    {
        m_vkSampleCount = VK_SAMPLE_COUNT_4_BIT;
    }
    else if (samples >= ETextureSamplesPerBit_2 && (formatProperties.sampleCounts & VK_SAMPLE_COUNT_2_BIT))
    {
        m_vkSampleCount = VK_SAMPLE_COUNT_2_BIT;
    }
    else
    {
        m_vkSampleCount = VK_SAMPLE_COUNT_1_BIT;
    }

    VkFormat formats[] = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM };
    m_vkDepthFormat = SelectSupportedFormat(m_vkGPU.m_vkPhysicalDevice, formats, 5, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    
    ionTextureManger().SetDepthFormat(m_vkDepthFormat);

    if (m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT) 
    {
        m_vkSupersampling = m_vkGPU.m_vkPhysicalDevFeatures.sampleRateShading == VK_TRUE;

        // MSAA
        {
            VkImageCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            createInfo.imageType = VK_IMAGE_TYPE_2D;
            createInfo.format = m_vkSwapchainFormat;
            createInfo.extent.width = m_vkSwapchainExtent.width;
            createInfo.extent.height = m_vkSwapchainExtent.height;
            createInfo.extent.depth = 1;
            createInfo.mipLevels = 1;
            createInfo.arrayLayers = 1;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.samples = m_vkSampleCount;
            createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            createInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
            createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkResult result = vkCreateImage(m_vkDevice, &createInfo, vkMemory, &m_vkMSAAImage);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image!", false);

            VkMemoryRequirements memoryRequirements = {};
            vkGetImageMemoryRequirements(m_vkDevice, m_vkMSAAImage, &memoryRequirements);

            {
                vkGpuMemoryCreateInfo createInfo = {};
                createInfo.m_size = memoryRequirements.size;
                createInfo.m_align = memoryRequirements.alignment;
                createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
                createInfo.m_usage = EMemoryUsage_GPU;
                createInfo.m_type = EGpuMemoryType_ImageOptimal;

                m_vkMSAAAllocation = ionGPUMemoryManager().Alloc(createInfo);
                ionAssertReturnValue(m_vkMSAAAllocation.m_result == VK_SUCCESS, "Cannot Allocate memory!", false);

                result = vkBindImageMemory(m_vkDevice, m_vkMSAAImage, m_vkMSAAAllocation.m_memory, m_vkMSAAAllocation.m_offset);
                ionAssertReturnValue(result == VK_SUCCESS, "Cannot bind the image memory!", false);
            }

            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.format = m_vkSwapchainFormat;
            viewInfo.image = m_vkMSAAImage;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

            result = vkCreateImageView(m_vkDevice, &viewInfo, vkMemory, &m_vkMSAAImageView);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image view!", false);
        }
        
        // DEPTH
        {
            VkImageCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            createInfo.imageType = VK_IMAGE_TYPE_2D;
            createInfo.format = m_vkDepthFormat;
            createInfo.extent.width = m_vkSwapchainExtent.width;
            createInfo.extent.height = m_vkSwapchainExtent.height;
            createInfo.extent.depth = 1;
            createInfo.mipLevels = 1;
            createInfo.arrayLayers = 1;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.samples = m_vkSampleCount;
            createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            createInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkResult result = vkCreateImage(m_vkDevice, &createInfo, vkMemory, &m_vkDepthImage);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image!", false);

            VkMemoryRequirements memoryRequirements = {};
            vkGetImageMemoryRequirements(m_vkDevice, m_vkDepthImage, &memoryRequirements);

            {
                vkGpuMemoryCreateInfo createInfo = {};
                createInfo.m_size = memoryRequirements.size;
                createInfo.m_align = memoryRequirements.alignment;
                createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
                createInfo.m_usage = EMemoryUsage_GPU;
                createInfo.m_type = EGpuMemoryType_ImageOptimal;

                m_vkDepthAllocation = ionGPUMemoryManager().Alloc(createInfo);
                ionAssertReturnValue(m_vkDepthAllocation.m_result == VK_SUCCESS, "Cannot Allocate memory!", false);

                result = vkBindImageMemory(m_vkDevice, m_vkDepthImage, m_vkDepthAllocation.m_memory, m_vkDepthAllocation.m_offset);
                ionAssertReturnValue(result == VK_SUCCESS, "Cannot bind the image memory!", false);
            }

            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.format = m_vkDepthFormat;
            viewInfo.image = m_vkDepthImage;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

            result = vkCreateImageView(m_vkDevice, &viewInfo, vkMemory, &m_vkDepthImageView);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image view!", false);
        }
    }

    // DEPTH STENCIL
    {
        VkImageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType = VK_IMAGE_TYPE_2D;
        createInfo.format = m_vkDepthFormat;
        createInfo.extent.width = m_vkSwapchainExtent.width;
        createInfo.extent.height = m_vkSwapchainExtent.height;
        createInfo.extent.depth = 1;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = 1;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        createInfo.flags = 0;

        VkResult result = vkCreateImage(m_vkDevice, &createInfo, vkMemory, &m_vkDepthStencilImage);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image!", false);

        VkMemoryRequirements memoryRequirements = {};
        vkGetImageMemoryRequirements(m_vkDevice, m_vkDepthStencilImage, &memoryRequirements);

        {
            vkGpuMemoryCreateInfo createInfo = {};
            createInfo.m_size = memoryRequirements.size;
            createInfo.m_align = memoryRequirements.alignment;
            createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
            createInfo.m_usage = EMemoryUsage_GPU;
            createInfo.m_type = EGpuMemoryType_ImageOptimal;

            m_vkDepthStencilAllocation = ionGPUMemoryManager().Alloc(createInfo);
            ionAssertReturnValue(m_vkDepthStencilAllocation.m_result == VK_SUCCESS, "Cannot Allocate memory!", false);

            result = vkBindImageMemory(m_vkDevice, m_vkDepthStencilImage, m_vkDepthStencilAllocation.m_memory, m_vkDepthStencilAllocation.m_offset);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot bind the image memory!", false);
        }

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.format = m_vkDepthFormat;
        viewInfo.image = m_vkDepthStencilImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

        result = vkCreateImageView(m_vkDevice, &viewInfo, vkMemory, &m_vkDepthStencilImageView);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create image view!", false);
    }

    return true;
}

void RenderCore::DestroyRenderTargets()
{
    if (m_vkMSAAImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_vkDevice, m_vkMSAAImageView, vkMemory);

        if (m_vkMSAAImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(m_vkDevice, m_vkMSAAImage, vkMemory);
            
            ionGPUMemoryManager().Free(m_vkMSAAAllocation);
            m_vkMSAAAllocation = vkGpuMemoryAllocation();

            m_vkMSAAImage = VK_NULL_HANDLE;
        }

        m_vkMSAAImageView = VK_NULL_HANDLE;
    }

    if (m_vkDepthImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_vkDevice, m_vkDepthImageView, vkMemory);

        if (m_vkDepthImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(m_vkDevice, m_vkDepthImage, vkMemory);

            ionGPUMemoryManager().Free(m_vkDepthAllocation);
            m_vkDepthAllocation = vkGpuMemoryAllocation();

            m_vkDepthImage = VK_NULL_HANDLE;
        }

        m_vkDepthImageView = VK_NULL_HANDLE;
    }

    if (m_vkDepthStencilImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_vkDevice, m_vkDepthStencilImageView, vkMemory);

        if (m_vkDepthStencilImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(m_vkDevice, m_vkDepthStencilImage, vkMemory);

            ionGPUMemoryManager().Free(m_vkDepthStencilAllocation);
            m_vkDepthStencilAllocation = vkGpuMemoryAllocation();

            m_vkDepthStencilImage = VK_NULL_HANDLE;
        }

        m_vkDepthStencilImageView = VK_NULL_HANDLE;
    }
}

ionBool RenderCore::CreateRenderPass()
{
    const ionBool resolve = m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT;

    if (resolve)
    {
        VkAttachmentDescription attachments[4];
        memset(attachments, 0, sizeof(attachments));

        VkAttachmentDescription& colorAttachment = attachments[0];
        colorAttachment.format = m_vkSwapchainFormat;
        colorAttachment.samples = m_vkSampleCount;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription& colorResolver = attachments[1];
        colorResolver.format = m_vkSwapchainFormat;
        colorResolver.samples = VK_SAMPLE_COUNT_1_BIT;
        colorResolver.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorResolver.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorResolver.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorResolver.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorResolver.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorResolver.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription& depthAttachment = attachments[2];
        depthAttachment.format = m_vkDepthFormat;
        depthAttachment.samples = m_vkSampleCount;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription& depthResolver = attachments[3];
        depthResolver.format = m_vkDepthFormat;
        depthResolver.samples = VK_SAMPLE_COUNT_1_BIT;
        depthResolver.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthResolver.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthResolver.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthResolver.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthResolver.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthResolver.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorRef = {};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthRef = {};
        depthRef.attachment = 2;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference resolveRef = {};
        resolveRef.attachment = 1;
        resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = &depthRef;
        subpass.pResolveAttachments = &resolveRef;

        eosVector(VkSubpassDependency) dependencies;
        dependencies.resize(2);

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 4;
        renderPassCreateInfo.pAttachments = attachments;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = static_cast<ionU32>(dependencies.size());
        renderPassCreateInfo.pDependencies = dependencies.data();

        VkResult result = vkCreateRenderPass(m_vkDevice, &renderPassCreateInfo, vkMemory, &m_vkRenderPass);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create render pass!", false);
    }
    else
    {
        VkAttachmentDescription attachments[2];
        memset(attachments, 0, sizeof(attachments));

        VkAttachmentDescription& colorResolver = attachments[0];
        colorResolver.format = m_vkSwapchainFormat;
        colorResolver.samples = VK_SAMPLE_COUNT_1_BIT;
        colorResolver.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorResolver.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorResolver.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorResolver.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorResolver.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorResolver.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription& depthResolver = attachments[1];
        depthResolver.format = m_vkDepthFormat;
        depthResolver.samples = VK_SAMPLE_COUNT_1_BIT;
        depthResolver.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthResolver.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthResolver.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthResolver.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthResolver.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthResolver.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorRef = {};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthRef = {};
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = &depthRef;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = nullptr;
        subpass.pResolveAttachments = nullptr;

        eosVector(VkSubpassDependency) dependencies;
        dependencies.resize(2);

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 2;
        renderPassCreateInfo.pAttachments = attachments;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = static_cast<ionU32>(dependencies.size());
        renderPassCreateInfo.pDependencies = dependencies.data();

        VkResult result = vkCreateRenderPass(m_vkDevice, &renderPassCreateInfo, vkMemory, &m_vkRenderPass);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create render pass!", false);
    }

    return true;
}

ionBool RenderCore::CreatePipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    VkResult result = vkCreatePipelineCache(m_vkDevice, &pipelineCacheCreateInfo, vkMemory, &m_vkPipelineCache);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create pipeline cache!", false);

    return true;
}

ionBool RenderCore::CreateFrameBuffers()
{
    VkImageView attachments[4] = {};

    const ionBool resolve = m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT;
    if (resolve)
    {
        attachments[0] = m_vkMSAAImageView;
        attachments[2] = m_vkDepthImageView;
        attachments[3] = m_vkDepthStencilImageView;
    }
    else
    {
        attachments[1] = m_vkDepthStencilImageView;
    }

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = m_vkRenderPass;
    frameBufferCreateInfo.attachmentCount = resolve ? 4 : 2;
    frameBufferCreateInfo.pAttachments = attachments;
    frameBufferCreateInfo.width = m_width;
    frameBufferCreateInfo.height = m_height;
    frameBufferCreateInfo.layers = 1;

    m_vkFrameBuffers.resize(m_swapChainImageCount, VK_NULL_HANDLE);
    for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
    {
        if (resolve)
        {
            attachments[1] = m_vkSwapchainViews[i];
        }
        else
        {
            attachments[0] = m_vkSwapchainViews[i];
        }

        VkResult result = vkCreateFramebuffer(m_vkDevice, &frameBufferCreateInfo, vkMemory, &m_vkFrameBuffers[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Impossible to create frame buffer.", false);
    }

    return true;
}

void RenderCore::DestroyFrameBuffers()
{
    for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
    {
        vkDestroyFramebuffer(m_vkDevice, m_vkFrameBuffers[i], vkMemory);
    }
    m_vkFrameBuffers.clear();
}

RenderCore::RenderCore() : m_instance(nullptr), m_window(nullptr)
{
    Clear();
}


RenderCore::~RenderCore()
{
    m_vkFrameBuffers.clear();
    m_vkSwapchainViews.clear();
    m_vkSwapchainImages.clear();
    m_vkCommandBufferFences.clear();
    m_vkCommandBuffers.clear();
}

void RenderCore::Clear()
{
    // core context
    m_jointCacheHandler = 0;
    m_vkGPU = GPU();
    m_vkDevice = VK_NULL_HANDLE;
    m_vkGraphicsFamilyIndex = -1;
    m_vkPresentFamilyIndex = -1;
    m_vkGraphicsQueue = VK_NULL_HANDLE;
    m_vkPresentQueue = VK_NULL_HANDLE;
    m_vkDepthFormat = VK_FORMAT_UNDEFINED;
    m_vkRenderPass = VK_NULL_HANDLE;
    m_vkPipelineCache = VK_NULL_HANDLE;
    m_vkSampleCount = VK_SAMPLE_COUNT_1_BIT;
    m_vkSupersampling = false;


    // core
    m_counter = 0;
    m_width = 800;
    m_height = 600;
    m_vkInstance = VK_NULL_HANDLE;
    m_vkFullScreen = false;
    m_vkSupportBlit = false;
    m_vkSwapchain = VK_NULL_HANDLE;
    m_vkSwapchainFormat = VK_FORMAT_UNDEFINED;
    m_vkCurrentSwapIndex = 0;
    m_vkMSAAImage = VK_NULL_HANDLE;
    m_vkMSAAImageView = VK_NULL_HANDLE;
    m_vkDepthImage = VK_NULL_HANDLE;
    m_vkDepthImageView = VK_NULL_HANDLE;
    m_vkCommandPool = VK_NULL_HANDLE;
    m_vkSurface = VK_NULL_HANDLE;
    m_vkPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    m_vkValidationEnabled = false;

    m_vkAcquiringSemaphore = VK_NULL_HANDLE;
    m_vkCompletedSemaphore = VK_NULL_HANDLE;

    m_vkSwapchainExtent.width = m_width;
    m_vkSwapchainExtent.height = m_height;
    m_vkSwapchainImages.clear();
    m_vkSwapchainViews.clear();
    m_vkFrameBuffers.clear();
    m_currentSwapIndex = 0;

    m_vkCommandBuffers.clear();
    m_vkCommandBufferFences.clear();
}

ionBool RenderCore::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    Clear();

    m_instance = _instance;
    m_window = _handle;

    m_width = _width;
    m_height = _height;
    m_vkFullScreen = _fullScreen;

    if (!CreateInstance(_enableValidationLayer))
    {
        return false;
    }

    CreateDebugReport();

    if (!CreatePresentationSurface(m_instance, m_window))
    {
        return false;
    }

    if (!CreatePhysicalDevice())
    {
        return false;
    }


    m_vkSupportBlit = true;
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(m_vkGPU.m_vkPhysicalDevice, m_vkSwapchainFormat, &formatProps);
    if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
    {
        m_vkSupportBlit = false;
    }

    vkGetPhysicalDeviceFormatProperties(m_vkGPU.m_vkPhysicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
    if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
    {
        m_vkSupportBlit = false;
    }


    if (!CreateLogicalDeviceAndQueues())
    {
        return false;
    }

    ionGPUMemoryManager().Init(m_vkGPU.m_vkPhysicalDevice, m_vkDevice, _vkDeviceLocalSize, _vkHostVisibleSize, m_vkGPU.m_vkPhysicalDeviceProps.limits.bufferImageGranularity);

    ionStagingBufferManager().Init(_vkStagingBufferSize, m_vkDevice, m_vkGraphicsQueue, m_vkGraphicsFamilyIndex);

    if (!CreateSemaphores())
    {
        return false;
    }

    if (!CreateCommandPool())
    {
        return false;
    }

    if (!CreateSwapChain())
    {
        return false;
    }

    if(!CreateCommandBuffer())
    {
        return false;
    }

    if (!CreateRenderTargets())
    {
        return false;
    }

    if (!CreateRenderPass())
    {
        return false;
    }

    if (!CreatePipelineCache())
    {
        return false;
    }
 
    if (!CreateFrameBuffers())
    {
        return false;
    }

    ionShaderProgramManager().Init(m_vkDevice);

    ionVertexCacheManager().Init(m_vkDevice, m_vkGPU.m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment);

    ionTextureManger().Init(m_vkDevice, ETextureSamplesPerBit_16);

    return true;
}

void RenderCore::Shutdown()
{
    vkDeviceWaitIdle(m_vkDevice);

    ionTextureManger().Shutdown();

    ionVertexCacheManager().Shutdown();

    ionShaderProgramManager().Shutdown();

    DestroyFrameBuffers();

    if (m_vkPipelineCache != VK_NULL_HANDLE)
    {
        vkDestroyPipelineCache(m_vkDevice, m_vkPipelineCache, vkMemory);
    }

    if (m_vkRenderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(m_vkDevice, m_vkRenderPass, vkMemory);
    }

    DestroyRenderTargets();

    DestroySwapChain();

    for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
    {
        if (m_vkCommandBufferFences[i] != VK_NULL_HANDLE)
        {
            vkDestroyFence(m_vkDevice, m_vkCommandBufferFences[i], vkMemory);
        }
    }

    vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, static_cast<ionU32>(m_vkCommandBuffers.size()), m_vkCommandBuffers.data());

    if (m_vkCommandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, vkMemory);
    }


    if (m_vkAcquiringSemaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_vkDevice, m_vkAcquiringSemaphore, vkMemory);
    }
    if (m_vkCompletedSemaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_vkDevice, m_vkCompletedSemaphore, vkMemory);
    }
    
    ionStagingBufferManager().Shutdown();
    ionGPUMemoryManager().Shutdown();

    // Create physical device has not destruction because is more as "selected" 

    if (m_vkSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, vkMemory);
    }

    if (m_vkDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_vkDevice, vkMemory);
    }

    DestroyDebugReport();

    if (m_vkInstance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_vkInstance, vkMemory);
    }
}

void RenderCore::Recreate()
{
    vkDeviceWaitIdle(m_vkDevice);

    for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
    {
        vkDestroyFramebuffer(m_vkDevice, m_vkFrameBuffers[i], vkMemory);
    }

    vkDestroyPipelineCache(m_vkDevice, m_vkPipelineCache, vkMemory);

    if (m_vkRenderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(m_vkDevice, m_vkRenderPass, vkMemory);
    }

    DestroyRenderTargets();

    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkGPU.m_vkPhysicalDevice, m_vkSurface, &m_vkGPU.m_vkSurfaceCaps);
    ionAssertReturnVoid(result == VK_SUCCESS, "Device capabilities changed and not supported!");

    VkBool32 supportsPresent = VK_FALSE;
    result = vkGetPhysicalDeviceSurfaceSupportKHR(m_vkGPU.m_vkPhysicalDevice, m_vkPresentFamilyIndex, m_vkSurface, &supportsPresent);
    ionAssertReturnVoid(result == VK_SUCCESS, "Device surface changed and not supported!");
    ionAssertReturnVoid(supportsPresent == VK_TRUE, "New surface does not support present");

    for (ionU32 i = 0; i < m_swapChainImageCount; ++i)
    {
        if (m_vkCommandBufferFences[i] != VK_NULL_HANDLE)
        {
            vkDestroyFence(m_vkDevice, m_vkCommandBufferFences[i], vkMemory);
        }
    }

    vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, static_cast<ionU32>(m_vkCommandBuffers.size()), m_vkCommandBuffers.data());

    CreateSwapChain();
    CreateCommandBuffer();
    CreateRenderTargets();
    CreateRenderPass();
    CreatePipelineCache();
    CreateFrameBuffers();

    m_vkSupportBlit = true;
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(m_vkGPU.m_vkPhysicalDevice, m_vkSwapchainFormat, &formatProps);
    if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
    {
        m_vkSupportBlit = false;
    }

    vkGetPhysicalDeviceFormatProperties(m_vkGPU.m_vkPhysicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
    if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
    {
        m_vkSupportBlit = false;
    }

    ionShaderProgramManager().Restart();
}


ionBool RenderCore::StartFrame()
{
    VkResult result = vkAcquireNextImageKHR(m_vkDevice, m_vkSwapchain, UINT64_MAX, m_vkAcquiringSemaphore, VK_NULL_HANDLE, &m_currentSwapIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) 
    {
        Recreate();
        return false;
    }
    ionAssertReturnValue(result == VK_SUCCESS || result  == VK_SUBOPTIMAL_KHR, "vkAcquireNextImageKHR failed!", false);

    result = vkWaitForFences(m_vkDevice, 1, &m_vkCommandBufferFences[m_currentSwapIndex], VK_TRUE, UINT64_MAX);
    ionAssertReturnValue(result == VK_SUCCESS, "Wait for fences failed!", false);

    result = vkResetFences(m_vkDevice, 1, &m_vkCommandBufferFences[m_currentSwapIndex]);
    ionAssertReturnValue(result == VK_SUCCESS, "Reset fences failed!", false);

    ionStagingBufferManager().Submit();
    ionShaderProgramManager().StartFrame();
    
    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentSwapIndex];

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    ionAssertReturnValue(result == VK_SUCCESS, "vkBeginCommandBuffer failed!", false);


    return true;
}

void RenderCore::StartRenderPass(ionFloat _clearDepthValue, ionU8 _clearStencilValue, ionFloat _clearRed, ionFloat _clearGreen, ionFloat _clearBlue)
{
    ionAssertReturnVoid(_clearDepthValue >= 0.0f && _clearDepthValue <= 1.0f, "Clear depth must be between 0 and 1!");
    ionAssertReturnVoid(_clearRed >= 0.0f && _clearRed <= 1.0f, "Clear red must be between 0 and 1!");
    ionAssertReturnVoid(_clearGreen >= 0.0f && _clearGreen <= 1.0f, "Clear green must be between 0 and 1!");
    ionAssertReturnVoid(_clearBlue >= 0.0f && _clearBlue <= 1.0f, "Clear blue must be between 0 and 1!");

    const ionBool resolve = m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT;

    VkClearValue clearValues[3];
    if (resolve)
    {
        clearValues[0].color = { { _clearRed, _clearGreen, _clearBlue, 1.0f } };
        clearValues[1].color = { { _clearRed, _clearGreen, _clearBlue, 1.0f } };
        clearValues[2].depthStencil = { _clearDepthValue, _clearStencilValue };
    }
    else {
        clearValues[0].color = { { _clearRed, _clearGreen, _clearBlue, 1.0f } };
        clearValues[1].depthStencil = { _clearDepthValue, _clearStencilValue };
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_vkRenderPass;
    renderPassBeginInfo.framebuffer = m_vkFrameBuffers[m_currentSwapIndex];
    renderPassBeginInfo.renderArea.extent = m_vkSwapchainExtent;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;    
    renderPassBeginInfo.clearValueCount = resolve ? 3 : 2;
    renderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(m_vkCommandBuffers[m_currentSwapIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderCore::EndRenderPass()
{
    vkCmdEndRenderPass(m_vkCommandBuffers[m_currentSwapIndex]);
}

void RenderCore::EndFrame()
{
    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentSwapIndex];


    // Transition our swap image to present.
    // Do this instead of having the renderpass do the transition
    // so we can take advantage of the general layout to avoid 
    // additional image barriers.
    /*
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_vkSwapchainImages[m_currentSwapIndex];
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    */

    VkResult result = vkEndCommandBuffer(commandBuffer);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkEndCommandBuffer failed!");

    ionShaderProgramManager().EndFrame();

    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_vkAcquiringSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_vkCompletedSemaphore;
    submitInfo.pWaitDstStageMask = &dstStageMask;

    result = vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, m_vkCommandBufferFences[m_currentSwapIndex]);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkQueueSubmit failed!");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_vkCompletedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_vkSwapchain;
    presentInfo.pImageIndices = &m_currentSwapIndex;

    result = vkQueuePresentKHR(m_vkPresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        Recreate();
    }
    else if (result != VK_SUCCESS)
    {
        ionAssertReturnVoid(false, "vkQueuePresentKHR failed!");
    }

    ++m_counter;
}

void RenderCore::SetDefaultState()
{
    m_stateBits = 0;

    SetState(0);

    SetScissor(0, 0, m_width, m_height);
}

void RenderCore::SetState(ionU64 _stateBits)
{
    m_stateBits = _stateBits | (m_stateBits & ERasterization_DepthTest_Mask);

    // if the current rendered draw view is a mirror should set the 
    // m_stateBits |= ERasterization_View_Specular;
}

void RenderCore::SetScissor(ionS32 _leftX, ionS32 _bottomY, ionU32 _width, ionU32 _height)
{
    VkRect2D scissor;
    scissor.offset.x = _leftX;
    scissor.offset.y = _bottomY;
    scissor.extent.width = _width;
    scissor.extent.height = _height;
    vkCmdSetScissor(m_vkCommandBuffers[m_currentSwapIndex], 0, 1, &scissor);
}

void RenderCore::SetViewport(ionFloat _leftX, ionFloat _bottomY, ionFloat _width, ionFloat _height, ionFloat _minDepth, ionFloat _maxDepth)
{
    VkViewport viewport;
    viewport.x = _leftX;
    viewport.y = _bottomY;
    viewport.width = _width;
    viewport.height = _height;
    viewport.minDepth = _minDepth;
    viewport.maxDepth = _maxDepth;
    vkCmdSetViewport(m_vkCommandBuffers[m_currentSwapIndex], 0, 1, &viewport);
}

void RenderCore::SetPolygonOffset(ionFloat _scale, ionFloat _bias)
{
    vkCmdSetDepthBias(m_vkCommandBuffers[m_currentSwapIndex], _bias, 0.0f, _scale);
}

void RenderCore::SetDepthBoundsTest(ionFloat _zMin, ionFloat _zMax)
{
    if (!m_vkGPU.m_vkPhysicalDevFeatures.depthBounds || _zMin > _zMax)
    {
        return;
    }

    if (_zMin == 0.0f && _zMax == 0.0f)
    {
        m_stateBits = m_stateBits & ~ERasterization_DepthTest_Mask;
    }
    else 
    {
        m_stateBits |= ERasterization_DepthTest_Mask;
        vkCmdSetDepthBounds(m_vkCommandBuffers[m_currentSwapIndex], _zMin, _zMax);
    }
}

// to use after the end frame (outside the "current" render command buffer)
void RenderCore::CopyFrameBuffer(Texture* _texture)
{
    VkCommandBuffer commandBuffer = CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    if (BeginCustomCommandBuffer(commandBuffer))
    {
        VkImage srcImage = m_vkSwapchainImages[m_currentSwapIndex];

        VkImageMemoryBarrier dstBarrier = {};
        {
            dstBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            dstBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            dstBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            dstBarrier.image = _texture->GetImage();

            dstBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            dstBarrier.subresourceRange.baseMipLevel = 0;
            dstBarrier.subresourceRange.levelCount = 1;
            dstBarrier.subresourceRange.baseArrayLayer = 0;
            dstBarrier.subresourceRange.layerCount = 1;

            dstBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            dstBarrier.srcAccessMask = 0;
            dstBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);
        }

        VkImageMemoryBarrier srcBarrier = {};
        {
            srcBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            srcBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            srcBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            srcBarrier.image = srcImage;

            srcBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            srcBarrier.subresourceRange.baseMipLevel = 0;
            srcBarrier.subresourceRange.levelCount = 1;
            srcBarrier.subresourceRange.baseArrayLayer = 0;
            srcBarrier.subresourceRange.layerCount = 1;

            srcBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            srcBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            srcBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            srcBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcBarrier);
        }

        if (m_vkSupportBlit)
        {
            VkOffset3D srcBlitSize;
            srcBlitSize.x = m_vkSwapchainExtent.width;
            srcBlitSize.y = m_vkSwapchainExtent.height;
            srcBlitSize.z = 1;

            VkOffset3D dstBlitSize;
            dstBlitSize.x = _texture->GetWidth();
            dstBlitSize.y = _texture->GetHeight();
            dstBlitSize.z = 1;

            VkImageBlit imageBlitRegion{};
            imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlitRegion.srcSubresource.layerCount = 1;
            imageBlitRegion.srcOffsets[1] = srcBlitSize;

            imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlitRegion.dstSubresource.layerCount = 1;
            imageBlitRegion.dstOffsets[1] = dstBlitSize;

            vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlitRegion, VK_FILTER_NEAREST);
        }
        else
        {
            VkImageCopy imageCopyRegion{};
            imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.srcSubresource.layerCount = 1;
            imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.dstSubresource.layerCount = 1;
            imageCopyRegion.extent.width = m_vkSwapchainExtent.width;
            imageCopyRegion.extent.height = m_vkSwapchainExtent.height;
            imageCopyRegion.extent.depth = 1;

            vkCmdCopyImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1, &imageCopyRegion);
        }

        {
            dstBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            dstBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            dstBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            dstBarrier.image = _texture->GetImage();

            dstBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            dstBarrier.subresourceRange.baseMipLevel = 0;
            dstBarrier.subresourceRange.levelCount = 1;
            dstBarrier.subresourceRange.baseArrayLayer = 0;
            dstBarrier.subresourceRange.layerCount = 1;

            dstBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            dstBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            dstBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);
        }

        {
            srcBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            srcBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            srcBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            srcBarrier.image = srcImage;

            srcBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            srcBarrier.subresourceRange.baseMipLevel = 0;
            srcBarrier.subresourceRange.levelCount = 1;
            srcBarrier.subresourceRange.baseArrayLayer = 0;
            srcBarrier.subresourceRange.layerCount = 1;

            srcBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            srcBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            srcBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            srcBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcBarrier);
        }

        EndCustomCommandBuffer(commandBuffer);
        FlushCustomCommandBuffer(commandBuffer);
    }
}

/*
void RenderCore::CopyFrameBuffer(Texture* _texture, ionS32 _width, ionS32 _height)
{
    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentSwapIndex];

    vkCmdEndRenderPass(commandBuffer);

    VkImageMemoryBarrier dstBarrier = {};
    dstBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    dstBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.image = _texture->GetImage();
    dstBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    dstBarrier.subresourceRange.baseMipLevel = 0;
    dstBarrier.subresourceRange.levelCount = 1;
    dstBarrier.subresourceRange.baseArrayLayer = 0;
    dstBarrier.subresourceRange.layerCount = 1;

    // Pre copy transitions
    {
        // Transition the color dest texture so we can transfer to it.
        dstBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dstBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dstBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);
    }

    // Perform the blit/copy
    {
        VkImageBlit region = {};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.baseArrayLayer = 0;
        region.srcSubresource.mipLevel = 0;
        region.srcSubresource.layerCount = 1;
        region.srcOffsets[1] = { _width, _height, 1 };

        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.baseArrayLayer = 0;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.layerCount = 1;
        region.dstOffsets[1] = { _texture->GetWidth(), _texture->GetHeight(), 1 };

        vkCmdBlitImage(commandBuffer, m_vkSwapchainImages[m_currentSwapIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_NEAREST);
    }

    // Post copy transitions
    {
        // Transition the color dest texture so we can transfer to it.
        dstBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dstBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier( commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);
    }

    const ionBool resolve = m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT;

    VkClearValue clearValues[3];
    if (resolve)
    {
        clearValues[0].color = { { 1.0f, 1.0f, 1.0f, 1.0f } };
        clearValues[1].color = { { 1.0f, 1.0f, 1.0f, 1.0f } };
        clearValues[2].depthStencil = { 1.0f, 0 };
    }
    else {
        clearValues[0].color = { { 1.0f, 1.0f, 1.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_vkRenderPass;
    renderPassBeginInfo.framebuffer = m_vkFrameBuffers[m_currentSwapIndex];
    renderPassBeginInfo.renderArea.extent = m_vkSwapchainExtent;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.clearValueCount = resolve ? 3 : 2;
    renderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}
*/

void RenderCore::Draw(const DrawSurface& _surface)
{
    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentSwapIndex];
    
    ionShaderProgramManager().SetRenderParamMatrix(ION_MODEL_MATRIX_PARAM_HASH, &_surface.m_modelMatrix[0]);
    ionShaderProgramManager().SetRenderParamMatrix(ION_VIEW_MATRIX_PARAM_HASH, &_surface.m_viewMatrix[0]);
    ionShaderProgramManager().SetRenderParamMatrix(ION_PROJ_MATRIX_PARAM_HASH, &_surface.m_projectionMatrix[0]);

    ionS32  vertexShaderIndex = -1;
    ionS32  fragmentShaderIndex = -1;
    ionS32  tessellationControlIndex = -1;
    ionS32  tessellationEvaluationIndex = -1;
    ionS32  geometryIndex = -1;
    ionBool useJoint = false;
    ionBool useSkinning = false;

    _surface.m_material->GetShaders(vertexShaderIndex, fragmentShaderIndex, tessellationControlIndex, tessellationEvaluationIndex, geometryIndex, useJoint, useSkinning);

    const ionS32 shaderProgramIndex = 
        ionShaderProgramManager().FindProgram(_surface.m_material->GetShaderProgramName(), _surface.m_material->GetVertexLayout(), _surface.m_material->GetConstantsShaders(),
        vertexShaderIndex, fragmentShaderIndex, tessellationControlIndex, tessellationEvaluationIndex, geometryIndex, useJoint, useSkinning);

    ionShaderProgramManager().BindProgram(shaderProgramIndex);
    ionShaderProgramManager().CommitCurrent(*this, m_stateBits, commandBuffer);

    ionSize indexOffset = 0;
    ionSize vertexOffset = 0;
    IndexBuffer indexBuffer;
    if (ionVertexCacheManager().GetIndexBuffer(_surface.m_indexCache, &indexBuffer))
    {
        const VkBuffer buffer = indexBuffer.GetObject();
        const VkDeviceSize offset = indexBuffer.GetOffset();
        indexOffset = offset;
        vkCmdBindIndexBuffer(commandBuffer, buffer, offset, VK_INDEX_TYPE_UINT32);
    }

    VertexBuffer vertexBufer;
    if (ionVertexCacheManager().GetVertexBuffer(_surface.m_vertexCache, &vertexBufer))
    {
        const VkBuffer buffer = vertexBufer.GetObject();
        const VkDeviceSize offset = vertexBufer.GetOffset();
        vertexOffset = offset;
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &offset);
    }

    vkCmdDrawIndexed(commandBuffer, _surface.m_indexCount, 1, _surface.m_indexStart /*(indexOffset >> 1)*/, 0 /*vertexOffset / sizeof(Vertex)*/, 0);
}

VkCommandBuffer RenderCore::CreateCustomCommandBuffer(VkCommandBufferLevel _level)
{
    VkCommandBuffer cmdBuffer;

    VkCommandBufferAllocateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    createInfo.commandPool = m_vkCommandPool;
    createInfo.commandBufferCount = 1;
    createInfo.level = _level;

    VkResult result = vkAllocateCommandBuffers(m_vkDevice, &createInfo, &cmdBuffer);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create command buffer!", cmdBuffer);

    return cmdBuffer;
}

ionBool RenderCore::BeginCustomCommandBuffer(VkCommandBuffer _commandBuffer)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult result = vkBeginCommandBuffer(_commandBuffer, &commandBufferBeginInfo);
    ionAssertReturnValue(result == VK_SUCCESS, "vkBeginCommandBuffer failed!", false);

    return true;
}

void RenderCore::EndCustomCommandBuffer(VkCommandBuffer _commandBuffer)
{
    if (_commandBuffer == VK_NULL_HANDLE)
    {
        return;
    }

    VkResult result = vkEndCommandBuffer(_commandBuffer);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkEndCommandBuffer failed!");
}

void RenderCore::FlushCustomCommandBuffer(VkCommandBuffer _commandBuffer)
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;

    VkResult result = vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkQueueSubmit failed!");

    result = vkQueueWaitIdle(m_vkGraphicsQueue);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkQueueWaitIdle failed!");

    vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, 1, &_commandBuffer);
}


ION_NAMESPACE_END