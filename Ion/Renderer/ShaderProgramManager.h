#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "RenderCommon.h"
#include "ShaderProgram.h"
#include "UniformBufferObject.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

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

    void	StartFrame();
    void    EndFrame();

private:
    ShaderProgramManager(const ShaderProgramManager& _Orig) = delete;
    ShaderProgramManager& operator = (const ShaderProgramManager&) = delete;

public:
    eosList(ShaderProgram) m_shaderPrograms;

private:
    ionS32	            m_current;
    eosList(Vector)     m_uniforms;
    eosList(Shader)	    m_shaders;

    ionS32				m_counter;
    ionS32				m_currentData;
    ionS32				m_currentDescSet;
    ionS32				m_currentParmBufferOffset;
    VkDescriptorPool	m_descriptorPools[ION_RENDER_BUFFER_COUNT];
    VkDescriptorSet		m_descriptorSets[ION_RENDER_BUFFER_COUNT][ION_MAX_DESCRIPTOR_SETS];

    UniformBuffer*	    m_parmBuffers[ION_RENDER_BUFFER_COUNT];

private:
    static ShaderProgramManager *s_instance;
};

ION_NAMESPACE_END


#define ionShaderProgramManager() ion::ShaderProgramManager::Instance()