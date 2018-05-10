#include "RenderManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"

#include "../Utilities/LoaderGLTF.h"

#include "VertexCacheManager.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{

}

ionBool RenderManager::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    return m_renderCore.Init(_instance, _handle, _width, _height, _fullScreen, _enableValidationLayer, _shaderFolderPath, _vkDeviceLocalSize, _vkHostVisibleSize, _vkStagingBufferSize);
}

void RenderManager::Shutdown()
{
    m_renderCore.Shutdown();
}

void RenderManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(RenderManager, EOS_MEMORY_ALIGNMENT_SIZE);
    }
}

void RenderManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

RenderManager& RenderManager::Instance()
{
    return *s_instance;
}

ionBool RenderManager::LoadModelFromFile(const eosString& _fileName, Entity& _entity)
{
    LoaderGLTF loader;
    return loader.Load(_fileName, m_renderCore.GetDevice(), _entity);
}



ION_NAMESPACE_END