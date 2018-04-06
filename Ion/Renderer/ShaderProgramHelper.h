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
    static void CreateVertexDescriptor();
    static VkPipeline CreateGraphicsPipeline(VkPipelineLayout _pipelineLayout, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationShader = VK_NULL_HANDLE, VkShaderModule _geometryShader = VK_NULL_HANDLE);

private:
    static ShaderVertexLayout m_vertexLayout;
};

ION_NAMESPACE_END