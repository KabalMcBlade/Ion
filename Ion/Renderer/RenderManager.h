#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "RenderCommon.h"

#include "RenderCore.h"


ION_NAMESPACE_BEGIN


class ION_DLL RenderManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static RenderManager& Instance();

    ionBool Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize);
    void    Shutdown();

    RenderManager();
    ~RenderManager();


private:
    RenderManager(const RenderManager& _Orig) = delete;
    RenderManager& operator = (const RenderManager&) = delete;

private:
    RenderCore  m_renderCore;

    static RenderManager *s_instance;
};

ION_NAMESPACE_END


#define ionRenderManager() ion::RenderManager::Instance()