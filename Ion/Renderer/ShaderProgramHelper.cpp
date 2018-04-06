#include "ShaderProgramHelper.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"


#include "ShaderProgram.h"
#include "RenderState.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ShaderVertexLayout ShaderProgramHelper::m_vertexLayouts[EVertexLayout::EVertexLayout_Count];

void ShaderProgramHelper::CreateVertexDescriptor()
{
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    VkVertexInputBindingDescription binding = {};
    VkVertexInputAttributeDescription attribute = {};
    
    {
        ShaderVertexLayout& vertexLayout = m_vertexLayouts[EVertexLayout::EVertexLayout_Vertices];

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

        // TexCoord
        attribute.format = VK_FORMAT_R32G32_SFLOAT;         //VK_FORMAT_R16G16_SFLOAT;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_textureCoordUV);

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

        // Color1
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_color1);

        // Color2
        attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
        attribute.location = locationIndex++;
        attribute.offset = locationOffset;
        vertexLayout.m_attributegDescription.push_back(attribute);
        locationOffset += sizeof(Vertex::m_color2); // UNUSED because anything next! I keep it just to check at runtime!  
    }


    {
        ShaderVertexLayout& vertexLayout = m_vertexLayouts[EVertexLayout::EVertexLayout_Vertices_Plain];

        vertexLayout.m_inputState = createInfo;

        binding.stride = sizeof(PlainVertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexLayout.m_bindinggDescription.push_back(binding);

        // Position
        attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attribute.location = 0;
        attribute.offset = 0;
        vertexLayout.m_attributegDescription.push_back(attribute);
    }
}

// _pools MUST BE RESIDED BEFORE PASSED HERE TO ONE OF THE ENUM VALUE OF ERenderType !!!
void ShaderProgramHelper::CreateDescriptorPools(const VkDevice& _device, eosVector(VkDescriptorPool)& _pools)
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

    const ionSize size = _pools.size();
    for (ionSize i = 0; i < size; ++i)
    {
        VkResult result = vkCreateDescriptorPool(_device, &createInfo, vkMemory, &_pools[i]);
        ionAssertReturnVoid(result == VK_SUCCESS, "vkCreateDescriptorPool cannot create descriptor pool!");
    }
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

void ShaderProgramHelper::CreateDescriptorSetLayout(const VkDevice& _device, ShaderProgram& _shaderProgram, const Shader& _vertexShader, const Shader& _fragmentShader, const Shader& _tessellationControlShader, const Shader& _tessellationEvaluatorShader, const Shader& _geometryShader)
{
    // Descriptor Set Layout
    {
        eosVector(VkDescriptorSetLayoutBinding) layoutBindings;
        VkDescriptorSetLayoutBinding binding = {};
        binding.descriptorCount = 1;

        ionU32 bindingId = 0;


        if (_vertexShader.IsValid())
        {
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            for (ionSize i = 0; i < _vertexShader.m_bindings.size(); ++i)
            {
                binding.binding = ++bindingId;
                binding.descriptorType = GetDescriptorType(_vertexShader.m_bindings[i]);
                _shaderProgram.m_bindings.push_back(_vertexShader.m_bindings[i]);

                layoutBindings.push_back(binding);
            }
        }

        if (_tessellationControlShader.IsValid())
        {
            {
                binding.stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                for (ionSize i = 0; i < _tessellationControlShader.m_bindings.size(); ++i)
                {
                    binding.binding = ++bindingId;
                    binding.descriptorType = GetDescriptorType(_tessellationControlShader.m_bindings[i]);
                    _shaderProgram.m_bindings.push_back(_tessellationControlShader.m_bindings[i]);

                    layoutBindings.push_back(binding);
                }
            }
        }

        if (_tessellationEvaluatorShader.IsValid())
        {
            {
                binding.stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                for (ionSize i = 0; i < _tessellationEvaluatorShader.m_bindings.size(); ++i)
                {
                    binding.binding = ++bindingId;
                    binding.descriptorType = GetDescriptorType(_tessellationEvaluatorShader.m_bindings[i]);
                    _shaderProgram.m_bindings.push_back(_tessellationEvaluatorShader.m_bindings[i]);

                    layoutBindings.push_back(binding);
                }
            }
        }

        if (_geometryShader.IsValid())
        {
            {
                binding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
                for (ionSize i = 0; i < _geometryShader.m_bindings.size(); ++i)
                {
                    binding.binding = ++bindingId;
                    binding.descriptorType = GetDescriptorType(_geometryShader.m_bindings[i]);
                    _shaderProgram.m_bindings.push_back(_geometryShader.m_bindings[i]);

                    layoutBindings.push_back(binding);
                }
            }
        }

        if (_fragmentShader.IsValid())
        {
            binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            for (ionSize i = 0; i < _fragmentShader.m_bindings.size(); ++i)
            {
                binding.binding = bindingId++;
                binding.descriptorType = GetDescriptorType(_fragmentShader.m_bindings[i]);
                _shaderProgram.m_bindings.push_back(_fragmentShader.m_bindings[i]);

                layoutBindings.push_back(binding);
            }
        }

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = (ionU32)layoutBindings.size();
        createInfo.pBindings = layoutBindings.data();

        vkCreateDescriptorSetLayout(_device, &createInfo, vkMemory, &_shaderProgram.m_descriptorSetLayout);
    }


    // Pipeline Layout
    {
        VkPipelineLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.setLayoutCount = 1;
        createInfo.pSetLayouts = &_shaderProgram.m_descriptorSetLayout;

        vkCreatePipelineLayout(_device, &createInfo, vkMemory, &_shaderProgram.m_pipelineLayout);
    }
}

VkStencilOpState ShaderProgramHelper::GetStencilOpState(ionU64 _stencilStateBits)
{
    VkStencilOpState state = {};


    switch (_stencilStateBits & EStencilFrontOperator_Fail_Bits)
    {
    case EStencilFrontOperator_Fail_Keep:		    state.failOp = VK_STENCIL_OP_KEEP; break;
    case EStencilFrontOperator_Fail_Zero:		    state.failOp = VK_STENCIL_OP_ZERO; break;
    case EStencilFrontOperator_Fail_Replace:	    state.failOp = VK_STENCIL_OP_REPLACE; break;
    case EStencilFrontOperator_Fail_Increment:	    state.failOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Fail_Decrement:	    state.failOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Fail_Invert:	        state.failOp = VK_STENCIL_OP_INVERT; break;
    case EStencilFrontOperator_Fail_Increment_Wrap: state.failOp = VK_STENCIL_OP_INCREMENT_AND_WRAP; break;
    case EStencilFrontOperator_Fail_Decrement_Wrap: state.failOp = VK_STENCIL_OP_DECREMENT_AND_WRAP; break;
    }

    switch (_stencilStateBits & EStencilFrontOperator_ZFail_Bits)
    {
    case EStencilFrontOperator_ZFail_Keep:		        state.depthFailOp = VK_STENCIL_OP_KEEP; break;
    case EStencilFrontOperator_ZFail_Zero:		        state.depthFailOp = VK_STENCIL_OP_ZERO; break;
    case EStencilFrontOperator_ZFail_Replace:	        state.depthFailOp = VK_STENCIL_OP_REPLACE; break;
    case EStencilFrontOperator_ZFail_Increment:		    state.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_ZFail_Decrement:		    state.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_ZFail_Invert:	        state.depthFailOp = VK_STENCIL_OP_INVERT; break;
    case EStencilFrontOperator_ZFail_Increment_Wrap:    state.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_WRAP; break;
    case EStencilFrontOperator_ZFail_Decrement_Wrap:    state.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_WRAP; break;
    }

    switch (_stencilStateBits & EStencilFrontOperator_Pass_Bits)
    {
    case EStencilFrontOperator_Pass_Keep:		    state.passOp = VK_STENCIL_OP_KEEP; break;
    case EStencilFrontOperator_Pass_Zero:		    state.passOp = VK_STENCIL_OP_ZERO; break;
    case EStencilFrontOperator_Pass_Replace:	    state.passOp = VK_STENCIL_OP_REPLACE; break;
    case EStencilFrontOperator_Pass_Increment:		state.passOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Pass_Decrement:		state.passOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP; break;
    case EStencilFrontOperator_Pass_Invert:	        state.passOp = VK_STENCIL_OP_INVERT; break;
    case EStencilFrontOperator_Pass_Increment_Wrap:	state.passOp = VK_STENCIL_OP_INCREMENT_AND_WRAP; break;
    case EStencilFrontOperator_Pass_Decrement_Wrap:	state.passOp = VK_STENCIL_OP_DECREMENT_AND_WRAP; break;
    }

    return state;
}

VkPipeline ShaderProgramHelper::CreateGraphicsPipeline(VkPipelineLayout _pipelineLayout, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationControlShader  /*= VK_NULL_HANDLE*/, VkShaderModule _tessellationEvaluatorShader /*= VK_NULL_HANDLE*/, VkShaderModule _geometryShader /*= VK_NULL_HANDLE*/)
{
    return VK_NULL_HANDLE;
}


ION_NAMESPACE_END