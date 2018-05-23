#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderCore;

enum EUniformParameterType
{
    EUniformParameterType_Matrix = 0,
	EUniformParameterType_Vector,
	
    EUniformParameterType_Count
};

struct Shader
{
    Shader() :
        m_shaderModule(VK_NULL_HANDLE)
    {}

    ~Shader()
    {
        m_bindings.clear();
        m_parametersHash.clear();
    }

    ionBool IsValid() const
    {
        return m_shaderModule != VK_NULL_HANDLE;
    }

    eosString						m_name;
    EShaderStage					m_stage;
    VkShaderModule					m_shaderModule;
    eosVector(EShaderBinding)		m_bindings;
    eosVector(ionSize)				m_parametersHash;   // while load a shader, we need to pass the parameters, here we save the hash which will be used as key
	eosVector(EUniformParameterType)m_parameterType;
};

struct ShaderProgram
{
    ShaderProgram();

    struct PipelineState 
    {
        PipelineState();

        ionU64		m_stateBits;
        VkPipeline	m_pipeline;
    };

    VkPipeline GetPipeline(const RenderCore& _render, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationControlShader = VK_NULL_HANDLE, VkShaderModule _tessellationEvaluatorShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE);

    eosString				    m_name;
    ionBool						m_usesJoints;
    ionBool						m_usesSkinning;
    ionS32						m_vertexShaderIndex;
    ionS32						m_fragmentShaderIndex;
    ionS32						m_tessellationControlShaderIndex;
    ionS32						m_tessellationEvaluatorShaderIndex;
    ionS32						m_geometryShaderIndex;
    EVertexLayout               m_vertextLayoutType;
    VkPipelineLayout			m_pipelineLayout;
    VkDescriptorSetLayout		m_descriptorSetLayout;
    eosVector(EShaderBinding)	m_bindings;
    eosVector(PipelineState)	m_pipelines;
};


ION_NAMESPACE_END