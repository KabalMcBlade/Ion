#include "ShaderProgramManager.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/FileSystemManager.h"

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
    m_currentDescSet(0),
    m_currentParmBufferOffset(0)
{
}

ShaderProgramManager::~ShaderProgramManager()
{

}

void ShaderProgramManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(ShaderProgramManager, ION_MEMORY_ALIGNMENT_SIZE);
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

ionBool ShaderProgramManager::Init(VkDevice _vkDevice)
{
    m_vkDevice = _vkDevice;

    ShaderProgramHelper::CreateVertexDescriptor();

    ShaderProgramHelper::CreateDescriptorPools(m_vkDevice, m_descriptorPool);

    m_parmBufferMatrix = eosNew(UniformBuffer, ION_MEMORY_ALIGNMENT_SIZE);
    m_parmBufferMatrix->Alloc(m_vkDevice, nullptr, ION_MAX_DESCRIPTOR_SETS * ION_MAX_DESCRIPTOR_SET_UNIFORMS * sizeof(Matrix), EBufferUsage_Dynamic);

    m_parmBufferVector = eosNew(UniformBuffer, ION_MEMORY_ALIGNMENT_SIZE);
    m_parmBufferVector->Alloc(m_vkDevice, nullptr, ION_MAX_DESCRIPTOR_SETS * ION_MAX_DESCRIPTOR_SET_UNIFORMS * sizeof(Vector), EBufferUsage_Dynamic);

    m_parmBufferFloat = eosNew(UniformBuffer, ION_MEMORY_ALIGNMENT_SIZE);
    m_parmBufferFloat->Alloc(m_vkDevice, nullptr, ION_MAX_DESCRIPTOR_SETS * ION_MAX_DESCRIPTOR_SET_UNIFORMS * sizeof(ionFloat), EBufferUsage_Dynamic);

    m_parmBufferInteger = eosNew(UniformBuffer, ION_MEMORY_ALIGNMENT_SIZE);
    m_parmBufferInteger->Alloc(m_vkDevice, nullptr, ION_MAX_DESCRIPTOR_SETS * ION_MAX_DESCRIPTOR_SET_UNIFORMS * sizeof(ionS32), EBufferUsage_Dynamic);

    m_skinningUniformBuffer = eosNew(UniformBuffer, ION_MEMORY_ALIGNMENT_SIZE);
    m_skinningUniformBuffer->Alloc(m_vkDevice, nullptr, sizeof(Vector), EBufferUsage_Dynamic);

    return true;
}

void ShaderProgramManager::UnloadShader(ionS32 _index)
{
    ionAssertReturnVoid(_index >= 0 && _index < m_shaders.size(), "index out of bound");

    Shader& shader = m_shaders[_index];
    vkDestroyShaderModule(m_vkDevice, shader.m_shaderModule, vkMemory);
    shader.m_shaderModule = VK_NULL_HANDLE;
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

        vkDestroyPipelineLayout(m_vkDevice, shaderProgram.m_pipelineLayout, vkMemory);
        vkDestroyDescriptorSetLayout(m_vkDevice, shaderProgram.m_descriptorSetLayout, vkMemory);
    }
    m_shaderPrograms.clear();

    m_parmBufferMatrix->Free();
    eosDelete(m_parmBufferMatrix);
    m_parmBufferMatrix = nullptr;

    m_parmBufferVector->Free();
    eosDelete(m_parmBufferVector);
    m_parmBufferVector = nullptr;

    m_parmBufferFloat->Free();
    eosDelete(m_parmBufferFloat);
    m_parmBufferFloat = nullptr;

    m_parmBufferInteger->Free();
    eosDelete(m_parmBufferInteger);
    m_parmBufferInteger = nullptr;
    
    m_skinningUniformBuffer->Free();
    eosDelete(m_skinningUniformBuffer);
    m_skinningUniformBuffer = nullptr;

    //vkFreeDescriptorSets( m_vkDevice, m_descriptorPools, ION_MAX_DESCRIPTOR_SETS, m_descriptorSets);
    vkResetDescriptorPool(m_vkDevice, m_descriptorPool, 0);
    vkDestroyDescriptorPool(m_vkDevice, m_descriptorPool, vkMemory);

    memset(m_descriptorSets, 0, sizeof(m_descriptorSets));


    m_currentDescSet = 0;
}

///
///
///

const Vector& ShaderProgramManager::GetRenderParamVector(const eosString& _param)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    return GetRenderParamVector(hash);
}

const Vector& ShaderProgramManager::GetRenderParamVector(ionSize _paramHash)
{
    return m_uniformsVector[_paramHash];
}

const Matrix& ShaderProgramManager::GetRenderParamMatrix(const eosString& _param)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    return GetRenderParamMatrix(hash);
}

const Matrix& ShaderProgramManager::GetRenderParamMatrix(ionSize _paramHash)
{
    return m_uniformsMatrix[_paramHash];
}

const ionFloat ShaderProgramManager::GetRenderParamFloat(const eosString& _param)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    return GetRenderParamFloat(hash);
}

const ionFloat ShaderProgramManager::GetRenderParamFloat(ionSize _paramHash)
{
    return m_uniformsFloat[_paramHash];
}

const ionS32 ShaderProgramManager::GetRenderParamInteger(const eosString& _param)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    return GetRenderParamInteger(hash);
}

const ionS32 ShaderProgramManager::GetRenderParamInteger(ionSize _paramHash)
{
    return m_uniformsInteger[_paramHash];
}

///

void ShaderProgramManager::SetRenderParamVector(const eosString& _param, const ionFloat* _value)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    SetRenderParamVector(hash, _value);
}

void ShaderProgramManager::SetRenderParamVector(ionSize _paramHash, const ionFloat* _value)
{
    if (m_uniformsVector.count(_paramHash) > 0)
    {
        Vector v(_value[0], _value[1], _value[2], _value[3]);
        m_uniformsVector[_paramHash] = v;
    }
}

void ShaderProgramManager::SetRenderParamsVector(const eosString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const eosString indexParam(std::to_string(i).c_str());
        const eosString fullParam = _param + indexParam;
        const ionSize hash = std::hash<eosString>{}(fullParam);
        SetRenderParamVector(hash, _values + (i * 4));
    }
}

void ShaderProgramManager::SetRenderParamsVector(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamVector(_paramHash, _values + (i * 4));
    }
}

void ShaderProgramManager::SetRenderParamMatrix(const eosString& _param, const ionFloat* _value)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    SetRenderParamMatrix(hash, _value);
}

void ShaderProgramManager::SetRenderParamMatrix(ionSize _paramHash, const ionFloat* _value)
{
    if (m_uniformsMatrix.count(_paramHash) > 0)
    {
        Matrix m(_value[0], _value[1], _value[2], _value[3], _value[4], _value[5], _value[6], _value[7], _value[8], _value[9], _value[10], _value[11], _value[12], _value[13], _value[14], _value[15]);
        m_uniformsMatrix[_paramHash] = m;
    }
}

void ShaderProgramManager::SetRenderParamsMatrix(const eosString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const eosString indexParam(std::to_string(i).c_str());
        const eosString fullParam = _param + indexParam;
        const ionSize hash = std::hash<eosString>{}(fullParam);
        SetRenderParamMatrix(hash, _values + (i * 16));
    }
}

void ShaderProgramManager::SetRenderParamsMatrix(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamMatrix(_paramHash, _values + (i * 16));
    }
}

void ShaderProgramManager::SetRenderParamFloat(const eosString& _param, const ionFloat _value)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    SetRenderParamFloat(hash, _value);
}

void ShaderProgramManager::SetRenderParamFloat(ionSize _paramHash, const ionFloat _value)
{
    if (m_uniformsFloat.count(_paramHash) > 0)
    {
        m_uniformsFloat[_paramHash] = _value;
    }
}

void ShaderProgramManager::SetRenderParamsFloat(const eosString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const eosString indexParam(std::to_string(i).c_str());
        const eosString fullParam = _param + indexParam;
        const ionSize hash = std::hash<eosString>{}(fullParam);
        SetRenderParamFloat(hash, *(_values + i));
    }
}

void ShaderProgramManager::SetRenderParamsFloat(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamFloat(_paramHash, *(_values + i));
    }
}

void ShaderProgramManager::SetRenderParamInteger(const eosString& _param, const ionS32 _value)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    SetRenderParamInteger(hash, _value);
}

void ShaderProgramManager::SetRenderParamInteger(ionSize _paramHash, const ionS32 _value)
{
    if (m_uniformsInteger.count(_paramHash) > 0)
    {
        m_uniformsInteger[_paramHash] = _value;
    }
}

void ShaderProgramManager::SetRenderParamsInteger(const eosString& _param, const ionS32* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const eosString indexParam(std::to_string(i).c_str());
        const eosString fullParam = _param + indexParam;
        const ionSize hash = std::hash<eosString>{}(fullParam);
        SetRenderParamInteger(hash, *(_values + i));
    }
}

void ShaderProgramManager::SetRenderParamsInteger(ionSize _paramHash, const ionS32* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamInteger(_paramHash, *(_values + i));
    }
}

///
///
///

void ShaderProgramManager::StartFrame()
{
    m_currentDescSet = 0;
    m_currentParmBufferOffset = 0;

    // I'll add back when I'll move to double or triple buffering!
    //vkResetDescriptorPool(m_vkDevice, m_descriptorPool, 0);
}

void ShaderProgramManager::EndFrame()
{
    // Do nothing, just to coherence
}

void ShaderProgramManager::BindProgram(ionS32 _index)
{
    if (m_current != _index) 
    {
        m_current = _index;
    }
}

void ShaderProgramManager::CommitCurrent(const RenderCore& _render, VkRenderPass _renderPass, ionU64 _stateBits, VkCommandBuffer _commandBuffer)
{
    ShaderProgram& shaderProgram = m_shaderPrograms[m_current];

    VkPipeline pipeline = shaderProgram.GetPipeline(_render, _renderPass, _stateBits,
        m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderModule,
        shaderProgram.m_fragmentShaderIndex != -1 ? m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderModule : VK_NULL_HANDLE,
        shaderProgram.m_tessellationControlShaderIndex != -1 ? m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderModule : VK_NULL_HANDLE,
        shaderProgram.m_tessellationEvaluatorShaderIndex != -1 ? m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderModule : VK_NULL_HANDLE, 
        shaderProgram.m_geometryShaderIndex != -1 ? m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderModule : VK_NULL_HANDLE);

    VkDescriptorSetAllocateInfo setAllocInfo = {};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.pNext = nullptr;
    setAllocInfo.descriptorPool = m_descriptorPool;
    setAllocInfo.descriptorSetCount = 1;
    setAllocInfo.pSetLayouts = &shaderProgram.m_descriptorSetLayout;

    VkResult result = vkAllocateDescriptorSets(_render.GetDevice(), &setAllocInfo, &m_descriptorSets[m_currentDescSet]);
    ionAssertReturnVoid(result == VK_SUCCESS, "Cannot allocate the descriptor!");

    VkDescriptorSet descSet = m_descriptorSets[m_currentDescSet];
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

    ionS32 samplerIndex = 0;
    ionS32 uboIndex = 0;
    UniformBuffer* ubos[ION_MAX_DESCRIPTOR_SET_WRITES];
    const Texture* textures[ION_MAX_DESCRIPTOR_SET_WRITES];
    ionU32 destBinding[ION_MAX_DESCRIPTOR_SET_WRITES];
    ionU32 destBindingTexture[ION_MAX_DESCRIPTOR_SET_WRITES];
    memset(&ubos, 0, sizeof(ubos));
    memset(&textures, 0, sizeof(textures));
    memset(&destBinding, 0, sizeof(destBinding)); 
    memset(&destBindingTexture, 0, sizeof(destBindingTexture));

    UniformBuffer vertParms;
    if (shaderProgram.m_vertexShaderIndex > -1)
    {
        ionSize uniformCount = m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderLayout.m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderLayout.m_uniforms[i], vertParms);

            destBinding[uboIndex] = m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderLayout.m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &vertParms;

            ++uboIndex;
        }

        ionSize samplerCount = m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderLayout.m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderLayout.m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = m_shaders[shaderProgram.m_vertexShaderIndex].m_shaderLayout.m_samplers[i].m_texture;

            ++samplerIndex;
        }
    }

    // Need to rework here for skinning
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
    
    if (shaderProgram.m_tessellationControlShaderIndex > -1)
    {
        ionSize uniformCount = m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderLayout.m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderLayout.m_uniforms[i], tessCtrlParms);

            destBinding[uboIndex] = m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderLayout.m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &tessCtrlParms;

            ++uboIndex;
        }

        ionSize samplerCount = m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderLayout.m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderLayout.m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = m_shaders[shaderProgram.m_tessellationControlShaderIndex].m_shaderLayout.m_samplers[i].m_texture;

            ++samplerIndex;
        }
    }

    UniformBuffer tessEvalParms;
    if (shaderProgram.m_tessellationEvaluatorShaderIndex > -1)
    {
        ionSize uniformCount = m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderLayout.m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderLayout.m_uniforms[i], tessEvalParms);

            destBinding[uboIndex] = m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderLayout.m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &tessEvalParms;

            ++uboIndex;
        }

        ionSize samplerCount = m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderLayout.m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderLayout.m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = m_shaders[shaderProgram.m_tessellationEvaluatorShaderIndex].m_shaderLayout.m_samplers[i].m_texture;

            ++samplerIndex;
        }
    }

    UniformBuffer geometryParms;
    if (shaderProgram.m_geometryShaderIndex > -1)
    {
        ionSize uniformCount = m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderLayout.m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderLayout.m_uniforms[i], geometryParms);

            destBinding[uboIndex] = m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderLayout.m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &geometryParms;

            ++uboIndex;
        }

        ionSize samplerCount = m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderLayout.m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderLayout.m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = m_shaders[shaderProgram.m_geometryShaderIndex].m_shaderLayout.m_samplers[i].m_texture;

            ++samplerIndex;
        }
    }

    UniformBuffer fragParms;
    if (shaderProgram.m_fragmentShaderIndex > -1)
    {
        ionSize uniformCount = m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderLayout.m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderLayout.m_uniforms[i], fragParms);

            destBinding[uboIndex] = m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderLayout.m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &fragParms;

            ++uboIndex;
        }

        ionSize samplerCount = m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderLayout.m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderLayout.m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = m_shaders[shaderProgram.m_fragmentShaderIndex].m_shaderLayout.m_samplers[i].m_texture;

            ++samplerIndex;
        }
    }

    ionAssertReturnVoid(uboIndex < ION_MAX_DESCRIPTOR_SET_WRITES, "Uniforms exceed count");
    ionAssertReturnVoid(samplerIndex < ION_MAX_DESCRIPTOR_SET_WRITES, "Samplers exceed count");

    for (ionSize i = 0; i < shaderProgram.m_bindings.size(); ++i)
    {
        EShaderBinding binding = shaderProgram.m_bindings[i];

        switch (binding) 
        {
        case EShaderBinding_Uniform:
        {
            UniformBuffer* ubo = ubos[bufferIndex];

            VkDescriptorBufferInfo & bufferInfo = bufferInfos[bufferIndex];
            memset(&bufferInfo, 0, sizeof(VkDescriptorBufferInfo));
            bufferInfo.buffer = ubo->GetObject();
            bufferInfo.offset = ubo->GetOffset();
            bufferInfo.range = ubo->GetSize();

            VkWriteDescriptorSet & write = writes[writeIndex++];
            memset(&write, 0, sizeof(VkWriteDescriptorSet));
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descSet;
            write.dstBinding = destBinding[bufferIndex++];
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo = &bufferInfo;

            break;
        }
        case EShaderBinding_Sampler:
        {
            const Texture* image = textures[imageIndex];

            VkDescriptorImageInfo & imageInfo = imageInfos[imageIndex];
            memset(&imageInfo, 0, sizeof(VkDescriptorImageInfo));
            imageInfo.imageLayout = image->GetLayout();
            imageInfo.imageView = image->GetView();
            imageInfo.sampler = image->GetSampler();

            ionAssertReturnVoid(image->GetView() != VK_NULL_HANDLE, "View is null!");

            VkWriteDescriptorSet & write = writes[writeIndex++];
            memset(&write, 0, sizeof(VkWriteDescriptorSet));
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descSet;
            write.dstBinding = destBindingTexture[imageIndex++];
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

    if (shaderProgram.m_constantsDef.IsValid())
    {
        vkCmdPushConstants(_commandBuffer, shaderProgram.m_pipelineLayout, shaderProgram.m_constantsDef.m_runtimeStages, 0, static_cast<ionU32>(shaderProgram.m_constantsDef.GetSizeByte()), shaderProgram.m_constantsDef.GetData());
    }
}

void ShaderProgramManager::AllocUniformParametersBlockBuffer(const RenderCore& _render, const UniformBinding& _uniform, UniformBuffer& _ubo)
{
    // gather all informations
    static const ionU32 sMaxParamsAmount = 32;
    ionSize counterForType[EUniformParameterType_Count];
    ionSize indexForType[EUniformParameterType_Count][sMaxParamsAmount];

    memset(&counterForType, 0, sizeof(counterForType));
    memset(&indexForType, 0, sizeof(indexForType));

    const ionSize numParmsType = _uniform.m_type.size();
    for (ionSize i = 0; i < numParmsType; ++i)
    {
        const EUniformParameterType type = _uniform.m_type[i];
        switch (type)
        {
        case EUniformParameterType_Vector:
            indexForType[EUniformParameterType_Vector][counterForType[EUniformParameterType_Vector]] = i;
            ++counterForType[EUniformParameterType_Vector];
            break;
        case EUniformParameterType_Matrix:
            indexForType[EUniformParameterType_Matrix][counterForType[EUniformParameterType_Matrix]] = i;
            ++counterForType[EUniformParameterType_Matrix];
            break;
        case EUniformParameterType_Float:
            indexForType[EUniformParameterType_Float][counterForType[EUniformParameterType_Float]] = i;
            ++counterForType[EUniformParameterType_Float];
            break;
        case EUniformParameterType_Integer:
            indexForType[EUniformParameterType_Integer][counterForType[EUniformParameterType_Integer]] = i;
            ++counterForType[EUniformParameterType_Integer];
            break;
        default:
            break;
        }
    }


    /*
    ORDER OF UNIFORM ELEMENTS
    Matrix
    Vector
    Float
    Integer
    */

    //////////////////////////////////////////////////////////////////////////
    // Matrix 
    ionSize numParms = counterForType[EUniformParameterType_Matrix];

    if (numParms > 0)
    {
        ionSize size = numParms * sizeof(Matrix);
        ionSize mask = _render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1;
        ionSize alignedSize = (size + mask) & ~mask;

        _ubo.ReferenceTo(*m_parmBufferMatrix, m_currentParmBufferOffset, alignedSize);

        Matrix* uniforms = (Matrix*)_ubo.MapBuffer(EBufferMappingType_Write);

        for (ionSize i = 0; i < numParms; ++i)
        {
            uniforms[i] = GetRenderParamMatrix(_uniform.m_runtimeParameters[indexForType[EUniformParameterType_Matrix][i]]);
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

        _ubo.ReferenceTo(*m_parmBufferVector, m_currentParmBufferOffset, alignedSize);

        Vector* uniforms = (Vector*)_ubo.MapBuffer(EBufferMappingType_Write);

        for (ionSize i = 0; i < numParms; ++i)
        {
            uniforms[i] = GetRenderParamVector(_uniform.m_runtimeParameters[indexForType[EUniformParameterType_Vector][i]]);
        }

        _ubo.UnmapBuffer();

        m_currentParmBufferOffset += alignedSize;
    }


    //////////////////////////////////////////////////////////////////////////
    // Float 
    numParms = counterForType[EUniformParameterType_Float];

    if (numParms > 0)
    {
        ionSize size = numParms * sizeof(ionFloat);
        ionSize mask = _render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1;
        ionSize alignedSize = (size + mask) & ~mask;

        _ubo.ReferenceTo(*m_parmBufferFloat, m_currentParmBufferOffset, alignedSize);

        ionFloat* uniforms = (ionFloat*)_ubo.MapBuffer(EBufferMappingType_Write);

        for (ionSize i = 0; i < numParms; ++i)
        {
            uniforms[i] = GetRenderParamFloat(_uniform.m_runtimeParameters[indexForType[EUniformParameterType_Float][i]]);
        }

        _ubo.UnmapBuffer();

        m_currentParmBufferOffset += alignedSize;
    }


    //////////////////////////////////////////////////////////////////////////
    // Integer 
    numParms = counterForType[EUniformParameterType_Integer];

    if (numParms > 0)
    {
        ionSize size = numParms * sizeof(ionS32);
        ionSize mask = _render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1;
        ionSize alignedSize = (size + mask) & ~mask;

        _ubo.ReferenceTo(*m_parmBufferInteger, m_currentParmBufferOffset, alignedSize);

        ionS32* uniforms = (ionS32*)_ubo.MapBuffer(EBufferMappingType_Write);

        for (ionSize i = 0; i < numParms; ++i)
        {
            uniforms[i] = GetRenderParamInteger(_uniform.m_runtimeParameters[indexForType[EUniformParameterType_Integer][i]]);
        }

        _ubo.UnmapBuffer();

        m_currentParmBufferOffset += alignedSize;
    }
}

ionS32 ShaderProgramManager::FindShader(const eosString& _path, const eosString& _name, EShaderStage _stage, const ShaderLayoutDef& _defines)
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
    shader.m_path = _path;
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
    eosString shaderPath = _shader.m_path + _shader.m_name;

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

    _shader.m_shaderLayout = _defines;

    ionSize uniformCount = _shader.m_shaderLayout.m_uniforms.size();
    for (ionSize i = 0; i < uniformCount; ++i)
    {
        ionSize paramCount = _shader.m_shaderLayout.m_uniforms[i].m_parameters.size();
        _shader.m_shaderLayout.m_uniforms[i].m_runtimeParameters.resize(paramCount);
        for (ionSize j = 0; j < paramCount; ++j)
        {
            const ionSize hash = std::hash<eosString>{}(_shader.m_shaderLayout.m_uniforms[i].m_parameters[j]);
            _shader.m_shaderLayout.m_uniforms[i].m_runtimeParameters[j] = hash;
        }
    }

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvBuffer.str().length();
    createInfo.pCode = (ionU32*)spirvBuffer.str().data();

    VkResult result = vkCreateShaderModule(m_vkDevice, &createInfo, vkMemory, &_shader.m_shaderModule);
    ionAssertReturnVoid(result == VK_SUCCESS, "Cannot create shader!");
}

ionS32 ShaderProgramManager::FindProgram(const eosString& _name, EVertexLayout _vertexLayout, const ConstantsBindingDef& _constants, ionS32 _vertexIndex, ionS32 _fragmentIndex /*= -1*/, ionS32 _tessellationControlIndex /*= -1*/, ionS32 _tessellationEvaluationIndex /*= -1*/, ionS32 _geometryIndex /*= -1*/, ionBool _useJoint /*= false*/, ionBool _useSkinning /*= false*/)
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

    ShaderProgramHelper::CreateDescriptorSetLayout(m_vkDevice, program, vertexShader, fragmentShader, tessControlShader, tessEvalShader, geometryShader, _constants);

    program.m_constantsDef = _constants;    // add any

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
        vkDestroyDescriptorSetLayout(m_vkDevice, shaderProgram.m_descriptorSetLayout, vkMemory);
    }
    m_shaderPrograms.clear();
}


ION_NAMESPACE_END