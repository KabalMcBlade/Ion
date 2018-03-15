#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"
#include "../Dependencies/Eos/Eos/Eos.h"


VK_ALLOCATOR_USING_NAMESPACE
EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN


ION_DLL void InitializeAllocators(eosSize _uiHeapSize, eosSize _uiLinearSize, eosSize _uiStackBlockSize, eosSize _uiStackBlockCount);
ION_DLL void ShutdownAllocators();

ION_DLL void InitializeVulkanAllocators(vkaSize _uiSizeCommand, vkaSize _uiSizeObject, vkaSize _uiSizeCache, vkaSize _uiSizeDevice, vkaSize _uiSizeInstace, vkaSize _uiGpuMaxMemoryBlocks);
ION_DLL void ShutdownVulkanAllocators();



ION_NAMESPACE_END