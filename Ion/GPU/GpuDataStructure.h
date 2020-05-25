#pragma once

#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

class GpuMemoryList;

enum EMemoryUsage
{
	// not strictly specified, just follow the vkDeviceMemoryCreateInfo parameters
	EMemoryUsage_Unknown = 0,

	// Best for:
	// read and write resource on device only (image as attachment)
	// resource transfered once from host or something do not change too much, such VB, UB, etc.
	EMemoryUsage_GPU,

	// Best for: staging buffer
	EMemoryUsage_CPU,

	// Best for: textures, vertex buffers, uniform buffers updated every frame or every draw call.
	EMemoryUsage_CPU_to_GPU,

	// Best for:  screen capture, HDR and CPU-side like copy of vertex buffer and for collision detection.
	EMemoryUsage_GPU_to_CPU,

	EMemoryUsage_Count
};


enum EGpuMemoryType
{
	EGpuMemoryType_Free = 0,
	EGpuMemoryType_Buffer,
	EGpuMemoryType_Image,
	EGpuMemoryType_ImageLinear,
	EGpuMemoryType_ImageOptimal,
	EGpuMemoryType_Count,
};


ION_MEMORY_ALIGNED struct GpuMemoryCreateInfo final
{
	size			m_size;
	size			m_align;
	uint32          m_memoryTypeBits;
	EMemoryUsage    m_usage;
	EGpuMemoryType  m_type;

	GpuMemoryCreateInfo() :
		m_type(EGpuMemoryType_Free),
		m_memoryTypeBits(0),
		m_usage(EMemoryUsage_Unknown),
		m_align(0),
		m_size(0)
	{}
};


ION_MEMORY_ALIGNED struct GpuMemoryAllocation final
{
	VkResult            m_result;
	GpuMemoryList*		m_owner;
	uint32              m_index;
	VkDeviceMemory      m_memory;
	VkDeviceSize        m_size;
	VkDeviceSize        m_offset;
	uint8*              m_mappedData; // Not null means memory is mapped, so host visible.

	GpuMemoryAllocation() :
		m_result(VK_ERROR_INITIALIZATION_FAILED),
		m_owner(nullptr),
		m_index(0),
		m_memory(VK_NULL_HANDLE),
		m_offset(0),
		m_size(0),
		m_mappedData(nullptr) 
	{}
};


ION_NAMESPACE_END