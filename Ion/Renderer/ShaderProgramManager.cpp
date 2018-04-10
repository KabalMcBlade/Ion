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

ionBool ShaderProgramManager::Init(VkDevice _vkDevice, const eosString& _shaderFolderPath)
{
    m_vkDevice = _vkDevice;
    m_shaderFolderPath = _shaderFolderPath;
    return true;
}

const Vector& ShaderProgramManager::GetRenderParm(const eosString& _param)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    return GetRenderParm(hash);
}

// if parameter not found, return a vector 0 and create this new hash! BE CAREFUL!
const Vector& ShaderProgramManager::GetRenderParm(ionSize _paramHash)
{
    return m_uniforms[_paramHash];
}

void ShaderProgramManager::SetRenderParm(const eosString& _param, const ionFloat* _value)
{
    const ionSize hash = std::hash<eosString>{}(_param);
    SetRenderParm(hash, _value);
}

void ShaderProgramManager::SetRenderParm(ionSize _paramHash, const ionFloat* _value)
{
    Vector v(_value[0], _value[1], _value[2], _value[3]);
    m_uniforms[_paramHash] = v;
}

void ShaderProgramManager::SetRenderParms(const eosString& _param, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        const eosString indexParam(std::to_string(i).c_str());
        const eosString fullParam = _param + indexParam;
        const ionSize hash = std::hash<eosString>{}(fullParam);
        SetRenderParm(hash, _values + (i * 4));
    }
}

void ShaderProgramManager::SetRenderParms(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues)
{
    for (ionU32 i = 0; i < _numValues; ++i)
    {
        SetRenderParm(_paramHash, _values + (i * 4));
    }
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
    case EShaderStage_Vertex:           shaderPath += ".vspv"; break;
    case EShaderStage_Tessellation_Ctrl:shaderPath += ".cspv"; break;
    case EShaderStage_Tessellation_Eval:shaderPath += ".espv"; break;
    case EShaderStage_Geometry:         shaderPath += ".gspv"; break;
    case EShaderStage_Fragment:         shaderPath += ".fspv"; break;
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


ION_NAMESPACE_END