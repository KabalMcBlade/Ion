#include "GpuMemoryManager.h"


ION_NAMESPACE_BEGIN


GpuFreeListAllocator* GpuMemoryManager::GetAllocator()
{
	static HeapArea<Settings::kGPUAllocatorSize> memoryArea;
	static GpuFreeListAllocator memoryAllocator(memoryArea, "GPUFreeListAllocator");

	return &memoryAllocator;
}

GpuMemoryManager& GpuMemoryManager::Instance()
{
	static GpuMemoryManager instance;
	return instance;
}


ION_NAMESPACE_END
