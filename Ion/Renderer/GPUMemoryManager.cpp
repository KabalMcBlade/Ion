#include "GPUMemoryManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryForward.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


GPUMemoryManager *GPUMemoryManager::s_instance = nullptr;


GPUMemoryManager::GPUMemoryManager()
{
}

GPUMemoryManager::~GPUMemoryManager()
{
}

void GPUMemoryManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(GPUMemoryManager, ION_MEMORY_ALIGNMENT_SIZE);
    }
}

void GPUMemoryManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

GPUMemoryManager& GPUMemoryManager::Instance()
{
    return *s_instance;
}

ION_NAMESPACE_END