#include "ShaderProgramHelper.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "ShaderProgram.h"
#include "../Renderer/RenderState.h"
#include "../Renderer/RenderCore.h"

#include "../Material/Material.h"

EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ShaderVertexLayout **ShaderProgramHelper::m_vertexLayouts = nullptr;

void ShaderProgramHelper::Create()
{

    m_vertexLayouts = (ShaderVertexLayout**)eosNewRaw(sizeof(ShaderVertexLayout*) * EVertexLayout_Count, ION_MEMORY_ALIGNMENT_SIZE);
    for (int i = 0; i < EVertexLayout_Count; i++)
    {
        m_vertexLayouts[i] = eosNew(ShaderVertexLayout, ION_MEMORY_ALIGNMENT_SIZE);
    }
}

void ShaderProgramHelper::Destroy()
{
    for (int i = 0; i < EVertexLayout_Count; i++)
    {
        eosDelete(m_vertexLayouts[i]);
    }
    eosDeleteRaw(m_vertexLayouts);
}

void ShaderProgramHelper::CreateVertexDescriptor()
{
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    VkVertexInputBindingDescription binding = {};
    VkVertexInputAttributeDescription attribute = {};
    
    {
        ShaderVertexLayout& vertexLayout = *m_vertexLayouts[EVertexLayout::EVertexLayout_Full];

        vertexLayout.m_inputState = createInfo;

        ionU32 locationIndex = 0;
        ionU32 locationOffset = 0;

        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexLayout.m_bindinggDescription.push_back(binding);

        // Position
        attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;   //VK_FORMAT_R32G32B32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_position);

        // TexCoord0
        attribute.format = VK_FORMAT_R32G32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_textureCoordUV0);

        // TexCoord1
        attribute.format = VK_FORMAT_R32G32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_textureCoordUV1);

        // Joints
        attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_joints);

        // Normal
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_normal);

        // Tangent
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_tangent);

        // Color
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_color);

        // Weights
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
    }

    {
        ShaderVertexLayout& vertexLayout = *m_vertexLayouts[EVertexLayout::EVertexLayout_Pos_UV_Normal];

        vertexLayout.m_inputState = createInfo;

        ionU32 locationIndex = 0;
        ionU32 locationOffset = 0;

        binding.stride = sizeof(VertexSimple);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexLayout.m_bindinggDescription.push_back(binding);

        // Position
        attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(VertexSimple::m_position);

        // TexCoord0
        attribute.format = VK_FORMAT_R32G32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(VertexSimple::m_textureCoordUV);

        // Normal
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
    }

    {
        ShaderVertexLayout& vertexLayout = *m_vertexLayouts[EVertexLayout::EVertexLayout_Pos_UV];

        vertexLayout.m_inputState = createInfo;

        ionU32 locationIndex = 0;
        ionU32 locationOffset = 0;

        binding.stride = sizeof(VertexUV);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexLayout.m_bindinggDescription.push_back(binding);

        // Position
        attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(VertexUV::m_position);

        // TexCoord
        attribute.format = VK_FORMAT_R32G32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
    }

    {
        ShaderVertexLayout& vertexLayout = *m_vertexLayouts[EVertexLayout::EVertexLayout_Pos_Color];

        vertexLayout.m_inputState = createInfo;

        ionU32 locationIndex = 0;
        ionU32 locationOffset = 0;

        binding.stride = sizeof(VertexColored);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexLayout.m_bindinggDescription.push_back(binding);

        // Position
        attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(VertexColored::m_position);

        // Color
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
    }

    {
        ShaderVertexLayout& vertexLayout = *m_vertexLayouts[EVertexLayout::EVertexLayout_Pos];

        vertexLayout.m_inputState = createInfo;

        binding.stride = sizeof(VertexPlain);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexLayout.m_bindinggDescription.push_back(binding);

        // Position
        attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attribute.location = 0;
        attribute.offset = 0;
        vertexLayout.m_attributegDescription.push_back(attribute);
    }

    {
        ShaderVertexLayout& vertexLayout = *m_vertexLayouts[EVertexLayout::EVertexLayout_Empty];
        vertexLayout.m_inputState = createInfo;
    }
}

void ShaderProgramHelper::CreateDescriptorPools(const VkDevice& _device, VkDescriptorPool& _pool)
{
    const ionU32 poolCount = 2;
    VkDescriptorPoolSize poolSizes[poolCount];
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = ION_MAX_DESCRIPTOR_UNIFORM_BUFFERS;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = ION_MAX_DESCRIPTOR_IMAGE_SAMPLERS;

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.maxSets = ION_MAX_DESCRIPTOR_SETS;
    createInfo.poolSizeCount = poolCount;
    createInfo.pPoolSizes = poolSizes;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkResult result = vkCreateDescriptorPool(_device, &createInfo, vkMemory, &_pool);
    ionAssertReturnVoid(result == VK_SUCCESS, "vkCreateDescriptorPool cannot create descriptor pool!");
}

VkDescriptorType ShaderProgramHelper::GetDescriptorType(EShaderBinding _type)
{
    switch (_type) 
    {
    case EShaderBinding_Uniform: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case EShaderBinding_Sampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        ionAssertReturnValue(false, "Invalid descriptor type!", VK_DESCRIPTOR_TYPE_MAX_ENUM);
    }
}

void ShaderProgramHelper::CreateDescriptorSetLayout(const VkDevice& _device, ShaderProgram& _shaderProgram, const Shader& _vertexShader, const Shader& _fragmentShader, const Shader& _tessellationControlShader, const Shader& _tessellationEvaluatorShader, const Shader& _geometryShader, const Material* _material)
{
    // Descriptor Set Layout
    {
        eosVector(VkDescriptorSetLayoutBinding) layoutBindings;
        VkDescriptorSetLayoutBinding binding = {};
        binding.descriptorCount = 1;

        if (_vertexShader.IsValid())
        {
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            ionSize uniformCount = _material->GetVertexShaderLayout().m_uniforms.size();
            for (ionSize i = 0; i < uniformCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.binding = _material->GetVertexShaderLayout().m_uniforms[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Uniform);
            }

            ionSize samplerCount = _material->GetVertexShaderLayout().m_samplers.size();
            for (ionSize i = 0; i < samplerCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                binding.binding = _material->GetVertexShaderLayout().m_samplers[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Sampler);
            }
        }

        if (_tessellationControlShader.IsValid())
        {
            binding.stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

            ionSize uniformCount = _material->GetTessellationControlShaderLayout().m_uniforms.size();
            for (ionSize i = 0; i < uniformCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.binding = _material->GetTessellationControlShaderLayout().m_uniforms[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Uniform);
            }

            ionSize samplerCount = _material->GetTessellationControlShaderLayout().m_samplers.size();
            for (ionSize i = 0; i < samplerCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                binding.binding = _material->GetTessellationControlShaderLayout().m_samplers[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Sampler);
            }
        }

        if (_tessellationEvaluatorShader.IsValid())
        {
            binding.stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

            ionSize uniformCount = _material->GetTessellationEvaluatorShaderLayout().m_uniforms.size();
            for (ionSize i = 0; i < uniformCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.binding = _material->GetTessellationEvaluatorShaderLayout().m_uniforms[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Uniform);
            }

            ionSize samplerCount = _material->GetTessellationEvaluatorShaderLayout().m_samplers.size();
            for (ionSize i = 0; i < samplerCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                binding.binding = _material->GetTessellationEvaluatorShaderLayout().m_samplers[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Sampler);
            }
        }

        if (_geometryShader.IsValid())
        {
            binding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;

            ionSize uniformCount = _material->GetGeometryShaderLayout().m_uniforms.size();
            for (ionSize i = 0; i < uniformCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.binding = _material->GetGeometryShaderLayout().m_uniforms[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Uniform);
            }

            ionSize samplerCount = _material->GetGeometryShaderLayout().m_samplers.size();
            for (ionSize i = 0; i < samplerCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                binding.binding = _material->GetGeometryShaderLayout().m_samplers[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Sampler);
            }
        }

        if (_fragmentShader.IsValid())
        {
            binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            ionSize uniformCount = _material->GetFragmentShaderLayout().m_uniforms.size();
            for (ionSize i = 0; i < uniformCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.binding = _material->GetFragmentShaderLayout().m_uniforms[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Uniform);
            }

            ionSize samplerCount = _material->GetFragmentShaderLayout().m_samplers.size();
            for (ionSize i = 0; i < samplerCount; ++i)
            {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                binding.binding = _material->GetFragmentShaderLayout().m_samplers[i].m_bindingIndex;
                layoutBindings.push_back(binding);

                _shaderProgram.m_bindings.push_back(EShaderBinding_Sampler);
            }
        }

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = (ionU32)layoutBindings.size();
        createInfo.pBindings = layoutBindings.data();

        VkResult result = vkCreateDescriptorSetLayout(_device, &createInfo, vkMemory, &_shaderProgram.m_descriptorSetLayout);
        ionAssertReturnVoid(result == VK_SUCCESS, "vkCreateDescriptorSetLayout cannot create descriptor set layout!");
    }


    // Pipeline Layout
    {
        VkPipelineLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.setLayoutCount = 1;
        createInfo.pSetLayouts = &_shaderProgram.m_descriptorSetLayout;

        if (_material->GetConstantsShaders().IsValid())
        {
            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags = _material->GetConstantsShaders().m_runtimeStages;
            pushConstantRange.offset = 0;
            pushConstantRange.size = static_cast<ionU32>(_material->GetConstantsShaders().GetSizeByte());

            createInfo.pushConstantRangeCount = 1;
            createInfo.pPushConstantRanges = &pushConstantRange;
        }

        VkResult result = vkCreatePipelineLayout(_device, &createInfo, vkMemory, &_shaderProgram.m_pipelineLayout);
        ionAssertReturnVoid(result == VK_SUCCESS, "vkCreateDescriptorSetLayout cannot create pipeline layout!");
    }
}

VkStencilOpState ShaderProgramHelper::GetStencilOpState(ionU64 _stencilStateBits)
{
    VkStencilOpState state = {};


    switch (_stencilStateBits & EStencilFrontOperator_Fail_Bits)
    {
    case EStencilFrontOperator_Fail_Keep:            state.failOp = VK_STENCIL_OP_KEEP; break;
    case EStencilFrontOperator_Fail_Zero:            state.failOp = VK_STENCIL_OP_ZERO; break;
    case EStencilFrontOperator_Fail_Replace:        state.failOp = VK_STENCIL_OP_REPLACE; break;
    case EStencilFrontOperator_Fail_Increment:        state.failOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Fail_Decrement:        state.failOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Fail_Invert:            state.failOp = VK_STENCIL_OP_INVERT; break;
    case EStencilFrontOperator_Fail_Increment_Wrap: state.failOp = VK_STENCIL_OP_INCREMENT_AND_WRAP; break;
    case EStencilFrontOperator_Fail_Decrement_Wrap: state.failOp = VK_STENCIL_OP_DECREMENT_AND_WRAP; break;
    }

    switch (_stencilStateBits & EStencilFrontOperator_ZFail_Bits)
    {
    case EStencilFrontOperator_ZFail_Keep:                state.depthFailOp = VK_STENCIL_OP_KEEP; break;
    case EStencilFrontOperator_ZFail_Zero:                state.depthFailOp = VK_STENCIL_OP_ZERO; break;
    case EStencilFrontOperator_ZFail_Replace:            state.depthFailOp = VK_STENCIL_OP_REPLACE; break;
    case EStencilFrontOperator_ZFail_Increment:            state.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_ZFail_Decrement:            state.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_ZFail_Invert:            state.depthFailOp = VK_STENCIL_OP_INVERT; break;
    case EStencilFrontOperator_ZFail_Increment_Wrap:    state.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_WRAP; break;
    case EStencilFrontOperator_ZFail_Decrement_Wrap:    state.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_WRAP; break;
    }

    switch (_stencilStateBits & EStencilFrontOperator_Pass_Bits)
    {
    case EStencilFrontOperator_Pass_Keep:            state.passOp = VK_STENCIL_OP_KEEP; break;
    case EStencilFrontOperator_Pass_Zero:            state.passOp = VK_STENCIL_OP_ZERO; break;
    case EStencilFrontOperator_Pass_Replace:        state.passOp = VK_STENCIL_OP_REPLACE; break;
    case EStencilFrontOperator_Pass_Increment:        state.passOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Pass_Decrement:        state.passOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Pass_Invert:            state.passOp = VK_STENCIL_OP_INVERT; break;
    case EStencilFrontOperator_Pass_Increment_Wrap:    state.passOp = VK_STENCIL_OP_INCREMENT_AND_WRAP; break;
    case EStencilFrontOperator_Pass_Decrement_Wrap:    state.passOp = VK_STENCIL_OP_DECREMENT_AND_WRAP; break;
    }

    return state;
}

VkPipeline ShaderProgramHelper::CreateGraphicsPipeline(const RenderCore& _render, VkRenderPass _renderPass, VkPrimitiveTopology _topology, EVertexLayout _vertexLayoutType, VkPipelineLayout _pipelineLayout, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader /*= VK_NULL_HANDLE*/, VkShaderModule _tessellationControlShader /*= VK_NULL_HANDLE*/, VkShaderModule _tessellationEvaluatorShader /*= VK_NULL_HANDLE*/, VkShaderModule _geometryShader /*= VK_NULL_HANDLE*/)
{
    if (_vertexShader == VK_NULL_HANDLE)
    {
        return VK_NULL_HANDLE;
    }

    ShaderVertexLayout& vertexLayout = *m_vertexLayouts[_vertexLayoutType];

    VkPipelineVertexInputStateCreateInfo vertexInputState = vertexLayout.m_inputState;
    vertexInputState.vertexBindingDescriptionCount = (ionU32)vertexLayout.m_bindinggDescription.size();
    vertexInputState.pVertexBindingDescriptions = vertexLayout.m_bindinggDescription.data();
    vertexInputState.vertexAttributeDescriptionCount = (ionU32)vertexLayout.m_attributegDescription.size();
    vertexInputState.pVertexAttributeDescriptions = vertexLayout.m_attributegDescription.data();

    VkPipelineInputAssemblyStateCreateInfo assemblyInputState = {};
    assemblyInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyInputState.topology = _topology;
    
    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.depthBiasEnable = (_stateBits & ERasterization_PolygonMode_Offset) != 0;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.frontFace = (_stateBits & ERasterization_Face_Clockwise) ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.polygonMode = (_stateBits & ERasterization_PolygonMode_Line) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;

    switch (_stateBits & ECullingMode_Bits)
    {
    case ECullingMode_TwoSide:
        rasterizationState.cullMode = VK_CULL_MODE_NONE;
        break;
    case ECullingMode_Back:
        if (_stateBits & ERasterization_View_Specular)
        {
            rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
        }
        else 
        {
            rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        }
        break;
    case ECullingMode_Front:
    default:
        if (_stateBits & ERasterization_View_Specular)
        {
            rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        }
        else 
        {
            rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
        }
        break;
    }

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    {
        VkBlendFactor srcFactor = VK_BLEND_FACTOR_ONE;
        switch (_stateBits & EBlendState_SourceBlend_Bits)
        {
        case EBlendState_SourceBlend_Zero:                    srcFactor = VK_BLEND_FACTOR_ZERO; break;
        case EBlendState_SourceBlend_One:                    srcFactor = VK_BLEND_FACTOR_ONE; break;
        case EBlendState_SourceBlend_Dest_Color:                srcFactor = VK_BLEND_FACTOR_DST_COLOR; break;
        case EBlendState_SourceBlend_One_Minus_Dest_Color:    srcFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; break;
        case EBlendState_SourceBlend_Source_Alpha:            srcFactor = VK_BLEND_FACTOR_SRC_ALPHA; break;
        case EBlendState_SourceBlend_One_Minus_Source_Alpha:    srcFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; break;
        case EBlendState_SourceBlend_Dest_Alpha:                srcFactor = VK_BLEND_FACTOR_DST_ALPHA; break;
        case EBlendState_SourceBlend_One_Minus_Dest_Alpha:    srcFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; break;
        }

        VkBlendFactor dstFactor = VK_BLEND_FACTOR_ZERO;
        switch (_stateBits & EBlendState_DestBlend_Bits)
        {
        case EBlendState_DestBlend_Zero:                        dstFactor = VK_BLEND_FACTOR_ZERO; break;
        case EBlendState_DestBlend_One:                        dstFactor = VK_BLEND_FACTOR_ONE; break;
        case EBlendState_DestBlend_Source_Color:                dstFactor = VK_BLEND_FACTOR_SRC_COLOR; break;
        case EBlendState_DestBlend_One_Minus_Source_Color:    dstFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; break;
        case EBlendState_DestBlend_Source_Alpha:                dstFactor = VK_BLEND_FACTOR_SRC_ALPHA; break;
        case EBlendState_DestBlend_One_Minus_Source_Alpha:    dstFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; break;
        case EBlendState_DestBlend_Dest_Alpha:                dstFactor = VK_BLEND_FACTOR_DST_ALPHA; break;
        case EBlendState_DestBlend_One_Minus_Dest_Alpha:        dstFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; break;
        }

        VkBlendOp blendOp = VK_BLEND_OP_ADD;
        switch (_stateBits & EBlendOperator_Bits)
        {
        case EBlendOperator_Min: blendOp = VK_BLEND_OP_MIN; break;
        case EBlendOperator_Max: blendOp = VK_BLEND_OP_MAX; break;
        case EBlendOperator_Add: blendOp = VK_BLEND_OP_ADD; break;
        case EBlendOperator_Sub: blendOp = VK_BLEND_OP_SUBTRACT; break;
        }

        colorBlendAttachmentState.blendEnable = (srcFactor != VK_BLEND_FACTOR_ONE || dstFactor != VK_BLEND_FACTOR_ZERO);
        colorBlendAttachmentState.colorBlendOp = blendOp;
        colorBlendAttachmentState.srcColorBlendFactor = srcFactor;
        colorBlendAttachmentState.dstColorBlendFactor = dstFactor;
        colorBlendAttachmentState.alphaBlendOp = blendOp;
        colorBlendAttachmentState.srcAlphaBlendFactor = srcFactor;
        colorBlendAttachmentState.dstAlphaBlendFactor = dstFactor;

        colorBlendAttachmentState.colorWriteMask = 0;
        colorBlendAttachmentState.colorWriteMask |= (_stateBits & EColorMask_Red) ? 0 : VK_COLOR_COMPONENT_R_BIT;
        colorBlendAttachmentState.colorWriteMask |= (_stateBits & EColorMask_Green) ? 0 : VK_COLOR_COMPONENT_G_BIT;
        colorBlendAttachmentState.colorWriteMask |= (_stateBits & EColorMask_Blue) ? 0 : VK_COLOR_COMPONENT_B_BIT;
        colorBlendAttachmentState.colorWriteMask |= (_stateBits & EColorMask_Alpha) ? 0 : VK_COLOR_COMPONENT_A_BIT;
    }

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    {
        VkCompareOp depthCompareOp = VK_COMPARE_OP_ALWAYS;
        switch (_stateBits & EDepthFunction_Bits)
        {
        case EDepthFunction_Equal:        depthCompareOp = VK_COMPARE_OP_EQUAL; break;
        case EDepthFunction_Always:        depthCompareOp = VK_COMPARE_OP_ALWAYS; break;
        case EDepthFunction_Less:        depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; break;
        case EDepthFunction_Greater:    depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; break;
        }

        VkCompareOp stencilFrontCompareOp = VK_COMPARE_OP_ALWAYS;
        switch (_stateBits & EStencilFrontFunction_Bits)
        {
        case EStencilFrontFunction_Never:            stencilFrontCompareOp = VK_COMPARE_OP_NEVER; break;
        case EStencilFrontFunction_Lesser:            stencilFrontCompareOp = VK_COMPARE_OP_LESS; break;
        case EStencilFrontFunction_Equal:            stencilFrontCompareOp = VK_COMPARE_OP_EQUAL; break;
        case EStencilFrontFunction_LesserOrEqual:    stencilFrontCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; break;
        case EStencilFrontFunction_Greater:            stencilFrontCompareOp = VK_COMPARE_OP_GREATER; break;
        case EStencilFrontFunction_NotEqual:        stencilFrontCompareOp = VK_COMPARE_OP_NOT_EQUAL; break;
        case EStencilFrontFunction_GreaterOrEqual:    stencilFrontCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; break;
        case EStencilFrontFunction_Always:            stencilFrontCompareOp = VK_COMPARE_OP_ALWAYS; break;
        }

        VkCompareOp stencilBackCompareOp = VK_COMPARE_OP_ALWAYS;
        switch (_stateBits & EStencilBackFunction_Bits)
        {
        case EStencilBackFunction_Never:            stencilBackCompareOp = VK_COMPARE_OP_NEVER; break;
        case EStencilBackFunction_Lesser:            stencilBackCompareOp = VK_COMPARE_OP_LESS; break;
        case EStencilBackFunction_Equal:            stencilBackCompareOp = VK_COMPARE_OP_EQUAL; break;
        case EStencilBackFunction_LesserOrEqual:    stencilBackCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; break;
        case EStencilBackFunction_Greater:            stencilBackCompareOp = VK_COMPARE_OP_GREATER; break;
        case EStencilBackFunction_NotEqual:        stencilBackCompareOp = VK_COMPARE_OP_NOT_EQUAL; break;
        case EStencilBackFunction_GreaterOrEqual:    stencilBackCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; break;
        case EStencilBackFunction_Always:            stencilBackCompareOp = VK_COMPARE_OP_ALWAYS; break;
        }

        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.depthTestEnable = (_stateBits & EColorMask_Depth) == 0;   //VK_TRUE;
        depthStencilState.depthWriteEnable = (_stateBits & EColorMask_Depth) == 0;  //(_stateBits & EColorMask_Depth) == 0;
        depthStencilState.depthCompareOp = depthCompareOp;

        if (_render.GetGPU().m_vkPhysicalDevFeatures.depthBounds)
        {
            depthStencilState.depthBoundsTestEnable = (_stateBits & ERasterization_DepthTest_Mask) != 0;
            depthStencilState.minDepthBounds = 0.0f;
            depthStencilState.maxDepthBounds = 1.0f;
        }
        depthStencilState.stencilTestEnable = (_stateBits & (EStencilFrontFunction_Bits | EStencilOperator_Bits)) != 0;

        ionU32 ref = ionU32((_stateBits & EStencilFunctionReference_RefBits) >> EStencilFunctionReference_RefShift);
        ionU32 mask = ionU32((_stateBits & EStencilFunctionReference_MaskBits) >> EStencilFunctionReference_MaskShift);

        if (_stateBits & EStencilSeparate_Stencil)
        {
            depthStencilState.front = GetStencilOpState(_stateBits & EStencilFrontOperator_Bits);
            depthStencilState.front.writeMask = 0xFFFFFFFF;
            depthStencilState.front.compareOp = stencilFrontCompareOp;
            depthStencilState.front.compareMask = mask;
            depthStencilState.front.reference = ref;

            depthStencilState.back = GetStencilOpState((_stateBits & EStencilBackOperator_Bits) >> 12);
            depthStencilState.back.writeMask = 0xFFFFFFFF;
            depthStencilState.back.compareOp = stencilBackCompareOp;
            depthStencilState.back.compareMask = mask;
            depthStencilState.back.reference = ref;
        }
        else
        {
            depthStencilState.front = GetStencilOpState(_stateBits);
            depthStencilState.front.writeMask = 0xFFFFFFFF;
            depthStencilState.front.compareOp = stencilFrontCompareOp;
            depthStencilState.front.compareMask = mask;
            depthStencilState.front.reference = ref;
            depthStencilState.back = depthStencilState.front;
        }
       
    }
    // IMPORTANT: These two lines worked for PBR opaque! Keep in mind!
    //depthStencilState.front = depthStencilState.back;
    //depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS; 

    
    VkPipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = _render.GetSampleCount();
    // cause a pipeline compilation bug on some GPU
    //if (_render.GetUsesSuperSampling())
    //{
    //    multisampleState.sampleShadingEnable = VK_TRUE;
    //    multisampleState.minSampleShading = 1.0f;
    //}

    eosVector(VkPipelineShaderStageCreateInfo) stages;
    VkPipelineShaderStageCreateInfo stage = {};
    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.pName = "main";

    {
        stage.module = _vertexShader;
        stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages.push_back(stage);
    }

    if (_tessellationControlShader != VK_NULL_HANDLE)
    {
        stage.module = _tessellationControlShader;
        stage.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        stages.push_back(stage);
    }

    if (_tessellationEvaluatorShader != VK_NULL_HANDLE)
    {
        stage.module = _tessellationEvaluatorShader;
        stage.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        stages.push_back(stage);
    }

    if (_geometryShader != VK_NULL_HANDLE)
    {
        stage.module = _geometryShader;
        stage.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        stages.push_back(stage);
    }

    if (_fragmentShader != VK_NULL_HANDLE)
    {
        stage.module = _fragmentShader;
        stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages.push_back(stage);
    }

    eosVector(VkDynamicState) dynamic;
    dynamic.push_back(VK_DYNAMIC_STATE_SCISSOR);
    dynamic.push_back(VK_DYNAMIC_STATE_VIEWPORT);

    if (_stateBits & ERasterization_PolygonMode_Offset)
    {
        dynamic.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
    }

    if (_render.GetGPU().m_vkPhysicalDevFeatures.depthBounds && (_stateBits & ERasterization_DepthTest_Mask))
    {
        dynamic.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
    }

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<ionU32>(dynamic.size());
    dynamicState.pDynamicStates = dynamic.data();

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // Pipeline Create
    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.layout = _pipelineLayout;
    createInfo.renderPass = _renderPass;
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &assemblyInputState;
    createInfo.pRasterizationState = &rasterizationState;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDepthStencilState = &depthStencilState;
    createInfo.pMultisampleState = &multisampleState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.pViewportState = &viewportState;
    createInfo.stageCount = (ionU32)stages.size();
    createInfo.pStages = stages.data();

    VkPipeline pipeline = VK_NULL_HANDLE;

    VkResult result = vkCreateGraphicsPipelines(_render.GetDevice(), _render.GetPipelineCache(), 1, &createInfo, vkMemory, &pipeline);
    ionAssertReturnValue(result == VK_SUCCESS, "Cannot create the graphic pipeline!", VK_NULL_HANDLE);

    return pipeline;
}


ION_NAMESPACE_END