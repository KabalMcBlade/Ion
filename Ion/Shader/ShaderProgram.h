#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN




enum EBufferParameterType
{
    EBufferParameterType_Matrix = 0,
    EBufferParameterType_Vector,
    EBufferParameterType_Float,
    EBufferParameterType_Integer,

    EBufferParameterType_Count
};


enum EPushConstantStage : ionU32
{
    EPushConstantStage_Vertex = VK_SHADER_STAGE_VERTEX_BIT,
    EPushConstantStage_TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    EPushConstantStage_TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    EPushConstantStage_Geometryx = VK_SHADER_STAGE_GEOMETRY_BIT,
    EPushConstantStage_Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    EPushConstantStage_Compute = VK_SHADER_STAGE_COMPUTE_BIT,
    EPushConstantStage_All = VK_SHADER_STAGE_ALL_GRAPHICS
};

//////////////////////////////////////////////////////////////////////////

struct ION_DLL UniformBinding final
{
    ionU32                              m_bindingIndex;
    eosVector(eosString)                m_parameters;
    eosVector(EBufferParameterType)    m_type;

    // it is computed by the engine, do not set manually
    eosVector(ionSize)                  m_runtimeParameters;

    ~UniformBinding()
    {
        m_parameters.clear();
        m_type.clear();
        m_runtimeParameters.clear();
    }
};

ION_INLINE ionBool operator==(const UniformBinding& lhs, const UniformBinding& rhs)
{
    const eosVector(EBufferParameterType)::size_type count = lhs.m_type.size();
    for (eosVector(EBufferParameterType)::size_type i = 0; i != count; ++i)
    {
        if ((lhs.m_type[i] != rhs.m_type[i]) || (lhs.m_runtimeParameters[i] != rhs.m_runtimeParameters[i]))
        {
            return false;
        }
    }

    return (lhs.m_bindingIndex == rhs.m_bindingIndex);
}

ION_INLINE ionBool operator!=(const UniformBinding& lhs, const UniformBinding& rhs)
{
    const eosVector(EBufferParameterType)::size_type count = lhs.m_type.size();
    for (eosVector(EBufferParameterType)::size_type i = 0; i != count; ++i)
    {
        if ((lhs.m_type[i] == rhs.m_type[i]) || (lhs.m_runtimeParameters[i] == rhs.m_runtimeParameters[i]))
        {
            return false;
        }
    }

    return (lhs.m_bindingIndex != rhs.m_bindingIndex);
}

//////////////////////////////////////////////////////////////////////////

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

ION_INLINE ionBool operator==(const SamplerBinding& lhs, const SamplerBinding& rhs)
{
    return (lhs.m_bindingIndex == rhs.m_bindingIndex) && (lhs.m_texture == rhs.m_texture);
}

ION_INLINE ionBool operator!=(const SamplerBinding& lhs, const SamplerBinding& rhs)
{
    return (lhs.m_bindingIndex != rhs.m_bindingIndex) && (lhs.m_texture != rhs.m_texture);
}

//////////////////////////////////////////////////////////////////////////

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

ION_INLINE ionBool operator==(const ConstantsBindingDef& lhs, const ConstantsBindingDef& rhs)
{
    return (lhs.GetData() == rhs.GetData());
}

ION_INLINE ionBool operator!=(const ConstantsBindingDef& lhs, const ConstantsBindingDef& rhs)
{
    return (lhs.GetData() != rhs.GetData());
}

//////////////////////////////////////////////////////////////////////////

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

ION_INLINE ionBool operator==(const ShaderLayoutDef& lhs, const ShaderLayoutDef& rhs)
{
    const eosVector(UniformBinding)::size_type uniformCount = lhs.m_uniforms.size();
    for (eosVector(UniformBinding)::size_type i = 0; i != uniformCount; ++i)
    {
        if (lhs.m_uniforms[i] != rhs.m_uniforms[i])
        {
            return false;
        }
    }

    const eosVector(SamplerBinding)::size_type samplerCount = lhs.m_samplers.size();
    for (eosVector(SamplerBinding)::size_type i = 0; i != samplerCount; ++i)
    {
        if (lhs.m_samplers[i] != rhs.m_samplers[i])
        {
            return false;
        }
    }

    return true;
}

ION_INLINE ionBool operator!=(const ShaderLayoutDef& lhs, const ShaderLayoutDef& rhs)
{
    const eosVector(UniformBinding)::size_type uniformCount = lhs.m_uniforms.size();
    for (eosVector(UniformBinding)::size_type i = 0; i != uniformCount; ++i)
    {
        if (lhs.m_uniforms[i] == rhs.m_uniforms[i])
        {
            return false;
        }
    }

    const eosVector(SamplerBinding)::size_type samplerCount = lhs.m_samplers.size();
    for (eosVector(SamplerBinding)::size_type i = 0; i != samplerCount; ++i)
    {
        if (lhs.m_samplers[i] == rhs.m_samplers[i])
        {
            return false;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

// Specialization constants
// they are assigned to the shader stage directly and for now ONLY floats supported.
// Anyway seems enough, you can pass matrix, vector, bool, float and integer as "float" representation

/*
use in the shader like below if you, for instance, have 2 values
layout (constant_id = 0) const float LIGHTING_MODEL = 0.0;
layout (constant_id = 1) const float PARAM_TOON_DESATURATION = 0.0;
*/
struct SpecializationConstants
{
    eosVector(ionFloat) m_values;

    // if the command "Generate" is not executed, this one is invalid!
    VkSpecializationInfo m_specializationInfo;

    ionBool m_isGenerated;

    SpecializationConstants() : m_isGenerated(false)
    {
        memset(&m_specializationInfo, 0, sizeof(VkSpecializationInfo));
    }

    ~SpecializationConstants()
    {
        Clear();
    }

    void Clear()
    {
        m_values.clear();
    }

    void Generate()
    {
        eosVector(VkSpecializationMapEntry) specializations;
        specializations.resize(m_values.size());

        const ionU32 count = static_cast<ionU32>(specializations.size());
        for (ionU32 i = 0; i < count; ++i)
        {
            specializations[i].constantID = i;
            specializations[i].size = sizeof(ionFloat);
            specializations[i].offset = i * sizeof(ionFloat);
        }

        m_specializationInfo.dataSize = m_values.size() * sizeof(ionFloat);
        m_specializationInfo.mapEntryCount = static_cast<ionU32>(m_values.size());
        m_specializationInfo.pMapEntries = specializations.data();
        m_specializationInfo.pData = m_values.data();

        m_isGenerated = true;
    }
};


//////////////////////////////////////////////////////////////////////////

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

    SpecializationConstants* GetSpecializationConstants()
    {
        if (m_specializationConstants.m_values.size() > 0)
        {
            return &m_specializationConstants;
        }
        else
        {
            return nullptr;
        }
    }

    eosString                       m_name;
    eosString                       m_path;
    EShaderStage                    m_stage;
    VkShaderModule                  m_shaderModule;
    SpecializationConstants         m_specializationConstants;
};

//////////////////////////////////////////////////////////////////////////

class Material;
struct ShaderProgram
{
    ShaderProgram();
    ~ShaderProgram();

    struct PipelineState 
    {
        PipelineState();

        ionU64          m_stateBits;
        VkPipeline      m_pipeline;
        VkRenderPass    m_renderpass;
    };

    VkPipeline GetPipeline(const RenderCore& _render, VkRenderPass _renderPass, ionU64 _stateBits, VkPrimitiveTopology _topology, 
                            VkShaderModule _vertexShader = VK_NULL_HANDLE, VkShaderModule _fragmentShader = VK_NULL_HANDLE, VkShaderModule _tessellationControlShader = VK_NULL_HANDLE, VkShaderModule _tessellationEvaluatorShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE,
                            SpecializationConstants* _vertexSpecConst = nullptr, SpecializationConstants* _fragmentSpecConst = nullptr, SpecializationConstants* _tessCtrlSpecConst = nullptr, SpecializationConstants* _tessEvalSpecConst = nullptr, SpecializationConstants* _geomSpecConst = nullptr);

    eosVector(EShaderBinding)   m_bindings;
    eosVector(PipelineState)    m_pipelines;
    EVertexLayout               m_vertextLayoutType;
    VkPipelineLayout            m_pipelineLayout;
    VkDescriptorSetLayout       m_descriptorSetLayout;
    const Material*             m_material;
};


ION_NAMESPACE_END