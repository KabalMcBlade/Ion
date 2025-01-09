// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Core\MemoryWrapper.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


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