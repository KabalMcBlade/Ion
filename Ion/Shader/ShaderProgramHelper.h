// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Shader\ShaderProgramHelper.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


using ShaderHelperAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


struct Shader;
struct ShaderProgram;
struct ConstantsBindingDef;
struct SpecializationConstants;
class Material;
class RenderCore;
class ION_DLL ShaderProgramHelper
{
public:
	static ShaderHelperAllocator* GetAllocator();

public:
    static void Create();
    static void Destroy();

    static void CreateVertexDescriptor();
    static void CreateDescriptorPools(const VkDevice& _device, VkDescriptorPool& _pool);
    static void CreateDescriptorSetLayout(const VkDevice& _device, ShaderProgram& _shaderProgram, const Shader* _vertexShader, const Shader* _fragmentShader, const Shader* _tessellationControlShader, const Shader* _tessellationEvaluatorShader, const Shader* _geometryShader, const Material* _material);
    static VkPipeline CreateGraphicsPipeline(const RenderCore& _render, VkRenderPass _renderPass, VkPrimitiveTopology _topology, EVertexLayout _vertexLayoutType, VkPipelineLayout _pipelineLayout, ionU64 _stateBits, 
        VkShaderModule _vertexShader = VK_NULL_HANDLE, VkShaderModule _fragmentShader = VK_NULL_HANDLE, VkShaderModule _tessellationControlShader = VK_NULL_HANDLE, VkShaderModule _tessellationEvaluatorShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE,
        SpecializationConstants* _vertexSpecConst = nullptr, SpecializationConstants* _fragmentSpecConst = nullptr, SpecializationConstants* _tessCtrlSpecConst = nullptr, SpecializationConstants* _tessEvalSpecConst = nullptr, SpecializationConstants* _geomSpecConst = nullptr);

private:
    static VkStencilOpState GetStencilOpState(ionU64 _stencilStateBits);

private:
	static ShaderVertexLayout* m_vertexLayouts[EVertexLayout_Count];
};

ION_NAMESPACE_END