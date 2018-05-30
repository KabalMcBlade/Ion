#include "ShaderProgramManager.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "ShaderProgramHelper.h"
#include "../Renderer/RenderCore.h"

#include "../Renderer/VertexCacheManager.h"
#include "../Texture/Texture.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN


ShaderProgramManager *ShaderProgramManager::s_instance = nullptr;


ShaderProgramManager::ShaderProgramManager() :
    m_current(0),
    m_counter(0),
    m_currentData(0),
    m_currentDescSet(0),
    m_currentParmBufferOffset(0)
{
    memset(m_parmBuffers, 0, sizeof(m_parmBuffers));
}

ShaderProgramManager::~ShaderProgramManager()
{

}

void ShaderProgramManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(ShaderProgramManager, EOS_MEMORY_ALIGNMENT_SIZE);
    }
}

void ShaderProgramManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

ShaderProgramManager& ShaderProgramManager::Instance()
{
    return *s_instance;
}

ionBool ShaderProgramManager::Init(VkDevice _vkDevice, const eosString& _shaderFolderPath)
{
    m_vkDevice = _vkDevice;
    m_shaderFolderPath = _shaderFolderPath;

    ShaderProgramHelper::CreateVertexDescriptor();

    ShaderProgramHelper::CreateDescriptorPools(m_vkDevice, m_descriptorPools);

    for (ionU32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
    {
        m_parmBuffers[i] = eosNew(UniformBuffer, EOS_MEMORY_ALIGNMENT_SIZE);
        m_parmBuffers[i]->Alloc(m_vkDevice, nullptr, ION_MAX_DESCRIPTOR_SETS * ION_MAX_DESCRIPTOR_SET_UNIFORMS * sizeof(Vector), EBufferUsage_Dynamic);
    }

    m_skinningUniformBuffer = eosNew(UniformBuffer, EOS_MEMORY_ALIGNMENT_SIZE);
    m_skinningUniformBuffer->Alloc(m_vkDevice, nullptr, sizeof(Vector), EBufferUsage_Dynamic);

    return true;
}

void ShaderProgramManager::Shutdown()
{
    for (ionSize i = 0; i < m_shaders.size(); ++i) 
    {
        Shader& shader = m_shaders[i];
        vkDestroyShaderModule(m_vkDevice, shader.m_shaderModule, vkMemory);
        shader.m_shaderModule = VK_NULL_HANDLE;
    }

    for (ionSize i = 0; i < m_shaderPrograms.size(); ++i)
    {
        ShaderProgram& shaderProgram = m_shaderPrograms[i];

        for (ionSize j = 0; j < shaderProgram.m_pipelines.size(); ++j)
        {
            vkDestroyPipeline(m_vkDevice, shaderProgram.m_pipelines[j].m_pipeline, vkMemory);
        }
        shaderProgram.m_pipelines.clear();

        vkDestroyDescriptorSetLayout(m_vkDevice, shaderProgram.m_descriptorSetLayout, vkMemory);
        vkDestroyPipelineLayout(m_vkDevice, shaderProgram.m_pipelineLayout, vkMemory);
    }
    m_shaderPrograms.clear();

    for (ionU32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
    {
        m_parmBuffers[i]->Free();
        eosDelete(m_parmBuffers[i]);
        m_parmBuffers[i] = nullptr;
    }

    m_skinningUniformBuffer->Free();
    eosDelete(m_skinningUniformBuffer);

    for (ionU32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
    {
        //vkFreeDescriptorSets( m_vkDevice, m_descriptorPools[i], ION_MAX_DESCRIPTOR_SETS, m_descriptorSets[i]);
        vkResetDescriptorPool(m_vkDevice, m_descriptorPools[i], 0);
        vkDestroyDescriptorPool(m_vkDevice, m_descriptorPools[i], vkMemory);
    }

    memset(m_descriptorSets, 0, sizeof(m_descriptorSets));
    memset(m_descriptorPools, 0, sizeof(m_descriptorPools));

    m_counter = 0;
    m_currentData = 0;
    m_currentDescSet = 0;
}

const Vector& ShaderProgramManager::GetRenderParmVector(const eosString& _param)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    return GetRenderParmVector(hash);
}

// if parameter not found, return a vector 0 and create this new hash! BE CAREFUL!
const Vector& ShaderProgramManager::GetRenderParmVector(ionSize _paramHash)
{
    return m_uniformsVector[_paramHash];
}

const Matrix& ShaderProgramManager::GetRenderParmMatrix(const eosString& _param)
{
	const ionSize hash = std::hash<eosString>{}(_param);
	return GetRenderParmMatrix(hash);
}

const Matrix& ShaderProgramManager::GetRenderParmMatrix(ionSize _paramHash)
{
	return m_uniformsMatrix[_paramHash];
}

void ShaderProgramManager::SetRenderParmVector(const eosString& _param, const ionFloat* _value)
{
    const ionSize hash = std::hash<eosString>{}(_param);
	SetRenderParmVector(hash, _value);
}

void ShaderProgramManager::SetRenderParmVector(ionSize _paramHash, const ionFloat* _value)
{
    Vector v(_value[0], _value[1], _value[2], _value[3]);
	m_uniformsVector[_paramHash] = v;
}

void ShaderProgramManager::SetRenderParmsVector(const eosString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const eosString indexParam(std::to_string(i).c_str());
        const eosString fullParam = _param + indexParam;
        const ionSize hash = std::hash<eosString>{}(fullParam);
        SetRenderParmVector(hash, _values + (i * 4));
    }
}

void ShaderProgramManager::SetRenderParmsVector(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParmVector(_paramHash, _values + (i * 4));
    }
}

void ShaderProgramManager::SetRenderParmMatrix(const eosString& _param, const ionFloat* _value)
{
	const ionSize hash = std::hash<eosString>{}(_param);
	SetRenderParmMatrix(hash, _value);
}

void ShaderProgramManager::SetRenderParmMatrix(ionSize _paramHash, const ionFloat* _value)
{
	Matrix m(_value[0], _value[1], _value[2], _value[3], _value[4], _value[5], _value[6], _value[7], _value[8], _value[9], _value[10], _value[11], _value[12], _value[13], _value[14], _value[15]);
	m_uniformsMatrix[_paramHash] = m;
}

void ShaderProgramManager::SetRenderParmsMatrix(const eosString& _param, const ionFloat* _values, ionU32 _numValues)
{
	for (ionU32 i = 0; i < _numValues; ++i)
	{
		const eosString indexParam(std::to_string(i).c_str());
		const eosString fullParam = _param + indexParam;
		const ionSize hash = std::hash<eosString>{}(fullParam);
		SetRenderParmMatrix(hash, _values + (i * 16));
	}
}

void ShaderProgramManager::SetRenderParmsMatrix(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
	for (ionU32 i = 0; i < _numValues; ++i)
	{
		SetRenderParmMatrix(_paramHash, _values + (i * 16));
	}
}


void ShaderProgramManager::StartFrame()
{
    ++m_counter;
    m_currentData = m_counter % ION_FRAME_DATA_COUNT;
    m_currentDescSet = 0;
    m_currentParmBufferOffset = 0;

    vkResetDescriptorPool(m_vkDevice, m_descriptorPools[m_currentData], 0);
}

void ShaderProgramManager::BindProgram(ionS32 _index)
{
    if (m_current != _index) 
    {
        m_current = _index;
    }
}

void ShaderProgramManager::CommitCurrent(const RenderCore& _render, ionU64 _stateBits, VkCommandBuffer _commandBuffer)
{
    ShaderProgram& shaderProgram = m_shaderPrograms[m_current];

    VkPipeline pipeline = shaderProgram.GetPipeline(_render, _stateBits, 
        m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderModule,
        shaderProgram.m_fragmentShaderIndex != -1 ? m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderModule : VK_NULL_HANDLE,
        shaderProgram.m_tessellationControlShaderIndex != -1 ? m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderModule : VK_NULL_HANDLE,
        shaderProgram.m_tessellationEvaluatorShaderIndex != -1 ? m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderModule : VK_NULL_HANDLE, 
        shaderProgram.m_geometryShaderIndex != -1 ? m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderModule : VK_NULL_HANDLE);

    VkDescriptorSetAllocateInfo setAllocInfo = {};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.pNext = nullptr;
    setAllocInfo.descriptorPool = m_descriptorPools[m_currentData];
    setAllocInfo.descriptorSetCount = 1;
    setAllocInfo.pSetLayouts = &shaderProgram.m_descriptorSetLayout;

    VkResult result = vkAllocateDescriptorSets(_render.GetDevice(), &setAllocInfo, &m_descriptorSets[m_currentData][m_currentDescSet]);
    ionAssertReturnVoid(result == VK_SUCCESS, "Cannot allocate the descriptor!");

    VkDescriptorSet descSet = m_descriptorSets[m_currentData][m_currentDescSet];
    ++m_currentDescSet;

    ionS32 writeIndex = 0;
    ionS32 bufferIndex = 0;
    ionS32 imageIndex = 0;
    ionS32 bindingIndex = 0;

    VkWriteDescriptorSet writes[ION_MAX_DESCRIPTOR_SET_WRITES];
    VkDescriptorBufferInfo bufferInfos[ION_MAX_DESCRIPTOR_SET_WRITES];
    VkDescriptorImageInfo imageInfos[ION_MAX_DESCRIPTOR_SET_WRITES];

    memset(&writes, 0, sizeof(writes));
    memset(&bufferInfos, 0, sizeof(bufferInfos));
    memset(&imageInfos, 0, sizeof(imageInfos));

    ionS32 uboIndex = 0;
    UniformBuffer* ubos[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    UniformBuffer vertParms;
    if (shaderProgram.m_vertexShaderIndex > -1 && m_shaders[shaderProgram.m_vertexShaderIndex].m_parametersHash.size() > 0)
    {
        AllocParametersBlockBuffer(_render, m_shaders[shaderProgram.m_vertexShaderIndex].m_parametersHash, m_shaders[shaderProgram.m_vertexShaderIndex].m_parameterType, vertParms);

        ubos[uboIndex++] = &vertParms;
    }

    UniformBuffer jointBuffer;
    if (shaderProgram.m_usesJoints && _render.GetJointCacheHandler() > 0)
    {
        if (!ionVertexCacheManager().GetJointBuffer(_render.GetJointCacheHandler(), &jointBuffer))
        {
            ionAssertReturnVoid(false, "CommitCurrent: The jointBuffer is nullptr");
            return;
        }
        ionAssertReturnVoid((jointBuffer.GetOffset() & (_render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1)) == 0, "Error in the joint buffer");

        ubos[uboIndex++] = &jointBuffer;
    }
    else if (shaderProgram.m_usesSkinning)
    {
        ubos[uboIndex++] = m_skinningUniformBuffer;
    }

    UniformBuffer tessCtrlParms;
    if (shaderProgram.m_tessellationControlShaderIndex > -1 && m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_parametersHash.size() > 0)
    {
        AllocParametersBlockBuffer(_render, m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_parametersHash, m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_parameterType, tessCtrlParms);

        ubos[uboIndex++] = &tessCtrlParms;
    }

    UniformBuffer tessEvalParms;
    if (shaderProgram.m_tessellationEvaluatorShaderIndex > -1 && m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_parametersHash.size() > 0)
    {
        AllocParametersBlockBuffer(_render, m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_parametersHash, m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_parameterType, tessEvalParms);

        ubos[uboIndex++] = &tessEvalParms;
    }

    UniformBuffer geometryParms;
    if (shaderProgram.m_geometryShaderIndex > -1 && m_shaders[shaderProgram.m_geometryShaderIndex].m_parametersHash.size() > 0)
    {
        AllocParametersBlockBuffer(_render, m_shaders[shaderProgram.m_geometryShaderIndex].m_parametersHash, m_shaders[shaderProgram.m_geometryShaderIndex].m_parameterType, geometryParms);

        ubos[uboIndex++] = &geometryParms;
    }

    UniformBuffer fragParms;
    if (shaderProgram.m_fragmentShaderIndex > -1 && m_shaders[shaderProgram.m_fragmentShaderIndex].m_parametersHash.size() > 0)
    {
        AllocParametersBlockBuffer(_render, m_shaders[shaderProgram.m_fragmentShaderIndex].m_parametersHash, m_shaders[shaderProgram.m_fragmentShaderIndex].m_parameterType, fragParms);

        ubos[uboIndex++] = &fragParms;
    }


    for (ionSize i = 0; i < shaderProgram.m_bindings.size(); ++i)
    {
        EShaderBinding binding = shaderProgram.m_bindings[i];

        switch (binding) 
        {
        case EShaderBinding_Uniform:
        {
            UniformBuffer* ubo = ubos[bufferIndex];

            VkDescriptorBufferInfo & bufferInfo = bufferInfos[bufferIndex++];
            memset(&bufferInfo, 0, sizeof(VkDescriptorBufferInfo));
            bufferInfo.buffer = ubo->GetObject();
            bufferInfo.offset = ubo->GetOffset();
            bufferInfo.range = ubo->GetSize();

            VkWriteDescriptorSet & write = writes[writeIndex++];
            memset(&write, 0, sizeof(VkWriteDescriptorSet));
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descSet;
            write.dstBinding = bindingIndex++;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo = &bufferInfo;

            break;
        }
        case EShaderBinding_Sampler:
        {
            const Texture* image = _render.GetTextureParam(imageIndex);

            VkDescriptorImageInfo & imageInfo = imageInfos[imageIndex++];
            memset(&imageInfo, 0, sizeof(VkDescriptorImageInfo));
            imageInfo.imageLayout = image->GetLayout();
            imageInfo.imageView = image->GetView();
            imageInfo.sampler = image->GetSampler();

            ionAssertReturnVoid(image->GetView() != VK_NULL_HANDLE, "View is null!");

            VkWriteDescriptorSet & write = writes[writeIndex++];
            memset(&write, 0, sizeof(VkWriteDescriptorSet));
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descSet;
            write.dstBinding = bindingIndex++;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.pImageInfo = &imageInfo;

            break;
        }
        }
    }

    vkUpdateDescriptorSets(m_vkDevice, writeIndex, writes, 0, nullptr);

    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderProgram.m_pipelineLayout, 0, 1, &descSet, 0, nullptr);
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void ShaderProgramManager::AllocParametersBlockBuffer(const RenderCore& _render, const eosVector(ionSize) & paramsHash, const eosVector(EUniformParameterType) & paramsType, UniformBuffer& _ubo)
{
    // gather all informations
    static const ionU32 sMaxParamsAmount = 32;
    ionSize counterForType[EUniformParameterType_Count];
    ionSize indexForType[EUniformParameterType_Count][sMaxParamsAmount];

    memset(&counterForType, 0, sizeof(counterForType));
    memset(&indexForType, 0, sizeof(indexForType));

    const ionSize numParmsType = paramsType.size();
    for (ionSize i = 0; i < numParmsType; ++i)
    {
        const EUniformParameterType type = paramsType[i];
        if (type == EUniformParameterType_Vector)
        {
            indexForType[EUniformParameterType_Vector][counterForType[EUniformParameterType_Vector]] = i;
            ++counterForType[EUniformParameterType_Vector];
        }
        else
        {
            indexForType[EUniformParameterType_Matrix][counterForType[EUniformParameterType_Matrix]] = i;
            ++counterForType[EUniformParameterType_Matrix];
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // Matrix 
    ionSize numParms = counterForType[EUniformParameterType_Matrix];

    if (numParms > 0)
    {
        ionSize size = numParms * sizeof(Matrix);
        ionSize mask = _render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1;
        ionSize alignedSize = (size + mask) & ~mask;

        _ubo.ReferenceTo(*m_parmBuffers[m_currentData], m_currentParmBufferOffset, alignedSize);

        Matrix* uniforms = (Matrix*)_ubo.MapBuffer(EBufferMappingType_Write);

        for (ionSize i = 0; i < numParms; ++i)
        {
            uniforms[i] = GetRenderParmMatrix(paramsHash[indexForType[EUniformParameterType_Matrix][i]]);
        }

        _ubo.UnmapBuffer();

        m_currentParmBufferOffset += alignedSize;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // Vector 
    numParms = counterForType[EUniformParameterType_Vector];

    if (numParms > 0)
    {
        ionSize size = numParms * sizeof(Vector);
        ionSize mask = _render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1;
        ionSize alignedSize = (size + mask) & ~mask;

        _ubo.ReferenceTo(*m_parmBuffers[m_currentData], m_currentParmBufferOffset, alignedSize);

        Vector* uniforms = (Vector*)_ubo.MapBuffer(EBufferMappingType_Write);

        for (ionSize i = 0; i < numParms; ++i)
        {
            uniforms[i] = GetRenderParmVector(paramsHash[indexForType[EUniformParameterType_Vector][i]]);
        }

        _ubo.UnmapBuffer();

        m_currentParmBufferOffset += alignedSize;
    }
}

ionS32 ShaderProgramManager::FindShader(const eosString& _name, EShaderStage _stage, const ShaderLayoutDef& _defines)
{
    for (ionS32 i = 0; i < m_shaders.size(); ++i)
    {
        Shader& shader = m_shaders[i];
        if (shader.m_name == _name && shader.m_stage == _stage)
        {
            LoadShader(i, _defines);
            return i;
        }
    }

    Shader shader;
    shader.m_name = _name;
    shader.m_stage = _stage;
    m_shaders.push_back(shader);
    ionS32 index = (ionS32)(m_shaders.size() - 1);
    LoadShader(index, _defines);
    return index;
}

void ShaderProgramManager::LoadShader(ionS32 _index, const ShaderLayoutDef& _defines)
{
    if (m_shaders[_index].m_shaderModule != VK_NULL_HANDLE)
    {
        return; // Already loaded
    }

    LoadShader(m_shaders[_index], _defines);
}

void ShaderProgramManager::LoadShader(Shader& _shader, const ShaderLayoutDef& _defines)
{
    eosString shaderPath = m_shaderFolderPath + _shader.m_name;

    // just direct set, not binary
    switch(_shader.m_stage)
    {
    case EShaderStage_Vertex:           shaderPath += ".vert.spv"; break;
    case EShaderStage_Tessellation_Ctrl:shaderPath += ".ctrl.spv"; break;
    case EShaderStage_Tessellation_Eval:shaderPath += ".eval.spv"; break;
    case EShaderStage_Geometry:         shaderPath += ".geom.spv"; break;
    case EShaderStage_Fragment:         shaderPath += ".frag.spv"; break;
    default:
        ionAssertReturnVoid(false, "Shader stage mismatch!");
        break;
    }

    std::ifstream fileStream;
    fileStream.open(shaderPath.c_str(), std::ios::in | std::ios::binary);

    ionAssertReturnVoid(fileStream.is_open(), "Failed to open shader file!");

    eosStringStream spirvBuffer;
    spirvBuffer << fileStream.rdbuf();

    fileStream.close();

    ionSize size = _defines.m_uniforms.size();
    for (ionSize i = 0; i < size; ++i)
    {
        const ionSize hash = std::hash<eosString>{}(_defines.m_uniforms[i]);
        _shader.m_parametersHash.push_back(hash);
		_shader.m_parameterType.push_back(_defines.m_uniformTypes[i]);
    }
     
    size = _defines.m_bindings.size();
    for (ionSize i = 0; i < size; ++i)
    {
        _shader.m_bindings.push_back(_defines.m_bindings[i]);
    }

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvBuffer.str().length();
    createInfo.pCode = (ionU32*)spirvBuffer.str().data();

    VkResult result = vkCreateShaderModule(m_vkDevice, &createInfo, vkMemory, &_shader.m_shaderModule);
    ionAssertReturnVoid(result == VK_SUCCESS, "Cannot create shader!");
}

ionS32 ShaderProgramManager::FindProgram(const eosString& _name, EVertexLayout _vertexLayout, ionS32 _vertexIndex, ionS32 _fragmentIndex /*= -1*/, ionS32 _tessellationControlIndex /*= -1*/, ionS32 _tessellationEvaluationIndex /*= -1*/, ionS32 _geometryIndex /*= -1*/, ionBool _useJoint /*= false*/, ionBool _useSkinning /*= false*/)
{
    for (ionSize i = 0; i < m_shaderPrograms.size(); ++i)
    {
        ShaderProgram& prog = m_shaderPrograms[i];
        if (prog.m_vertexShaderIndex == _vertexIndex && prog.m_fragmentShaderIndex == _fragmentIndex && prog.m_tessellationControlShaderIndex == _tessellationControlIndex && prog.m_tessellationEvaluatorShaderIndex == _tessellationEvaluationIndex && prog.m_geometryShaderIndex == _geometryIndex)
        {
            return (ionS32)i;
        }
    }

    ShaderProgram program;
    program.m_name = _name;
    program.m_vertexShaderIndex = _vertexIndex;
    program.m_fragmentShaderIndex = _fragmentIndex;
    program.m_tessellationControlShaderIndex = _tessellationControlIndex;
    program.m_tessellationEvaluatorShaderIndex = _tessellationEvaluationIndex;
    program.m_geometryShaderIndex = _geometryIndex;
    program.m_vertextLayoutType = _vertexLayout;
    program.m_usesJoints = _useJoint;
    program.m_usesSkinning = _useSkinning;

    // TODO:
    // If index is -1 for some shader, I have to manually pass an "invalid" shader to the next function!
    const ionSize shaderCount = m_shaders.size();
    const Shader& vertexShader = _vertexIndex > -1 && _vertexIndex < shaderCount ? m_shaders[_vertexIndex] : Shader();
    const Shader& fragmentShader = _fragmentIndex > -1 && _fragmentIndex < shaderCount ? m_shaders[_fragmentIndex] : Shader();
    const Shader& tessControlShader = _tessellationControlIndex > -1 && _tessellationControlIndex < shaderCount ? m_shaders[_tessellationControlIndex] : Shader();
    const Shader& tessEvalShader = _tessellationEvaluationIndex > -1 && _tessellationEvaluationIndex < shaderCount ? m_shaders[_tessellationEvaluationIndex] : Shader();
    const Shader& geometryShader = _geometryIndex > -1 && _geometryIndex < shaderCount ? m_shaders[_geometryIndex] : Shader();

    ShaderProgramHelper::CreateDescriptorSetLayout(m_vkDevice, program, vertexShader, fragmentShader, tessControlShader, tessEvalShader, geometryShader);

    // skinning here?

    m_shaderPrograms.push_back(program);

    const ionS32 index = (ionS32)(m_shaderPrograms.size() - 1);
    return index;
}

void ShaderProgramManager::Restart()
{
    for (ionSize i = 0; i < m_shaderPrograms.size(); ++i)
    {
        ShaderProgram& shaderProgram = m_shaderPrograms[i];

        for (ionSize j = 0; j < shaderProgram.m_pipelines.size(); ++j)
        {
            vkDestroyPipeline(m_vkDevice, shaderProgram.m_pipelines[j].m_pipeline, vkMemory);
        }
        shaderProgram.m_pipelines.clear();

        vkDestroyPipelineLayout(m_vkDevice, shaderProgram.m_pipelineLayout, vkMemory);
        m_shaderPrograms.clear();
    }
}


ION_NAMESPACE_END