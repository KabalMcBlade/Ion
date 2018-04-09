#include "ShaderProgramManager.h"


#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"

#include "ShaderProgramHelper.h"


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

ionBool ShaderProgramManager::Init(const eosString& _shaderFolderPath)
{
    m_shaderFolderPath = _shaderFolderPath;
    return true;
}

const Vector& ShaderProgramManager::GetRenderParm(const eosString& _param)
{
    const ionSize hash = std::hash<eosString>{}(_param);   // from the original with extension
    return GetRenderParm(hash);
}

// if parameter not found, return a vector 0 and create this new hash! BE CAREFUL!
const Vector& ShaderProgramManager::GetRenderParm(ionSize _paramHash)
{
    return m_uniforms[_paramHash];
}

void ShaderProgramManager::SetRenderParm(const eosString& _param, const ionFloat* _value)
{

}

void ShaderProgramManager::SetRenderParm(ionSize _paramHash, const ionFloat* _value)
{

}

void ShaderProgramManager::SetRenderParms(const eosString& _param, const ionFloat* _values, ionS32 _numValues)
{

}

void ShaderProgramManager::SetRenderParms(ionSize _paramHash, const ionFloat* _values, ionS32 _numValues)
{

}

void ShaderProgramManager::Shutdown()
{
}

void ShaderProgramManager::StartFrame()
{
}

void ShaderProgramManager::EndFrame()
{
}

ionS32 ShaderProgramManager::FindShader(const eosString& _name, EShaderStage _stage)
{
    for (ionS32 i = 0; i < m_shaders.size(); ++i)
    {
        Shader& shader = m_shaders[i];
        if (shader.m_name == _name && shader.m_stage == _stage)
        {
            LoadShader(i);
            return i;
        }
    }

    Shader shader;
    shader.m_name = _name;
    shader.m_stage = _stage;
    m_shaders.push_back(shader);
    ionS32 index = (ionS32)(m_shaders.size() - 1);
    LoadShader(index);
    return index;
}

void ShaderProgramManager::LoadShader(ionS32 _index)
{
    if (m_shaders[_index].m_shaderModule != VK_NULL_HANDLE)
    {
        return; // Already loaded
    }

    LoadShader(m_shaders[_index]);
}

void ShaderProgramManager::LoadShader(Shader& _shader)
{

}


ION_NAMESPACE_END