#include "VertexCacheManager.h"

#include "UniformBufferObject.h"

#include "../Dependencies/Eos/Eos/Eos.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


VertexCacheManager *VertexCacheManager::s_instance = nullptr;


VertexCacheManager::VertexCacheManager()
{
}

VertexCacheManager::~VertexCacheManager()
{

}

ionBool VertexCacheManager::Init(VkDeviceSize _uniformBufferOffsetAlignment)
{
    return false;
}

void VertexCacheManager::Shutdown()
{

}

void VertexCacheManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(VertexCacheManager, EOS_MEMORY_ALIGNMENT_SIZE);
    }
}

void VertexCacheManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

VertexCacheManager& VertexCacheManager::Instance()
{
    return *s_instance;
}

ionBool	VertexCacheManager::GetJointBuffer(VertexCacheHandler _handler, UniformBuffer *_jointBuffer)
{
    return false;
}

ION_NAMESPACE_END