#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


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

class ShaderProgram
{
    ShaderProgram();

    struct PipelineState 
    {
        PipelineState();

        ionU64		m_stateBits;
        VkPipeline	m_pipeline;
    };

    VkPipeline GetPipeline(ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE);

    eosString				    m_name;
    ionBool						m_usesJoints;
    ionBool						m_usesSkinning;
    ionS32						m_vertexShaderIndex;
    ionS32						m_fragmentShaderIndex;
    ionS32						m_tessellationShaderIndex;
    ionS32						m_geometryShaderIndex;
    VkPipelineLayout			m_pipelineLayout;
    VkDescriptorSetLayout		m_descriptorSetLayout;
    eosVector(EShaderBinding)	m_bindings;
    eosVector(PipelineState)	m_pipelines;
};


ION_NAMESPACE_END