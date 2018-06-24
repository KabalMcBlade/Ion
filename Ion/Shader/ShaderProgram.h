#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN




enum EUniformParameterType
{
    EUniformParameterType_Matrix = 0,
    EUniformParameterType_Vector,
    EUniformParameterType_Float,
    EUniformParameterType_Integer,

    EUniformParameterType_Count
};

struct ION_DLL UniformBinding final
{
    ionU32                              m_bindingIndex;
    eosVector(eosString)                m_parameters;
    eosVector(EUniformParameterType)    m_type;

    // it is computed by the engine, do not set manually
    eosVector(ionSize)                    m_runtimeParameters;

    ~UniformBinding()
    {
        m_parameters.clear();
        m_type.clear();
        m_runtimeParameters.clear();
    }
};

class Texture;
struct ION_DLL SamplerBinding final
{
    ionU32                      m_bindingIndex;
    const Texture*              m_texture;

    SamplerBinding()
    {
        m_texture = nullptr;
    }

    ~SamplerBinding()
    {
        m_texture = nullptr;
    }
};

struct ION_DLL ShaderLayoutDef final
{
    eosVector(UniformBinding)    m_uniforms;
    eosVector(SamplerBinding)   m_samplers;

    ~ShaderLayoutDef()
    {
        Clear();
    }

    void Clear()
    {
        m_uniforms.clear();
        m_samplers.clear();
    }
};

class RenderCore;
struct Shader
{
    Shader() :
        m_shaderModule(VK_NULL_HANDLE)
    {}

    ~Shader()
    {
        m_shaderLayout.Clear();
    }

    ionBool IsValid() const
    {
        return m_shaderModule != VK_NULL_HANDLE;
    }

    eosString                       m_name;
    eosString                       m_path;
    EShaderStage                    m_stage;
    VkShaderModule                  m_shaderModule;
    ShaderLayoutDef                 m_shaderLayout;
};

struct ShaderProgram
{
    ShaderProgram();

    struct PipelineState 
    {
        PipelineState();

        ionU64        m_stateBits;
        VkPipeline    m_pipeline;
    };

    VkPipeline GetPipeline(const RenderCore& _render, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationControlShader = VK_NULL_HANDLE, VkShaderModule _tessellationEvaluatorShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE);

    eosString                    m_name;
    ionBool                        m_usesJoints;
    ionBool                        m_usesSkinning;
    ionS32                        m_vertexShaderIndex;
    ionS32                        m_fragmentShaderIndex;
    ionS32                        m_tessellationControlShaderIndex;
    ionS32                        m_tessellationEvaluatorShaderIndex;
    ionS32                        m_geometryShaderIndex;
    EVertexLayout               m_vertextLayoutType;
    VkPipelineLayout            m_pipelineLayout;
    VkDescriptorSetLayout        m_descriptorSetLayout;
    eosVector(EShaderBinding)    m_bindings;
    eosVector(PipelineState)    m_pipelines;
};


ION_NAMESPACE_END