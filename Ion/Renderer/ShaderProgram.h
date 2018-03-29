#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


struct ShaderVertexLayout
{
    VkPipelineVertexInputStateCreateInfo            m_inputState;
    eosVector(VkVertexInputBindingDescription)      m_bindinggDescription;
    eosVector(VkVertexInputAttributeDescription)    m_attributegDescription;
};

struct Shader
{
    Shader() :
        m_shaderModule(VK_NULL_HANDLE)
    {}

    ~Shader()
    {
        m_bindings.clear();
        m_parameterIndices.clear();
    }

    eosString				    m_name;
    EShaderStage			    m_stage;
    VkShaderModule			    m_shaderModule;
    eosVector(EShaderBinding)	m_bindings;
    eosVector(ionS32)			m_parameterIndices;
};



ION_NAMESPACE_END