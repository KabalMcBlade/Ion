#include "RenderCore.h"

#include "RenderDefs.h"

#include "../Texture/TextureOptions.h"
#include "../Texture/Texture.h"
#include "../Texture/TextureManager.h"

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


VkExtent2D RenderCore::SelectSurfaceExtent(VkSurfaceCapabilitiesKHR& _vkCaps, ionU32 _width, ionU32 _height) const
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

/*
    REWORK ON THIS ON THE NEW VULKAN DEVICE ALLOCATOR
*/

ionU32 FindMemoryType(VkPhysicalDevice _vkPhysicalDevice, ionU32 typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_vkPhysicalDevice, &memProperties);

    for (ionU32 i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    ionAssertReturnValue(false, "Failed to find a suitable memory format!", 0);
}


//////////////////////////////////////////////////////////////////////////

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

    for (ionU32 i = 0; i < m_vkRenderType; ++i)
    {
        VkResult result = vkCreateSemaphore(m_vkDevice, &createInfo, vkMemory, &m_vkAcquiringSemaphores[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for locking!", false);

        result = vkCreateSemaphore(m_vkDevice, &createInfo, vkMemory, &m_vkCompletedSemaphores[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create semaphore for unlocking!", false);
    }
    return true;
}

ionBool RenderCore::CreateQueryPool()
{
    VkQueryPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    createInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    createInfo.queryCount = ION_QUERY_COUNT;

    for (ionU32 i = 0; i < m_vkRenderType; ++i)
    {
        VkResult result = vkCreateQueryPool(m_vkDevice, &createInfo, vkMemory, &m_vkQueryPools[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create query pool!", false);
    }

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

ionBool RenderCore::CreateCommandBuffer()
{
    {
        VkCommandBufferAllocateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        createInfo.commandPool = m_vkCommandPool;
        createInfo.commandBufferCount = m_vkRenderType;

        VkResult result = vkAllocateCommandBuffers(m_vkDevice, &createInfo, m_vkCommandBuffers.data());
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create command buffer!", false);
    }

    {
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        for (ionU32 i = 0; i < m_vkRenderType; ++i)
        {
            VkResult result = vkCreateFence(m_vkDevice, &createInfo, vkMemory, &m_vkCommandBufferFences[i]);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create fence!", false);
        }
    }

    return true;
}

ionBool RenderCore::CreateSwapChain(ionU32 _width, ionU32 _height, ionBool _fullScreen)
{
    m_width = _width;
    m_height = _height;

    VkSurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(m_vkGPU.m_vkSurfaceFormats);
    VkPresentModeKHR presentMode = SelectPresentMode(m_vkGPU.m_vkPresentModes);
    VkExtent2D extent = SelectSurfaceExtent(m_vkGPU.m_vkSurfaceCaps, _width, _height);

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = m_vkSurface;
    info.minImageCount = m_vkRenderType;
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
    for (ionU32 i = 0; i < m_vkRenderType; ++i)
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
    for (ionU32 i = 0; i < m_vkRenderType; ++i)
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

    VkFormat formats[] = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    m_vkDepthFormat = SelectSupportedFormat(m_vkGPU.m_vkPhysicalDevice, formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    
    TextureOptions depthTextureOptions;
    depthTextureOptions.m_format = ETextureFormat_Depth;
    depthTextureOptions.m_width = m_width;
    depthTextureOptions.m_height = m_height;
    depthTextureOptions.m_numLevels = 1;
    depthTextureOptions.m_samples = static_cast<ETextureSamplesPerBit>(m_vkSampleCount);

    ionTextureManger().CreateTextureFromOptions(m_vkDevice, "_ION_ViewDepth", depthTextureOptions);

    if (m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT) 
    {
        m_vkSupersampling = m_vkGPU.m_vkPhysicalDevFeatures.sampleRateShading == VK_TRUE;

        VkImageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType = VK_IMAGE_TYPE_2D;
        createInfo.format = m_vkSwapchainFormat;
        createInfo.extent.width = m_vkSwapchainExtent.width;
        createInfo.extent.height = m_vkSwapchainExtent.height;
        createInfo.extent.depth = 1;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = 1;
        createInfo.samples = m_vkSampleCount;
        createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
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

            m_vkMSAAAllocation =  m_gpuAllocator.Alloc(createInfo);
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

        result = vkCreateImageView(m_vkDevice, &viewInfo, vkMemory, &m_vkMSAAImageView);
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
            
            m_gpuAllocator.Free(m_vkMSAAAllocation);
            m_vkMSAAAllocation = vkGpuMemoryAllocation();

            m_vkMSAAImage = VK_NULL_HANDLE;
        }

        m_vkMSAAImageView = VK_NULL_HANDLE;
    }
}

ionBool RenderCore::CreateRenderPass()
{
    VkAttachmentDescription attachments[3];
    memset(attachments, 0, sizeof(attachments));

    const ionBool resolve = m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT;

    VkAttachmentDescription& colorAttachment = attachments[0];
    colorAttachment.format = m_vkSwapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentDescription& depthAttachment = attachments[1];
    depthAttachment.format = m_vkDepthFormat;
    depthAttachment.samples = m_vkSampleCount;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription& resolveAttachment = attachments[2];
    resolveAttachment.format = m_vkSwapchainFormat;
    resolveAttachment.samples = m_vkSampleCount;
    resolveAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolveAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    resolveAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    resolveAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference colorRef = {};
    colorRef.attachment = resolve ? 2 : 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef = {};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference resolveRef = {};
    resolveRef.attachment = 0;
    resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;
    if (resolve) 
    {
        subpass.pResolveAttachments = &resolveRef;
    }

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = resolve ? 3 : 2;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 0;

    VkResult result = vkCreateRenderPass(m_vkDevice, &renderPassCreateInfo, vkMemory, &m_vkRenderPass);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create render pass!", false);

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
    VkImageView attachments[3] = {};

    // depth attachment is the same
    Texture* depthTexture = ionTextureManger().GetTexture("_ION_ViewDepth");

    ionAssertReturnValue(depthTexture != nullptr, "No view depth texture found.", false);

    attachments[1] = depthTexture->GetView();

    const ionBool resolve = m_vkSampleCount > VK_SAMPLE_COUNT_1_BIT;
    if (resolve)
    {
        attachments[2] = m_vkMSAAImageView;
    }

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = m_vkRenderPass;
    frameBufferCreateInfo.attachmentCount = resolve ? 3 : 2;
    frameBufferCreateInfo.pAttachments = attachments;
    frameBufferCreateInfo.width = m_width;
    frameBufferCreateInfo.height = m_height;
    frameBufferCreateInfo.layers = 1;

    for (ionU32 i = 0; i < m_vkRenderType; ++i)
    {
        attachments[0] = m_vkSwapchainViews[i];
        VkResult result = vkCreateFramebuffer(m_vkDevice, &frameBufferCreateInfo, vkMemory, &m_vkFrameBuffers[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Impossible to create frame buffer.", false);
    }

    return true;
}

void RenderCore::DestroyFrameBuffers()
{
    for (ionU32 i = 0; i < m_vkRenderType; ++i)
    {
        vkDestroyFramebuffer(m_vkDevice, m_vkFrameBuffers[i], vkMemory);
    }
    m_vkFrameBuffers.clear();
}

RenderCore::RenderCore() :
    m_vkDevice(VK_NULL_HANDLE),
    m_vkSurface(VK_NULL_HANDLE),
    m_vkInstance(VK_NULL_HANDLE),
    m_vkGraphicsQueue(VK_NULL_HANDLE),
    m_vkPresentQueue(VK_NULL_HANDLE),
    m_vkCommandPool(VK_NULL_HANDLE),
    m_vkSwapchain(VK_NULL_HANDLE),
    m_vkMSAAImage(VK_NULL_HANDLE),
    m_vkMSAAImageView(VK_NULL_HANDLE),
    m_vkRenderPass(VK_NULL_HANDLE),
    m_vkPipelineCache(VK_NULL_HANDLE),
    m_vkGraphicsFamilyIndex(-1),
    m_vkPresentFamilyIndex(-1),
    m_vkFullScreen(false),
    m_vkValidationEnabled(false)
{
}

RenderCore::~RenderCore()
{
    m_vkFrameBuffers.clear();
    m_vkSwapchainViews.clear();
    m_vkSwapchainImages.clear();
    m_vkCommandBufferFences.clear();
    m_vkCommandBuffers.clear();
    m_vkQueryPools.clear();
    m_vkCompletedSemaphores.clear();
    m_vkAcquiringSemaphores.clear();
}


ionBool RenderCore::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ERenderType _renderType)
{
    m_vkRenderType = _renderType;

    m_vkAcquiringSemaphores.resize(_renderType, VK_NULL_HANDLE);
    m_vkCompletedSemaphores.resize(_renderType, VK_NULL_HANDLE);
    m_vkQueryPools.resize(_renderType, VK_NULL_HANDLE);
    m_vkCommandBuffers.resize(_renderType, VK_NULL_HANDLE);
    m_vkCommandBufferFences.resize(_renderType, VK_NULL_HANDLE);
    m_vkSwapchainImages.resize(_renderType, VK_NULL_HANDLE);
    m_vkSwapchainViews.resize(_renderType, VK_NULL_HANDLE);
    m_vkFrameBuffers.resize(_renderType, VK_NULL_HANDLE);

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

    m_gpuAllocator.Init(m_vkGPU.m_vkPhysicalDevice, m_vkDevice, _vkDeviceLocalSize, _vkHostVisibleSize, m_vkGPU.m_vkPhysicalDeviceProps.limits.bufferImageGranularity);

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

    return true;
}

void RenderCore::Shutdown()
{
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

    for (ionU32 i = 0; i < m_vkRenderType; ++i)
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

    for (ionU32 i = 0; i < m_vkRenderType; ++i)
    {
        if (m_vkQueryPools[i] != VK_NULL_HANDLE)
        {
            vkDestroyQueryPool(m_vkDevice, m_vkQueryPools[i], vkMemory);
        }
    }

    for (ionU32 i = 0; i < m_vkRenderType; ++i)
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

    m_gpuAllocator.Shutdown();

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