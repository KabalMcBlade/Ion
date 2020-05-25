#include "GPUMemoryManager.h"

// #include "../Dependencies/Eos/Eos/Eos.h"
// 
// 
// EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

GPUMemoryManager::GPUMemoryManager()
{
}

GPUMemoryManager::~GPUMemoryManager()
{
}

GPUMemoryManager& GPUMemoryManager::Instance()
{
    static GPUMemoryManager instance;
    return instance;
}

ION_NAMESPACE_END