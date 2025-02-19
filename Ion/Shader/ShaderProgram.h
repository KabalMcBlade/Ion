// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Shader\ShaderProgram.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"

#include "../Core/MemoryWrapper.h"

#include "../Core/MemorySettings.h"

#include "ShaderProgramHelper.h"


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

    void AddParameter(const ionString& _name, EBufferParameterType _type, ionU32  _count = 1)
    {
        ionAssertReturnVoid(_count > 0, "count must be greater of 0!");

        if (_count == 1)
        {
            m_parameters.push_back(_name);
            m_type.push_back(_type);
        }
        else
        {
            for (ionU32 i = 0; i < _count; ++i)
            {
                const ionString indexParam(std::to_string(i).c_str());
                const ionString fullParam = _name + indexParam;

                m_parameters.push_back(fullParam);
                m_type.push_back(_type);
            }
        }
    }

    // even if there are public, please use the above accessor.
    // this because help in case you have to set the array (otherwise you can still directly access to this parameters)
    ionVector<ionString, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator> m_parameters;
    ionVector<EBufferParameterType, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator> m_type;

    // it is computed by the engine, do not set manually
    ionVector<ionSize, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator> m_runtimeParameters;

    ~UniformBinding()
    {
        m_parameters.clear();
        m_type.clear();
        m_runtimeParameters.clear();
    }
};

ION_INLINE ionBool operator==(const UniformBinding& lhs, const UniformBinding& rhs)
{
    const ionVector<EBufferParameterType, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type count = lhs.m_type.size();
    for (ionVector<EBufferParameterType, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != count; ++i)
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
    const ionVector<EBufferParameterType, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type count = lhs.m_type.size();
    for (ionVector<EBufferParameterType, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != count; ++i)
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

struct ION_DLL StorageBinding final
{
    ionU32              m_bindingIndex;
    VertexCacheHandler  m_cache;

    StorageBinding()
    {
        m_cache = 0;
    }
};

ION_INLINE ionBool operator==(const StorageBinding& lhs, const StorageBinding& rhs)
{
    return (lhs.m_bindingIndex == rhs.m_bindingIndex && lhs.m_cache == rhs.m_cache);
}

ION_INLINE ionBool operator!=(const StorageBinding& lhs, const StorageBinding& rhs)
{
    return (lhs.m_bindingIndex != rhs.m_bindingIndex || lhs.m_cache != rhs.m_cache);
}

//////////////////////////////////////////////////////////////////////////


// The push_constant in the shader and for now ONLY floats supported.
// Anyway seems enough, you can pass matrix, vector, bool, float and integer as "float" representation
struct ION_DLL ConstantsBindingDef final
{
    ionVector<ionFloat, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator> m_values;
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
    ionVector<UniformBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>   m_uniforms;
    ionVector<SamplerBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>   m_samplers;
    ionVector<StorageBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>   m_storages;

    ~ShaderLayoutDef()
    {
        Clear();
    }

    void Clear()
    {
        m_uniforms.clear();
        m_samplers.clear();
        m_storages.clear();
    }
};

ION_INLINE ionBool operator==(const ShaderLayoutDef& lhs, const ShaderLayoutDef& rhs)
{
    const ionVector<UniformBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type uniformCount = lhs.m_uniforms.size();
    for (ionVector<UniformBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != uniformCount; ++i)
    {
        if (lhs.m_uniforms[i] != rhs.m_uniforms[i])
        {
            return false;
        }
    }

    const ionVector<SamplerBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type samplerCount = lhs.m_samplers.size();
    for (ionVector<SamplerBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != samplerCount; ++i)
    {
        if (lhs.m_samplers[i] != rhs.m_samplers[i])
        {
            return false;
        }
    }

    const ionVector<StorageBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type storageCount = lhs.m_storages.size();
    for (ionVector<StorageBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != storageCount; ++i)
    {
        if (lhs.m_storages[i] != rhs.m_storages[i])
        {
            return false;
        }
    }

    return true;
}

ION_INLINE ionBool operator!=(const ShaderLayoutDef& lhs, const ShaderLayoutDef& rhs)
{
    const ionVector<UniformBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type uniformCount = lhs.m_uniforms.size();
    for (ionVector<UniformBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != uniformCount; ++i)
    {
        if (lhs.m_uniforms[i] == rhs.m_uniforms[i])
        {
            return false;
        }
    }

    const ionVector<SamplerBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type samplerCount = lhs.m_samplers.size();
    for (ionVector<SamplerBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != samplerCount; ++i)
    {
        if (lhs.m_samplers[i] == rhs.m_samplers[i])
        {
            return false;
        }
    }

    const ionVector<StorageBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type storageCount = lhs.m_storages.size();
    for (ionVector<StorageBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>::size_type i = 0; i != storageCount; ++i)
    {
        if (lhs.m_storages[i] == rhs.m_storages[i])
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
	ionVector<ionFloat, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator> m_values;
	ionVector<VkSpecializationMapEntry, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator> m_specializations;

    // if the command "Generate" is not executed, this one is invalid!
    VkSpecializationInfo m_specializationInfo;

    ionBool m_isGenerated;

    SpecializationConstants() : m_isGenerated(false)
    {
        memset(&m_specializationInfo, 0, sizeof(VkSpecializationInfo));
    }

    ~SpecializationConstants()
    {
    }

    void Clear()
    {
        m_values.clear();
    }

    void Generate()
    {
		m_specializations.resize(m_values.size());

        const ionU32 count = static_cast<ionU32>(m_specializations.size());
        for (ionU32 i = 0; i < count; ++i)
        {
			m_specializations[i].constantID = i;
			m_specializations[i].size = sizeof(ionFloat);
			m_specializations[i].offset = i * sizeof(ionFloat);
        }

        m_specializationInfo.dataSize = m_values.size() * sizeof(ionFloat);
        m_specializationInfo.mapEntryCount = static_cast<ionU32>(m_values.size());
        m_specializationInfo.pMapEntries = m_specializations.data();
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

	ionString                       m_name;
	ionString                       m_path;
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

    ionVector<EShaderBinding, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>   m_bindings;
    ionVector<PipelineState, ShaderHelperAllocator, ShaderProgramHelper::GetAllocator>    m_pipelines;
    EVertexLayout               m_vertextLayoutType;
    VkPipelineLayout            m_pipelineLayout;
    VkDescriptorSetLayout       m_descriptorSetLayout;
    const Material*             m_material;
};


ION_NAMESPACE_END