#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "RenderCommon.h"
#include "ShaderProgram.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ShaderProgramManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static ShaderProgramManager& Instance();

    ionBool Init();
    void    Shutdown();

    ShaderProgramManager();
    ~ShaderProgramManager();

private:
    ShaderProgramManager(const ShaderProgramManager& _Orig) = delete;
    ShaderProgramManager& operator = (const ShaderProgramManager&) = delete;

private:

    static ShaderProgramManager *s_instance;
};

ION_NAMESPACE_END


#define ionShaderProgramManager() ion::ShaderProgramManager::Instance()