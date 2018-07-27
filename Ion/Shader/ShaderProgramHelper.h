#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


struct Shader;
struct ShaderProgram;
struct ConstantsBindingDef;
class RenderCore;

class ShaderProgramHelper
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    static void CreateVertexDescriptor();
    static void CreateDescriptorPools(const VkDevice& _device, VkDescriptorPool& _pool);
    static void CreateDescriptorSetLayout(const VkDevice& _device, ShaderProgram& _shaderProgram, const Shader& _vertexShader, const Shader& _fragmentShader, const Shader& _tessellationControlShader, const Shader& _tessellationEvaluatorShader, const Shader& _geometryShader, const ConstantsBindingDef& _constants);
    static VkPipeline CreateGraphicsPipeline(const RenderCore& _render, VkRenderPass _renderPass, EVertexLayout _vertexLayoutType, VkPipelineLayout _pipelineLayout, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader = VK_NULL_HANDLE, VkShaderModule _tessellationControlShader = VK_NULL_HANDLE, VkShaderModule _tessellationEvaluatorShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE);

private:
    static VkDescriptorType GetDescriptorType(EShaderBinding _type);
    static VkStencilOpState GetStencilOpState(ionU64 _stencilStateBits);

private:
    static ShaderVertexLayout* m_vertexLayouts;
};

ION_NAMESPACE_END