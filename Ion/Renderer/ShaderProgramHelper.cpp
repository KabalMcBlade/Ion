#include "ShaderProgramHelper.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

VkPipeline ShaderProgramHelper::CreateGraphicsPipeline(EVertexLayout _vertexLayout, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationShader, VkShaderModule _geometryShader, VkPipelineLayout _pipelineLayout, ionU64 _stateBits)
{
    return VK_NULL_HANDLE;
}


ION_NAMESPACE_END