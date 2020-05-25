#include "MemoryWrapper.h"

#include "../Core/MemorySettings.h"

EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN

CommonAllocator* GetCommonAllocator()
{
	static HeapArea<Settings::kCommonAllocatorSize> memoryArea;
	static CommonAllocator memoryAllocator(memoryArea, "CommonFreeListAllocator");

	return &memoryAllocator;
}

ION_NAMESPACE_END