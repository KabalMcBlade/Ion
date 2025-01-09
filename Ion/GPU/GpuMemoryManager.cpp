// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\GPU\GpuMemoryManager.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


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
