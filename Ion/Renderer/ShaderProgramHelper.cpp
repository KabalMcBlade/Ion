#include "ShaderProgramHelper.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ShaderVertexLayout ShaderProgramHelper::m_vertexLayout;

void ShaderProgramHelper::CreateVertexDescriptor()
{
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    VkVertexInputBindingDescription binding = {};
    VkVertexInputAttributeDescription attribute = {};
    
    
    m_vertexLayout.m_inputState = createInfo;

    ionU32 locationIndex = 0;
    ionU32 locationOffset = 0;

    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    m_vertexLayout.m_bindinggDescription.push_back(binding);

    // Position
    attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;   //VK_FORMAT_R32G32B32_SFLOAT;
    attribute.location = locationIndex++;
    attribute.offset = locationOffset;
    m_vertexLayout.m_attributegDescription.push_back(attribute);
    locationOffset += sizeof(Vertex::m_position);

    // TexCoord
    attribute.format = VK_FORMAT_R32G32_SFLOAT;         //VK_FORMAT_R16G16_SFLOAT;
    attribute.location = locationIndex++;
    attribute.offset = locationOffset;
    m_vertexLayout.m_attributegDescription.push_back(attribute);
    locationOffset += sizeof(Vertex::m_textureCoordUV);

    // Normal
    attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
    attribute.location = locationIndex++;
    attribute.offset = locationOffset;
    m_vertexLayout.m_attributegDescription.push_back(attribute);
    locationOffset += sizeof(Vertex::m_normal);

    // Tangent
    attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
    attribute.location = locationIndex++;
    attribute.offset = locationOffset;
    m_vertexLayout.m_attributegDescription.push_back(attribute);
    locationOffset += sizeof(Vertex::m_tangent);

    // Color1
    attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
    attribute.location = locationIndex++;
    attribute.offset = locationOffset;
    m_vertexLayout.m_attributegDescription.push_back(attribute);
    locationOffset += sizeof(Vertex::m_color1);

    // Color2
    attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
    attribute.location = locationIndex++;
    attribute.offset = locationOffset;
    m_vertexLayout.m_attributegDescription.push_back(attribute);
    locationOffset += sizeof(Vertex::m_color2); // UNUSED because anything next! I keep it just to check at runtime!  
}

VkPipeline ShaderProgramHelper::CreateGraphicsPipeline(VkPipelineLayout _pipelineLayout, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationShader /*= VK_NULL_HANDLE*/, VkShaderModule _geometryShader /*= VK_NULL_HANDLE*/)
{
    return VK_NULL_HANDLE;
}


ION_NAMESPACE_END