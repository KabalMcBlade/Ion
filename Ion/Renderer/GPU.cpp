#include "GPU.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

GPUAllocator* GPU::GetAllocator()
{
	static HeapArea<Settings::kGPUAllocatorSize> memoryArea;
	static GPUAllocator memoryAllocator(memoryArea, "GPUFreeListAllocator");

	return &memoryAllocator;
}

GPU::GPU() :  m_vkPhysicalDevice(VK_NULL_HANDLE)
{
	memset(&m_vkPhysicalDeviceProps, 0, sizeof(VkPhysicalDeviceProperties));
	memset(&m_vkPhysicalDeviceMemoryProperties, 0, sizeof(VkPhysicalDeviceMemoryProperties));
	memset(&m_vkPhysicalDevFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
	memset(&m_vkSurfaceCaps, 0, sizeof(VkSurfaceCapabilitiesKHR));
}

GPU::~GPU()
{
    m_vkSurfaceFormats.clear();
    //m_vkSurfaceFormats.clear();
    m_vkPresentModes.clear();
    m_vkQueueFamilyProps.clear();
    m_vkExtensionProps.clear();
}

ionBool GPU::Set(const VkInstance& _vkInstance, const VkSurfaceKHR& _vkSurface, const VkPhysicalDevice& _vkDevice)
{
    m_vkPhysicalDevice = _vkDevice;

    ionU32 numQueues = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &numQueues, nullptr);
    ionAssertReturnValue(numQueues > 0, "vkGetPhysicalDeviceQueueFamilyProperties returned zero queues.", false);

    m_vkQueueFamilyProps.resize(numQueues);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &numQueues, m_vkQueueFamilyProps.data());
    ionAssertReturnValue(numQueues > 0, "vkGetPhysicalDeviceQueueFamilyProperties returned zero queues.", false);

    ionU32 numExtension = 0;
    VkResult result = vkEnumerateDeviceExtensionProperties(m_vkPhysicalDevice, nullptr, &numExtension, nullptr);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot enumerate device extension!", false);
    ionAssertReturnValue(numExtension > 0, "vkEnumerateDeviceExtensionProperties returned zero extensions.", false);
   
    m_vkExtensionProps.resize(numExtension);
    result = vkEnumerateDeviceExtensionProperties(m_vkPhysicalDevice, nullptr, &numExtension, m_vkExtensionProps.data());
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot enumerate device extension!", false);
    ionAssertReturnValue(numExtension > 0, "vkEnumerateDeviceExtensionProperties returned zero extensions.", false);
    
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, _vkSurface, &m_vkSurfaceCaps);
    ionAssertReturnValue(result == VK_SUCCESS, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR returned not capabilities for the surface selected.", false);

    ionU32 numFormats = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, _vkSurface, &numFormats, nullptr);
    ionAssertReturnValue(result == VK_SUCCESS, "vkGetPhysicalDeviceSurfaceFormatsKHR returned surface formats.", false);
    ionAssertReturnValue(numFormats > 0, "vkGetPhysicalDeviceSurfaceFormatsKHR returned surface formats.", false);

    m_vkSurfaceFormats.resize(numFormats);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, _vkSurface, &numFormats, m_vkSurfaceFormats.data());
    ionAssertReturnValue(result == VK_SUCCESS, "vkGetPhysicalDeviceSurfaceFormatsKHR returned surface formats.", false);
    ionAssertReturnValue(numFormats > 0, "vkGetPhysicalDeviceSurfaceFormatsKHR returned zero surface formats.", false);

    ionU32 numPresentModes = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, _vkSurface, &numPresentModes, nullptr);
    ionAssertReturnValue(result == VK_SUCCESS, "vkGetPhysicalDeviceSurfacePresentModesKHR returned zero present modes.", false);
    ionAssertReturnValue(numPresentModes > 0, "vkGetPhysicalDeviceSurfacePresentModesKHR returned zero present modes.", false);

    m_vkPresentModes.resize(numPresentModes);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, _vkSurface, &numPresentModes, m_vkPresentModes.data());
    ionAssertReturnValue(result == VK_SUCCESS, "vkGetPhysicalDeviceSurfacePresentModesKHR returned zero present modes.", false);
    ionAssertReturnValue(numPresentModes > 0, "vkGetPhysicalDeviceSurfacePresentModesKHR returned zero present modes.", false);

    vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceMemoryProperties);
    vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceProps);
    vkGetPhysicalDeviceFeatures(m_vkPhysicalDevice, &m_vkPhysicalDevFeatures);

    return true;
}

ION_NAMESPACE_END