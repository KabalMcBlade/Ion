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


enum EPushConstantStage : ionU32
{
    EPushConstantStage_Vertex = VK_SHADER_STAGE_VERTEX_BIT,
    EPushConstantStage_TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    EPushConstantStage_TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    EPushConstantStage_Geometryx = VK_SHADER_STAGE_GEOMETRY_BIT,
    EPushConstantStage_Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    EPushConstantStage_All = VK_SHADER_STAGE_ALL_GRAPHICS
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

// The push_constant in the shader and for now ONLY floats supported.
// Anyway seems enough, you can pass matrix, vector, bool, float and integer as "float" representation
struct ION_DLL ConstantsBindingDef final
{
    eosVector(ionFloat) m_values;
    EPushConstantStage m_shaderStages;

    const void* GetData() const { return m_values.data(); }
    const ionSize GetSize() const { return m_values.size(); }
    const ionSize GetSizeByte() const { return m_values.size() * sizeof(ionFloat); }
    const ionBool IsValid() const { return GetSize() > 0; }

    // it is computed by the engine, do not set manually
    VkShaderStageFlagBits   m_runtimeStages;

    void Clear()
    {
        m_values.clear();
        m_shaderStages = (EPushConstantStage)0;
    }

    ~ConstantsBindingDef()
    {
        Clear();
    }
};

struct ION_DLL ShaderLayoutDef final
{
    eosVector(UniformBinding)   m_uniforms;
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
    }

    ionBool IsValid() const
    {
        return m_shaderModule != VK_NULL_HANDLE;
    }

    eosString                       m_name;
    eosString                       m_path;
    EShaderStage                    m_stage;
    VkShaderModule                  m_shaderModule;
};

struct ShaderProgram
{
    ShaderProgram();

    struct PipelineState 
    {
        PipelineState();

        ionU64          m_stateBits;
        VkPipeline      m_pipeline;
        VkRenderPass    m_renderpass;
    };

    VkPipeline GetPipeline(const RenderCore& _render, VkRenderPass _renderPass, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationControlShader = VK_NULL_HANDLE, VkShaderModule _tessellationEvaluatorShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE);

    eosVector(EShaderBinding)   m_bindings;
    eosVector(PipelineState)    m_pipelines;
    eosString                   m_name;
    EVertexLayout               m_vertextLayoutType;
    VkPipelineLayout            m_pipelineLayout;
    VkDescriptorSetLayout       m_descriptorSetLayout;
    ConstantsBindingDef         m_constantsDef;
    ionSize                     m_hash;
    ionS32                      m_vertexShaderIndex;
    ionS32                      m_fragmentShaderIndex;
    ionS32                      m_tessellationControlShaderIndex;
    ionS32                      m_tessellationEvaluatorShaderIndex;
    ionS32                      m_geometryShaderIndex;
    ionBool                     m_usesJoints;
    ionBool                     m_usesSkinning;
};


ION_NAMESPACE_END