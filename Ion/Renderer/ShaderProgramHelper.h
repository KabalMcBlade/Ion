#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "RenderCommon.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ShaderProgramHelper
{
public:
    static VkPipeline CreateGraphicsPipeline(EVertexLayout _vertexLayout, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationShader, VkShaderModule _geometryShader, VkPipelineLayout _pipelineLayout, ionU64 _stateBits);
};

ION_NAMESPACE_END