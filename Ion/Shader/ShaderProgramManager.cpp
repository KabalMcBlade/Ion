#include "ShaderProgramManager.h"

#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "../Core/FileSystemManager.h"

#include "ShaderProgramHelper.h"
#include "../Renderer/RenderCore.h"

#include "../Renderer/VertexCacheManager.h"
#include "../Texture/Texture.h"

#include "../Material/Material.h"


EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ShaderProgramManagerAllocator* ShaderProgramManager::GetAllocator()
{
	static HeapArea<Settings::kShaderProgamHelperAllocatorSize> memoryArea;
	static ShaderProgramManagerAllocator memoryAllocator(memoryArea, "ShaderProgramManagerListAllocator");

	return &memoryAllocator;
}


ShaderProgramManager::ShaderProgramManager() :
    m_current(0),
    m_currentDescSet(0),
    m_currentParmBufferOffset(0)
{
}

ShaderProgramManager::~ShaderProgramManager()
{

}

ShaderProgramManager& ShaderProgramManager::Instance()
{
    static ShaderProgramManager instance;
    return instance;
}

ionBool ShaderProgramManager::Init(VkDevice _vkDevice)
{
    m_vkDevice = _vkDevice;

    ShaderProgramHelper::CreateVertexDescriptor();

    ShaderProgramHelper::CreateDescriptorPools(m_vkDevice, m_descriptorPool);

    m_uniformBuffer = ionNew(UniformBuffer, GetAllocator());
    m_uniformBuffer->Alloc(m_vkDevice, nullptr, ION_MAX_DESCRIPTOR_SETS * ION_MAX_DESCRIPTOR_SET_UNIFORMS, EBufferUsage_Dynamic);

    m_skinningUniformBuffer = ionNew(UniformBuffer, GetAllocator());
    m_skinningUniformBuffer->Alloc(m_vkDevice, nullptr, sizeof(Vector4), EBufferUsage_Dynamic);

    return true;
}

void ShaderProgramManager::UnloadShader(ionSize _index)
{
    ionAssertReturnVoid(_index >= 0 && _index < m_shaders.size(), "index out of bound");

    Shader* shader = m_shaders[_index];
    vkDestroyShaderModule(m_vkDevice, shader->m_shaderModule, vkMemory);
    shader->m_shaderModule = VK_NULL_HANDLE;

	ionDelete(shader, GetAllocator());
}

void ShaderProgramManager::Shutdown()
{
    for (ionSize i = 0; i < m_shaders.size(); ++i) 
    {
		UnloadShader(i);
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

    m_uniformBuffer->Free();
    ionDelete(m_uniformBuffer, GetAllocator());
    m_uniformBuffer = nullptr;

    m_skinningUniformBuffer->Free();
    ionDelete(m_skinningUniformBuffer, GetAllocator());
    m_skinningUniformBuffer = nullptr;

 
    vkResetDescriptorPool(m_vkDevice, m_descriptorPool, 0);
    vkDestroyDescriptorPool(m_vkDevice, m_descriptorPool, vkMemory);

    memset(m_descriptorSets, 0, sizeof(m_descriptorSets));


    m_currentDescSet = 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

const Matrix& ShaderProgramManager::GetRenderParamMatrix(const ionString& _param)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    return GetRenderParamMatrix(hash);
}

const Matrix& ShaderProgramManager::GetRenderParamMatrix(ionSize _paramHash)
{
    return m_uniformsMatrix[_paramHash];
}

const Vector4& ShaderProgramManager::GetRenderParamVector(const ionString& _param)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    return GetRenderParamVector(hash);
}

const Vector4& ShaderProgramManager::GetRenderParamVector(ionSize _paramHash)
{
    return m_uniformsVector[_paramHash];
}

const ionFloat ShaderProgramManager::GetRenderParamFloat(const ionString& _param)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    return GetRenderParamFloat(hash);
}

const ionFloat ShaderProgramManager::GetRenderParamFloat(ionSize _paramHash)
{
    return m_uniformsFloat[_paramHash];
}

const ionS32 ShaderProgramManager::GetRenderParamInteger(const ionString& _param)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    return GetRenderParamInteger(hash);
}

const ionS32 ShaderProgramManager::GetRenderParamInteger(ionSize _paramHash)
{
    return m_uniformsInteger[_paramHash];
}

//////////////////////////////////////////////////////////////////////////

void ShaderProgramManager::SetRenderParamMatrix(const ionString& _param, const Matrix& _value)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    SetRenderParamMatrix(hash, _value);
}

void ShaderProgramManager::SetRenderParamMatrix(ionSize _paramHash, const Matrix& _value)
{
    m_uniformsMatrix[_paramHash] = _value;
}

void ShaderProgramManager::SetRenderParamMatrix(const ionString& _param, const ionFloat* _value)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    SetRenderParamMatrix(hash, _value);
}

void ShaderProgramManager::SetRenderParamMatrix(ionSize _paramHash, const ionFloat* _value)
{
    Matrix m(_value[0], _value[1], _value[2], _value[3], _value[4], _value[5], _value[6], _value[7], _value[8], _value[9], _value[10], _value[11], _value[12], _value[13], _value[14], _value[15]);
    m_uniformsMatrix[_paramHash] = m;
}

void ShaderProgramManager::SetRenderParamsMatrix(const ionString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const ionString indexParam(std::to_string(i).c_str());
        const ionString fullParam = _param + indexParam;
        const ionSize hash = std::hash<ionString>{}(fullParam);
        SetRenderParamMatrix(hash, _values + (i * 16));
    }
}

void ShaderProgramManager::SetRenderParamsMatrix(const ionString& _param, const ionVector<Matrix, ShaderProgramManagerAllocator, GetAllocator>& _values)
{
    typedef ionVector<Matrix, ShaderProgramManagerAllocator, GetAllocator>::size_type count_type;
    const count_type count = _values.size();
    for (count_type i = 0; i < count; ++i)
    {
        const ionString indexParam(std::to_string(i).c_str());
        const ionString fullParam = _param + indexParam;
        const ionSize hash = std::hash<ionString>{}(fullParam);
        SetRenderParamMatrix(hash, _values[i]);
    }
}

/*
void ShaderProgramManager::SetRenderParamsMatrix(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamMatrix(_paramHash, _values + (i * 16));
    }
}
*/

void ShaderProgramManager::SetRenderParamVector(const ionString& _param, const Vector4& _value)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    SetRenderParamVector(hash, _value);
}

void ShaderProgramManager::SetRenderParamVector(ionSize _paramHash, const Vector4& _value)
{
    m_uniformsVector[_paramHash] = _value;
}

void ShaderProgramManager::SetRenderParamVector(const ionString& _param, const ionFloat* _value)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    SetRenderParamVector(hash, _value);
}

void ShaderProgramManager::SetRenderParamVector(ionSize _paramHash, const ionFloat* _value)
{
    Vector4 v(_value[0], _value[1], _value[2], _value[3]);
    m_uniformsVector[_paramHash] = v;
}

void ShaderProgramManager::SetRenderParamsVector(const ionString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const ionString indexParam(std::to_string(i).c_str());
        const ionString fullParam = _param + indexParam;
        const ionSize hash = std::hash<ionString>{}(fullParam);
        SetRenderParamVector(hash, _values + (i * 4));
    }
}

void ShaderProgramManager::SetRenderParamsVector(const ionString& _param, const ionVector<Vector4, ShaderProgramManagerAllocator, GetAllocator>& _values)
{
    typedef ionVector<Vector4, ShaderProgramManagerAllocator, GetAllocator>::size_type count_type;
    const count_type count = _values.size();
    for (ionU32 i = 0; i < count; ++i)
    {
        const ionString indexParam(std::to_string(i).c_str());
        const ionString fullParam = _param + indexParam;
        const ionSize hash = std::hash<ionString>{}(fullParam);
        SetRenderParamVector(hash, _values[i]);
    }
}

/*
void ShaderProgramManager::SetRenderParamsVector(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamVector(_paramHash, _values + (i * 4));
    }
}
*/
void ShaderProgramManager::SetRenderParamFloat(const ionString& _param, const ionFloat _value)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    SetRenderParamFloat(hash, _value);
}

void ShaderProgramManager::SetRenderParamFloat(ionSize _paramHash, const ionFloat _value)
{
    m_uniformsFloat[_paramHash] = _value;
}

void ShaderProgramManager::SetRenderParamsFloat(const ionString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const ionString indexParam(std::to_string(i).c_str());
        const ionString fullParam = _param + indexParam;
        const ionSize hash = std::hash<ionString>{}(fullParam);
        SetRenderParamFloat(hash, *(_values + i));
    }
}
/*
void ShaderProgramManager::SetRenderParamsFloat(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamFloat(_paramHash, *(_values + i));
    }
}
*/
void ShaderProgramManager::SetRenderParamInteger(const ionString& _param, const ionS32 _value)
{
    const ionSize hash = std::hash<ionString>{}(_param);
    SetRenderParamInteger(hash, _value);
}

void ShaderProgramManager::SetRenderParamInteger(ionSize _paramHash, const ionS32 _value)
{
    m_uniformsInteger[_paramHash] = _value;
}

void ShaderProgramManager::SetRenderParamsInteger(const ionString& _param, const ionS32* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const ionString indexParam(std::to_string(i).c_str());
        const ionString fullParam = _param + indexParam;
        const ionSize hash = std::hash<ionString>{}(fullParam);
        SetRenderParamInteger(hash, *(_values + i));
    }
}
/*
void ShaderProgramManager::SetRenderParamsInteger(ionSize _paramHash, const ionS32* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParamInteger(_paramHash, *(_values + i));
    }
}
*/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void ShaderProgramManager::StartFrame()
{
    m_currentDescSet = 0;
    m_currentParmBufferOffset = 0;

    vkResetDescriptorPool(m_vkDevice, m_descriptorPool, 0);
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

void ShaderProgramManager::CommitCurrent(const RenderCore& _render, const Material* _material, VkRenderPass _renderPass, ionU64 _stateBits, VkCommandBuffer _commandBuffer)
{
    ShaderProgram& shaderProgram = m_shaderPrograms[m_current];

    ionS32  vertexShaderIndex = -1;
    ionS32  fragmentShaderIndex = -1;
    ionS32  tessellationControlIndex = -1;
    ionS32  tessellationEvaluationIndex = -1;
    ionS32  geometryIndex = -1;
    ionBool useJoint = false;
    ionBool useSkinning = false;

    _material->GetShaders(vertexShaderIndex, fragmentShaderIndex, tessellationControlIndex, tessellationEvaluationIndex, geometryIndex, useJoint, useSkinning);

    VkPipeline pipeline = shaderProgram.GetPipeline(_render, _renderPass, _stateBits, _material->GetTopology(),
        vertexShaderIndex != -1 ? m_shaders[vertexShaderIndex]->m_shaderModule : VK_NULL_HANDLE,
        fragmentShaderIndex != -1 ? m_shaders[fragmentShaderIndex]->m_shaderModule : VK_NULL_HANDLE,
        tessellationControlIndex != -1 ? m_shaders[tessellationControlIndex]->m_shaderModule : VK_NULL_HANDLE,
        tessellationEvaluationIndex != -1 ? m_shaders[tessellationEvaluationIndex]->m_shaderModule : VK_NULL_HANDLE,
        geometryIndex != -1 ? m_shaders[geometryIndex]->m_shaderModule : VK_NULL_HANDLE,
        vertexShaderIndex != -1 ? m_shaders[vertexShaderIndex]->GetSpecializationConstants() : nullptr,
        fragmentShaderIndex != -1 ? m_shaders[fragmentShaderIndex]->GetSpecializationConstants() : nullptr,
        tessellationControlIndex != -1 ? m_shaders[tessellationControlIndex]->GetSpecializationConstants() : nullptr,
        tessellationEvaluationIndex != -1 ? m_shaders[tessellationEvaluationIndex]->GetSpecializationConstants() : nullptr,
        geometryIndex != -1 ? m_shaders[geometryIndex]->GetSpecializationConstants() : nullptr);

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
    ionS32 storageIndex = 0;
    ionS32 imageIndex = 0;
    ionS32 bindingIndex = 0;

    VkWriteDescriptorSet writes[ION_MAX_DESCRIPTOR_SET_WRITES];
    VkDescriptorBufferInfo bufferInfos[ION_MAX_DESCRIPTOR_SET_WRITES];
    VkDescriptorImageInfo imageInfos[ION_MAX_DESCRIPTOR_SET_WRITES];
    VkDescriptorBufferInfo storageInfos[ION_MAX_DESCRIPTOR_SET_WRITES];

    memset(&writes, 0, sizeof(writes));
    memset(&bufferInfos, 0, sizeof(bufferInfos));
    memset(&imageInfos, 0, sizeof(imageInfos));
    memset(&storageInfos, 0, sizeof(storageInfos));

    ionS32 samplerIndex = 0;
    ionS32 uboIndex = 0;
    ionS32 sboIndex = 0;
    UniformBuffer* ubos[ION_MAX_DESCRIPTOR_SET_WRITES];
    const Texture* textures[ION_MAX_DESCRIPTOR_SET_WRITES];
    VertexCacheHandler storage[ION_MAX_DESCRIPTOR_SET_WRITES];
    ionU32 destBinding[ION_MAX_DESCRIPTOR_SET_WRITES];
    ionU32 destBindingTexture[ION_MAX_DESCRIPTOR_SET_WRITES];
    ionU32 destBindingStorage[ION_MAX_DESCRIPTOR_SET_WRITES];
    memset(&ubos, 0, sizeof(ubos));
    memset(&textures, 0, sizeof(textures));
    memset(&storage, 0, sizeof(storage));
    memset(&destBinding, 0, sizeof(destBinding)); 
    memset(&destBindingTexture, 0, sizeof(destBindingTexture));

    UniformBuffer vertParms;
    if (vertexShaderIndex > -1)
    {
        ionSize uniformCount = _material->GetVertexShaderLayout().m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, _material->GetVertexShaderLayout().m_uniforms[i], vertParms);

            destBinding[uboIndex] = _material->GetVertexShaderLayout().m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &vertParms;

            ++uboIndex;
        }

        ionSize samplerCount = _material->GetVertexShaderLayout().m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = _material->GetVertexShaderLayout().m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = _material->GetVertexShaderLayout().m_samplers[i].m_texture;

            ++samplerIndex;
        }

        ionSize storageCount = _material->GetVertexShaderLayout().m_storages.size();
        for (ionSize i = 0; i < storageCount; ++i)
        {
            destBindingStorage[sboIndex] = _material->GetVertexShaderLayout().m_storages[i].m_bindingIndex;
            storage[sboIndex] = _material->GetVertexShaderLayout().m_storages[i].m_cache;

            ++sboIndex;
        }
    }

    // Need to rework here for skinning
    UniformBuffer jointBuffer;
    if (useJoint && _render.GetJointCacheHandler() > 0)
    {
        if (!ionVertexCacheManager().GetJointBuffer(_render.GetJointCacheHandler(), &jointBuffer))
        {
            ionAssertReturnVoid(false, "CommitCurrent: The jointBuffer is nullptr");
            return;
        }
        ionAssertReturnVoid((jointBuffer.GetOffset() & (_render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1)) == 0, "Error in the joint buffer");

        ubos[uboIndex++] = &jointBuffer;
    }
    else if (useSkinning)
    {
        ubos[uboIndex++] = m_skinningUniformBuffer;
    }

    UniformBuffer tessCtrlParms;
    if (tessellationControlIndex > -1)
    {
        ionSize uniformCount = _material->GetTessellationControlShaderLayout().m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, _material->GetTessellationControlShaderLayout().m_uniforms[i], tessCtrlParms);

            destBinding[uboIndex] = _material->GetTessellationControlShaderLayout().m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &tessCtrlParms;

            ++uboIndex;
        }

        ionSize samplerCount = _material->GetTessellationControlShaderLayout().m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = _material->GetTessellationControlShaderLayout().m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = _material->GetTessellationControlShaderLayout().m_samplers[i].m_texture;

            ++samplerIndex;
        }

        ionSize storageCount = _material->GetTessellationControlShaderLayout().m_storages.size();
        for (ionSize i = 0; i < storageCount; ++i)
        {
            destBindingStorage[sboIndex] = _material->GetTessellationControlShaderLayout().m_storages[i].m_bindingIndex;
            storage[sboIndex] = _material->GetTessellationControlShaderLayout().m_storages[i].m_cache;

            ++sboIndex;
        }
    }

    UniformBuffer tessEvalParms;
    if (tessellationEvaluationIndex > -1)
    {
        ionSize uniformCount = _material->GetTessellationEvaluatorShaderLayout().m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, _material->GetTessellationEvaluatorShaderLayout().m_uniforms[i], tessEvalParms);

            destBinding[uboIndex] = _material->GetTessellationEvaluatorShaderLayout().m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &tessEvalParms;

            ++uboIndex;
        }

        ionSize samplerCount = _material->GetTessellationEvaluatorShaderLayout().m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = _material->GetTessellationEvaluatorShaderLayout().m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = _material->GetTessellationEvaluatorShaderLayout().m_samplers[i].m_texture;

            ++samplerIndex;
        }

        ionSize storageCount = _material->GetTessellationEvaluatorShaderLayout().m_storages.size();
        for (ionSize i = 0; i < storageCount; ++i)
        {
            destBindingStorage[sboIndex] = _material->GetTessellationEvaluatorShaderLayout().m_storages[i].m_bindingIndex;
            storage[sboIndex] = _material->GetTessellationEvaluatorShaderLayout().m_storages[i].m_cache;

            ++sboIndex;
        }
    }

    UniformBuffer geometryParms;
    if (geometryIndex > -1)
    {
        ionSize uniformCount = _material->GetGeometryShaderLayout().m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, _material->GetGeometryShaderLayout().m_uniforms[i], geometryParms);

            destBinding[uboIndex] = _material->GetGeometryShaderLayout().m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &geometryParms;

            ++uboIndex;
        }

        ionSize samplerCount = _material->GetGeometryShaderLayout().m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = _material->GetGeometryShaderLayout().m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = _material->GetGeometryShaderLayout().m_samplers[i].m_texture;

            ++samplerIndex;
        }

        ionSize storageCount = _material->GetGeometryShaderLayout().m_storages.size();
        for (ionSize i = 0; i < storageCount; ++i)
        {
            destBindingStorage[sboIndex] = _material->GetGeometryShaderLayout().m_storages[i].m_bindingIndex;
            storage[sboIndex] = _material->GetGeometryShaderLayout().m_storages[i].m_cache;

            ++sboIndex;
        }
    }

    UniformBuffer fragParms;
    if (fragmentShaderIndex > -1)
    {
        ionSize uniformCount = _material->GetFragmentShaderLayout().m_uniforms.size();
        for (ionSize i = 0; i < uniformCount; ++i)
        {
            AllocUniformParametersBlockBuffer(_render, _material->GetFragmentShaderLayout().m_uniforms[i], fragParms);

            destBinding[uboIndex] = _material->GetFragmentShaderLayout().m_uniforms[i].m_bindingIndex;
            ubos[uboIndex] = &fragParms;

            ++uboIndex;
        }

        ionSize samplerCount = _material->GetFragmentShaderLayout().m_samplers.size();
        for (ionSize i = 0; i < samplerCount; ++i)
        {
            destBindingTexture[samplerIndex] = _material->GetFragmentShaderLayout().m_samplers[i].m_bindingIndex;
            textures[samplerIndex] = _material->GetFragmentShaderLayout().m_samplers[i].m_texture;

            ++samplerIndex;
        }

        ionSize storageCount = _material->GetFragmentShaderLayout().m_storages.size();
        for (ionSize i = 0; i < storageCount; ++i)
        {
            destBindingStorage[sboIndex] = _material->GetFragmentShaderLayout().m_storages[i].m_bindingIndex;
            storage[sboIndex] = _material->GetFragmentShaderLayout().m_storages[i].m_cache;

            ++sboIndex;
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
        case EShaderBinding_Storage:
        {
            VkDescriptorBufferInfo & storageInfo = storageInfos[storageIndex];
            memset(&storageInfo, 0, sizeof(VkDescriptorBufferInfo));

            StorageBuffer storageBuffer;
            if (ionVertexCacheManager().GetStorageBuffer(storage[storageIndex], &storageBuffer))
            {
                storageInfo.buffer = storageBuffer.GetObject();
                storageInfo.offset = storageBuffer.GetOffset();
                storageInfo.range = storageBuffer.GetSize();

                VkWriteDescriptorSet & write = writes[writeIndex++];
                memset(&write, 0, sizeof(VkWriteDescriptorSet));
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = descSet;
                write.dstBinding = destBindingStorage[storageIndex++];
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                write.pBufferInfo = &storageInfo;
            }
            break;
        }
        }
    }

    vkUpdateDescriptorSets(m_vkDevice, writeIndex, writes, 0, nullptr);

    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderProgram.m_pipelineLayout, 0, 1, &descSet, 0, nullptr);
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    const ConstantsBindingDef& constantsDef = _material->GetConstantsShaders();

    if (constantsDef.IsValid())
    {
        vkCmdPushConstants(_commandBuffer, shaderProgram.m_pipelineLayout, constantsDef.m_runtimeStages, 0, static_cast<ionU32>(constantsDef.GetSizeByte()), constantsDef.GetData());
    }
}

void ShaderProgramManager::AllocUniformParametersBlockBuffer(const RenderCore& _render, const UniformBinding& _uniform, UniformBuffer& _ubo)
{
    // gather all informations
    static const ionU32 sMaxParamsAmount = 32;
    ionSize counterForType[EBufferParameterType_Count];
    ionSize indexForType[EBufferParameterType_Count][sMaxParamsAmount];

    memset(&counterForType, 0, sizeof(counterForType));
    memset(&indexForType, 0, sizeof(indexForType));

    const ionSize numParmsType = _uniform.m_type.size();
    for (ionSize i = 0; i < numParmsType; ++i)
    {
        const EBufferParameterType type = _uniform.m_type[i];
        switch (type)
        {
        case EBufferParameterType_Vector:
            indexForType[EBufferParameterType_Vector][counterForType[EBufferParameterType_Vector]] = i;
            ++counterForType[EBufferParameterType_Vector];
            break;
        case EBufferParameterType_Matrix:
            indexForType[EBufferParameterType_Matrix][counterForType[EBufferParameterType_Matrix]] = i;
            ++counterForType[EBufferParameterType_Matrix];
            break;
        case EBufferParameterType_Float:
            indexForType[EBufferParameterType_Float][counterForType[EBufferParameterType_Float]] = i;
            ++counterForType[EBufferParameterType_Float];
            break;
        case EBufferParameterType_Integer:
            indexForType[EBufferParameterType_Integer][counterForType[EBufferParameterType_Integer]] = i;
            ++counterForType[EBufferParameterType_Integer];
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

    const ionSize numParmsMatrix = counterForType[EBufferParameterType_Matrix];
    const ionSize sizeMatrix = numParmsMatrix * sizeof(Matrix);

    const ionSize numParmsVector = counterForType[EBufferParameterType_Vector];
    const ionSize sizeVector = numParmsVector * sizeof(Vector4);

    const ionSize numParmsFloat = counterForType[EBufferParameterType_Float];
    const ionSize sizeFloat = numParmsFloat * sizeof(ionFloat);

    const ionSize numParmsInt = counterForType[EBufferParameterType_Integer];
    const ionSize sizeInt = numParmsInt * sizeof(ionS32);


    const ionSize size = sizeMatrix + sizeVector + sizeFloat + sizeInt;
    const ionSize mask = _render.GetGPU().m_vkPhysicalDeviceProps.limits.minUniformBufferOffsetAlignment - 1;
    const ionSize alignedSize = (size + mask) & ~mask;

    //
    _ubo.ReferenceTo(*m_uniformBuffer, m_currentParmBufferOffset, alignedSize);


    Matrix* uniformsMatrix = (Matrix*)_ubo.MapBuffer(EBufferMappingType_Write);
    for (ionSize i = 0; i < numParmsMatrix; ++i)
    {
        uniformsMatrix[i] = GetRenderParamMatrix(_uniform.m_runtimeParameters[indexForType[EBufferParameterType_Matrix][i]]);
    }

    Vector4* uniformsVector = (Vector4*)_ubo.MapBuffer(EBufferMappingType_Write, (sizeof(Matrix) * numParmsMatrix));
    for (ionSize i = 0; i < numParmsVector; ++i)
    {
        uniformsVector[i] = GetRenderParamVector(_uniform.m_runtimeParameters[indexForType[EBufferParameterType_Vector][i]]);
    }

    ionFloat* uniformsFloat = (ionFloat*)_ubo.MapBuffer(EBufferMappingType_Write, (sizeof(Matrix) * numParmsMatrix) + (sizeof(Vector4) * numParmsVector));
    for (ionSize i = 0; i < numParmsFloat; ++i)
    {
        uniformsFloat[i] = GetRenderParamFloat(_uniform.m_runtimeParameters[indexForType[EBufferParameterType_Float][i]]);
    }

    ionS32* uniformsInt = (ionS32*)_ubo.MapBuffer(EBufferMappingType_Write, (sizeof(Matrix) * numParmsMatrix) + (sizeof(Vector4) * numParmsVector) + (sizeof(ionFloat) * numParmsFloat));
    for (ionSize i = 0; i < numParmsInt; ++i)
    {
        uniformsInt[i] = GetRenderParamInteger(_uniform.m_runtimeParameters[indexForType[EBufferParameterType_Integer][i]]);
    }

    _ubo.UnmapBuffer();

    m_currentParmBufferOffset += alignedSize;
}

ionS32 ShaderProgramManager::FindShader(const ionString& _path, const ionString& _name, EShaderStage _stage)
{
    ionVector<ionFloat, ShaderProgramManagerAllocator, GetAllocator> emptySpecializationConstants;
    return FindShader(_path, _name, _stage, emptySpecializationConstants);
}


ionS32 ShaderProgramManager::FindShader(const ionString& _path, const ionString& _name, EShaderStage _stage, const ionVector<ionFloat, ShaderProgramManagerAllocator, GetAllocator>& _specializationConstantValues)
{
    for (ionS32 i = 0; i < m_shaders.size(); ++i)
    {
        Shader* shader = m_shaders[i];
        if (shader->m_name == _name && shader->m_stage == _stage)
        {
            LoadShader(i);
            return i;
        }
    }

    Shader* shader = ionNew(Shader, GetAllocator());
    shader->m_path = _path;
    shader->m_name = _name;
    shader->m_stage = _stage;

    // specialization constants
    if (_specializationConstantValues.size() > 0)
    {
        for (ionVector<ionFloat, ShaderProgramManagerAllocator, GetAllocator>::size_type i = 0; i != _specializationConstantValues.size(); i++)
        {
            shader->m_specializationConstants.m_values.push_back(_specializationConstantValues[i]);
        }

        shader->m_specializationConstants.Generate();
    }

    m_shaders.push_back(shader);
    ionS32 index = (ionS32)(m_shaders.size() - 1);
    LoadShader(index);
    return index;
}

void ShaderProgramManager::LoadShader(ionS32 _index)
{
    if (m_shaders[_index]->m_shaderModule != VK_NULL_HANDLE)
    {
        return; // Already loaded
    }

    LoadShader(m_shaders[_index]);
}

void ShaderProgramManager::LoadShader(Shader* _shader)
{
	ionString shaderPath = _shader->m_path + _shader->m_name;

    // just direct set, not binary
    switch(_shader->m_stage)
    {
    case EShaderStage_Vertex:           shaderPath += ".vert.spv"; break;
    case EShaderStage_Tessellation_Ctrl:shaderPath += ".ctrl.spv"; break;
    case EShaderStage_Tessellation_Eval:shaderPath += ".eval.spv"; break;
    case EShaderStage_Geometry:         shaderPath += ".geom.spv"; break;
    case EShaderStage_Fragment:         shaderPath += ".frag.spv"; break;
    case EShaderStage_Compute:          shaderPath += ".comp.spv"; break;
    default:
        ionAssertReturnVoid(false, "Shader stage mismatch!");
        break;
    }

	std::ifstream fileStream(shaderPath.c_str(), std::ios::binary);

	fileStream.seekg(0, std::ios_base::end);
	const ionSize fileSize = fileStream.tellg();

	if (fileSize > 0)
	{
		ionAssertDialog(fileSize % sizeof(std::uint32_t) == 0);

		char* binary = reinterpret_cast<char*>(ionNewRaw(fileSize, GetAllocator()));

		fileStream.seekg(0, std::ios_base::beg);
		fileStream.read(binary, fileSize);

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = fileSize;
		createInfo.pCode = reinterpret_cast<const ionU32*>(binary);

		VkResult result = vkCreateShaderModule(m_vkDevice, &createInfo, vkMemory, &_shader->m_shaderModule);

		ionDeleteRaw(binary, GetAllocator());

		ionAssertReturnVoid(result == VK_SUCCESS, "Cannot create shader!");
	}
	else
	{
		ionAssertReturnVoid(false, "Shader file size is 0!");
	}
}

ionS32 ShaderProgramManager::FindProgram(const Material* _material)
{
    for (ionSize i = 0; i < m_shaderPrograms.size(); ++i)
    {
        ShaderProgram& prog = m_shaderPrograms[i];
        if ((prog.m_material == _material) /*&&         // NOT SURE THIS STILL NEED!
            (prog.m_material->GetVertexShaderLayout() == _material->GetVertexShaderLayout()) && 
            (prog.m_material->GetFragmentShaderLayout() == _material->GetFragmentShaderLayout()) &&
            (prog.m_material->GetTessellationControlShaderLayout() == _material->GetTessellationControlShaderLayout()) &&
            (prog.m_material->GetTessellationEvaluatorShaderLayout() == _material->GetTessellationEvaluatorShaderLayout()) &&
            (prog.m_material->GetGeometryShaderLayout() == _material->GetGeometryShaderLayout()) &&
            (prog.m_material->GetComputeShaderLayout() == _material->GetComputeShaderLayout())*/
            )
        {
            return (ionS32)i;
        }
    }

    ShaderProgram program;
    program.m_vertextLayoutType = _material->GetVertexLayout();
    program.m_material = _material;

    // If index is -1 for some shader, I have to manually pass an "invalid" shader to the next function!
    ionS32  vertexShaderIndex = -1;
    ionS32  fragmentShaderIndex = -1;
    ionS32  tessellationControlIndex = -1;
    ionS32  tessellationEvaluationIndex = -1;
    ionS32  geometryIndex = -1;
    ionBool useJoint = false;
    ionBool useSkinning = false;

    _material->GetShaders(vertexShaderIndex, fragmentShaderIndex, tessellationControlIndex, tessellationEvaluationIndex, geometryIndex, useJoint, useSkinning);

    const ionSize shaderCount = m_shaders.size();
    const Shader* vertexShader = vertexShaderIndex > -1 && vertexShaderIndex < shaderCount ? m_shaders[vertexShaderIndex] : nullptr;
    const Shader* fragmentShader = fragmentShaderIndex > -1 && fragmentShaderIndex < shaderCount ? m_shaders[fragmentShaderIndex] : nullptr;
    const Shader* tessControlShader = tessellationControlIndex > -1 && tessellationControlIndex < shaderCount ? m_shaders[tessellationControlIndex] : nullptr;
    const Shader* tessEvalShader = tessellationEvaluationIndex > -1 && tessellationEvaluationIndex < shaderCount ? m_shaders[tessellationEvaluationIndex] : nullptr;
    const Shader* geometryShader = geometryIndex > -1 && geometryIndex < shaderCount ? m_shaders[geometryIndex] : nullptr;

    ShaderProgramHelper::CreateDescriptorSetLayout(m_vkDevice, program, vertexShader, fragmentShader, tessControlShader, tessEvalShader, geometryShader, _material);

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

    vkResetDescriptorPool(m_vkDevice, m_descriptorPool, 0);
}


ION_NAMESPACE_END