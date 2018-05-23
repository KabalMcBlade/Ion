#include "RenderCore.h"

#include "RenderDefs.h"
#include "GPUMemoryManager.h"

#include "../Texture/TextureOptions.h"
#include "../Texture/Texture.h"
#include "../Texture/TextureManager.h"

#include "../Shader/ShaderProgramManager.h"

#include "StagingBufferManager.h"
#include "VertexCacheManager.h"

#include "RenderState.h"

#include "DrawSurface.h"

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

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) 
{
    std::cerr << "[VALIDATION LAYER]: " << msg << std::endl;
    return VK_FALSE;
}

void RenderCore::CreateDebugReport()
{
    if (m_vkValidationEnabled)
    {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
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

    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
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

	createInfo.pipelineStatistics =
		VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
		VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
		VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
		VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
		VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
		VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;

	if (m_vkGPU.m_vkPhysicalDevFeatures.tessellationShader)
	{
		createInfo.pipelineStatistics |=
			VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT;
	}
	createInfo.queryCount = m_queryCount;

    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
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
        createInfo.commandBufferCount = ION_RENDER_BUFFER_COUNT;

        VkResult result = vkAllocateCommandBuffers(m_vkDevice, &createInfo, m_vkCommandBuffers.data());
        ionAssertReturnValue(result == VK_SUCCESS, "Cannot create command buffer!", false);
    }

    {
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
        {
            VkResult result = vkCreateFence(m_vkDevice, &createInfo, vkMemory, &m_vkCommandBufferFences[i]);
            ionAssertReturnValue(result == VK_SUCCESS, "Cannot create fence!", false);
        }
    }

    return true;
}

ionBool RenderCore::CreateSwapChain()
{
    VkSurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(m_vkGPU.m_vkSurfaceFormats);
    VkPresentModeKHR presentMode = SelectPresentMode(m_vkGPU.m_vkPresentModes);
    VkExtent2D extent = SelectSurfaceExtent(m_vkGPU.m_vkSurfaceCaps, m_width, m_height);

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = m_vkSurface;
    info.minImageCount = ION_RENDER_BUFFER_COUNT;
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

    ionU32 numImages = 0;
    result = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &numImages, nullptr);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot get swap chain image!", false);
    ionAssertReturnValue(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.", false);

    result = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &numImages, m_vkSwapchainImages.data());
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot get swap chain image!", false);
    ionAssertReturnValue(numImages > 0, "vkGetSwapchainImagesKHR returned a zero image count.", false);

    // Triple buffer so I've 3 images
    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
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
    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
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
    depthTextureOptions.m_vkDepthFormat = m_vkDepthFormat;
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
            
            ionGPUMemoryManager().Free(m_vkMSAAAllocation);
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

    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
    {
        attachments[0] = m_vkSwapchainViews[i];
        VkResult result = vkCreateFramebuffer(m_vkDevice, &frameBufferCreateInfo, vkMemory, &m_vkFrameBuffers[i]);
        ionAssertReturnValue(result == VK_SUCCESS, "Impossible to create frame buffer.", false);
    }

    return true;
}

void RenderCore::DestroyFrameBuffers()
{
    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
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
    m_vkQueryPools.clear();
    m_vkCompletedSemaphores.clear();
    m_vkAcquiringSemaphores.clear();
    m_textureParams.clear();
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
    m_textureParams.clear();


    // core
    m_counter = 0;
    m_currentFrameData = 0;
    m_width = 640;
    m_height = 480;
    m_vkInstance = VK_NULL_HANDLE;
    m_vkFullScreen = false;
    m_vkSwapchain = VK_NULL_HANDLE;
    m_vkSwapchainFormat = VK_FORMAT_UNDEFINED;
    m_vkCurrentSwapIndex = 0;
    //m_vkMSAAAllocation;
    m_vkMSAAImage = VK_NULL_HANDLE;
    m_vkMSAAImageView = VK_NULL_HANDLE;
    m_vkCommandPool = VK_NULL_HANDLE;
    m_vkSurface = VK_NULL_HANDLE;
    m_vkPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    m_vkValidationEnabled = false;

    m_vkSwapchainExtent.width = m_width;
    m_vkSwapchainExtent.height = m_height;
    m_vkSwapchainImages.clear();
    m_vkSwapchainViews.clear();
    m_vkFrameBuffers.clear();
    m_vkAcquiringSemaphores.clear();
    m_vkCompletedSemaphores.clear();
    m_currentSwapIndex = 0;

    m_vkCommandBuffers.clear();
    m_vkCommandBufferFences.clear();
    m_vkCommandBufferRecorded.clear();

    m_queryIndex.clear();

    for (ionU32 i = 0; i < m_queryResults.capacity(); ++i)
    {
        m_queryResults[i].clear();
    }
    m_queryResults.clear();

    m_vkQueryPools.clear();
}

ionBool RenderCore::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    m_instance = _instance;
    m_window = _handle;

    m_width = _width;
    m_height = _height;
    m_vkFullScreen = _fullScreen;

    m_vkAcquiringSemaphores.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkCompletedSemaphores.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkQueryPools.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkCommandBuffers.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkCommandBufferFences.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkSwapchainImages.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkSwapchainViews.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkFrameBuffers.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);
    m_vkCommandBufferRecorded.resize(ION_RENDER_BUFFER_COUNT, VK_NULL_HANDLE);

    m_textureParams.resize(ION_RENDER_MAX_IMAGE_PARMS, nullptr);
    m_queryIndex.resize(ION_RENDER_BUFFER_COUNT, 0);
	m_queryResults.resize(ION_RENDER_BUFFER_COUNT);

 

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

	m_queryCount = m_vkGPU.m_vkPhysicalDevFeatures.tessellationShader ? 8 : 6;

	for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
	{
		m_queryResults[i].resize(m_queryCount);
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

    if (!CreateSwapChain())
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

    ionShaderProgramManager().Init(m_vkDevice, _shaderFolderPath);

    ionVertexCacheManager().Init(m_vkDevice, m_vkGPU.m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment);

    return true;
}

void RenderCore::Shutdown()
{
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

    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
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

    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
    {
        if (m_vkQueryPools[i] != VK_NULL_HANDLE)
        {
            vkDestroyQueryPool(m_vkDevice, m_vkQueryPools[i], vkMemory);
        }
    }

    for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
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

    ionStagingBufferManager().Shutdown();

    ionGPUMemoryManager().Shutdown();

    if (m_vkDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_vkDevice, vkMemory);
    }

    // Create physical device has not destruction because is more as "selected" 

    if (m_vkSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, vkMemory);
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

	for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
	{
		vkDestroyFramebuffer(m_vkDevice, m_vkFrameBuffers[i], vkMemory);
	}

	vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, static_cast<ionU32>(m_vkCommandBuffers.size()), m_vkCommandBuffers.data());

    //ionShaderProgramManager().Restart();
    vkDestroyPipelineCache(m_vkDevice, m_vkPipelineCache, vkMemory);

	if (m_vkRenderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(m_vkDevice, m_vkRenderPass, vkMemory);
	}

	for (ionU32 i = 0; i < ION_RENDER_BUFFER_COUNT; ++i)
	{
		if (m_vkSwapchainViews[i] != VK_NULL_HANDLE)
		{
			vkDestroyImageView(m_vkDevice, m_vkSwapchainViews[i], vkMemory);
		}
	}

	if (m_vkSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_vkDevice, m_vkSwapchain, vkMemory);
	}

	ionTextureManger().DestroyTexture("_ION_ViewDepth");
	DestroyRenderTargets();

	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkGPU.m_vkPhysicalDevice, m_vkSurface, &m_vkGPU.m_vkSurfaceCaps);
	ionAssertReturnVoid(result == VK_SUCCESS, "Device capabilities changed and not supported!");

	VkBool32 supportsPresent = VK_FALSE;
	result = vkGetPhysicalDeviceSurfaceSupportKHR(m_vkGPU.m_vkPhysicalDevice, m_vkPresentFamilyIndex, m_vkSurface, &supportsPresent);
	ionAssertReturnVoid(result == VK_SUCCESS, "Device surface changed and not supported!");
	ionAssertReturnVoid(supportsPresent == VK_TRUE, "New surface does not support present");

	CreateSwapChain();
	CreateRenderTargets();
	CreateRenderPass();
	CreatePipelineCache();
	CreateFrameBuffers();
}

void RenderCore::BlockingSwapBuffers()
{
    //++m_counter;
    m_currentFrameData = m_counter % ION_RENDER_BUFFER_COUNT;

    if (m_vkCommandBufferRecorded[m_currentFrameData] == false)
    {
        return;
    }

    VkResult result = vkWaitForFences(m_vkDevice, 1, &m_vkCommandBufferFences[m_currentFrameData], VK_TRUE, UINT64_MAX);
    ionAssertReturnVoid(result == VK_SUCCESS, "Wait for fences failed!");

    result = vkResetFences(m_vkDevice, 1, &m_vkCommandBufferFences[m_currentFrameData]);
    ionAssertReturnVoid(result == VK_SUCCESS, "Reset fences failed!");

    m_vkCommandBufferRecorded[m_currentFrameData] = false;

    //vkDeviceWaitIdle(m_vkDevice);   // it is needed?
}

ionBool RenderCore::StartFrame()
{
    VkResult result = vkAcquireNextImageKHR(m_vkDevice, m_vkSwapchain, UINT64_MAX, m_vkAcquiringSemaphores[m_currentFrameData], VK_NULL_HANDLE, &m_currentSwapIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) 
    {
        Recreate();
        return false;
    }
    ionAssertReturnValue(result == VK_SUCCESS || result  == VK_SUBOPTIMAL_KHR, "vkAcquireNextImageKHR failed!", false);

    ionStagingBufferManager().Submit();
    ionShaderProgramManager().StartFrame();
	
    VkQueryPool queryPool = m_vkQueryPools[m_currentFrameData];

    eosVector(ionU64) & results = m_queryResults[m_currentFrameData];

    if (m_queryIndex[m_currentFrameData] > 0) 
    {
        vkGetQueryPoolResults(m_vkDevice, queryPool, 0, 2, results.size(), results.data(), sizeof(ionU64), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

        const ionU64 gpuStart = results[0];
        const ionU64 gpuEnd = results[1];
        const ionU64 tick = (1000 * 1000 * 1000) / ((ionU64)m_vkGPU.m_vkPhysicalDeviceProps.limits.timestampPeriod);
        m_microSeconds = ((gpuEnd - gpuStart) * 1000 * 1000) / tick;

        m_queryIndex[m_currentFrameData] = 0;
    }

    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentFrameData];

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    ionAssertReturnValue(result == VK_SUCCESS, "vkBeginCommandBuffer failed!", false);

    vkCmdResetQueryPool(commandBuffer, queryPool, 0, m_queryCount);

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_vkRenderPass;
    renderPassBeginInfo.framebuffer = m_vkFrameBuffers[m_currentSwapIndex];
    renderPassBeginInfo.renderArea.extent = m_vkSwapchainExtent;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool, m_queryIndex[m_currentFrameData]++);

    return true;
}

void RenderCore::EndFrame()
{
    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentFrameData];

    vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_vkQueryPools[m_currentFrameData], m_queryIndex[m_currentFrameData]++);

    vkCmdEndRenderPass(commandBuffer);

    // Transition our swap image to present.
    // Do this instead of having the renderpass do the transition
    // so we can take advantage of the general layout to avoid 
    // additional image barriers.
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

    VkResult result = vkEndCommandBuffer(commandBuffer);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkEndCommandBuffer failed!");

    m_vkCommandBufferRecorded[m_currentFrameData] = true;

    VkSemaphore* acquire = &m_vkAcquiringSemaphores[m_currentFrameData];
    VkSemaphore* finished = &m_vkCompletedSemaphores[m_currentFrameData];

    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = acquire;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = finished;
    submitInfo.pWaitDstStageMask = &dstStageMask;

    result = vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, m_vkCommandBufferFences[m_currentFrameData]);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkQueueSubmit failed!");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = finished;
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
    m_currentFrameData = m_counter % ION_RENDER_BUFFER_COUNT;
}

void RenderCore::BindTexture(ionS32 _index, Texture* _image)
{
    ionAssertReturnVoid(_index >= 0 && _index < m_textureParams.capacity(), "Index out of bound of the capacity");
    m_textureParams[_index] = _image;
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

void RenderCore::SetScissor(ionS32 _leftX, ionS32 _bottomY, ionS32 _width, ionS32 _height)
{
    VkRect2D scissor;
    scissor.offset.x = _leftX;
    scissor.offset.y = _bottomY;
    scissor.extent.width = _width;
    scissor.extent.height = _height;
    vkCmdSetScissor(m_vkCommandBuffers[m_currentFrameData], 0, 1, &scissor);
}

void RenderCore::SetViewport(ionS32 _leftX, ionS32 _bottomY, ionS32 _width, ionS32 _height)
{
    VkViewport viewport;
    viewport.x = (ionFloat)_leftX;
    viewport.y = (ionFloat)_bottomY;
    viewport.width = (ionFloat)_width;
    viewport.height = (ionFloat)_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_vkCommandBuffers[m_currentFrameData], 0, 1, &viewport);
}

void RenderCore::SetPolygonOffset(ionFloat _scale, ionFloat _bias)
{
    vkCmdSetDepthBias(m_vkCommandBuffers[m_currentFrameData], _bias, 0.0f, _scale);
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
        vkCmdSetDepthBounds(m_vkCommandBuffers[m_currentFrameData], _zMin, _zMax);
    }
}

void RenderCore::SetClear(ionBool _color, ionBool _depth, ionBool _stencil, ionU8 _stencilValue, ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
{
    ionU32 numAttachments = 0;
    VkClearAttachment attachments[2];
    memset(attachments, 0, sizeof(attachments));

    if (_color)
    {
        VkClearAttachment & attachment = attachments[numAttachments++];
        attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment.colorAttachment = 0;
        VkClearColorValue & color = attachment.clearValue.color;
        color.float32[0] = _r;
        color.float32[1] = _g;
        color.float32[2] = _b;
        color.float32[3] = _a;
    }

    if (_depth || _stencil)
    {
        VkClearAttachment & attachment = attachments[numAttachments++];
        if (_depth) 
        {
            attachment.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (_stencil)
        {
            attachment.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        attachment.clearValue.depthStencil.depth = 1.0f;
        attachment.clearValue.depthStencil.stencil = _stencilValue;
    }

    VkClearRect clearRect = {};
    clearRect.baseArrayLayer = 0;
    clearRect.layerCount = 1;
    clearRect.rect.extent = m_vkSwapchainExtent;

    vkCmdClearAttachments(m_vkCommandBuffers[m_currentFrameData], numAttachments, attachments, 1, &clearRect);
}

void RenderCore::CopyFrameBuffer(Texture* _texture, ionS32 _x, ionS32 _y, ionS32 _textureWidth, ionS32 _textureHeight)
{
    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentFrameData];

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
        region.srcOffsets[1] = { _textureWidth, _textureHeight, 1 };

        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.baseArrayLayer = 0;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.layerCount = 1;
        region.dstOffsets[1] = { _textureWidth, _textureHeight, 1 };

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

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_vkRenderPass;
    renderPassBeginInfo.framebuffer = m_vkFrameBuffers[m_currentSwapIndex];
    renderPassBeginInfo.renderArea.extent = m_vkSwapchainExtent;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderCore::SetColor(const eosString& _param, ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
{
    float color[4];
    color[0] = _r;
    color[1] = _g;
    color[2] = _b;
    color[3] = _a;
    ionShaderProgramManager().SetRenderParmVector(_param, color);
}

void RenderCore::Draw(const DrawSurface& _surface)
{
    VkCommandBuffer commandBuffer = m_vkCommandBuffers[m_currentFrameData];
    
    // USING THE HASH VERSION HERE!!!
    ionShaderProgramManager().SetRenderParmMatrix(ION_MODEL_MATRIX_PARAM_TEXT, &_surface.m_modelMatrix[0]);
    ionShaderProgramManager().SetRenderParmMatrix(ION_VIEW_MATRIX_PARAM_TEXT, &_surface.m_viewMatrix[0]);
    ionShaderProgramManager().SetRenderParmMatrix(ION_PROJ_MATRIX_PARAM_TEXT, &_surface.m_projectionMatrix[0]);
  
    const ionS32 shaderProgramIndex = ionShaderProgramManager().FindProgram(_surface.m_material->GetShaderProgramName(), _surface.m_material->GetVertexLayout(), _surface.m_material->GetVertexShaderIndex(), _surface.m_material->GetFragmentShaderIndex());
    ionShaderProgramManager().BindProgram(shaderProgramIndex);
    ionShaderProgramManager().CommitCurrent(*this, m_stateBits, commandBuffer);

    ionSize indexOffset = 0;
    ionSize vertexOffset = 0;
    ionBool a = false;
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

    vkCmdDrawIndexed(commandBuffer, _surface.m_indexCount, 1, (indexOffset >> 1), vertexOffset / sizeof(Vertex), 0);
    //vkCmdDrawIndexed(commandBuffer, static_cast<ionU32>(_surface.m_indexCount), 1, 0, 0, 0);
}



ION_NAMESPACE_END