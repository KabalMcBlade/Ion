#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/UniformBufferObject.h"

#include "ShaderProgram.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Material;
class RenderCore;
class ION_DLL ShaderProgramManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static ShaderProgramManager& Instance();

    ionBool Init(VkDevice _vkDevice);
    void    Shutdown();

    ShaderProgramManager();
    ~ShaderProgramManager();



    /*
    IMPORTANT
    When a uniform is defined, in order to optimize the memory, I assume that all type are
    grouped.
    So in the shader all uniform must be set in this way:
    - ALL MATRIX
    - ALL VECTOR
    - ALL FLOAT
    - ALL INTEGER
    as well as in the shader layout on code side.
    */

    //////////////////////////////////////////////////////////////////////////
    // if parameter not found, return a vector 0 and create this new hash! BE CAREFUL!
    const   Matrix& GetRenderParamMatrix(const eosString& _param);
    const   Matrix& GetRenderParamMatrix(ionSize _paramHash);

    const   Vector& GetRenderParamVector(const eosString& _param);
    const   Vector& GetRenderParamVector(ionSize _paramHash);

    const   ionFloat GetRenderParamFloat(const eosString& _param);
    const   ionFloat GetRenderParamFloat(ionSize _paramHash);

    const   ionS32 GetRenderParamInteger(const eosString& _param);
    const   ionS32 GetRenderParamInteger(ionSize _paramHash);

    void    SetRenderParamMatrix(const eosString& _param, const ionFloat* _value);
    void    SetRenderParamMatrix(ionSize _paramHash, const ionFloat* _value);
    void    SetRenderParamsMatrix(const eosString& _param, const ionFloat* _values, ionU32 _numValues);
    //void    SetRenderParamsMatrix(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues);

    void    SetRenderParamVector(const eosString& _param, const ionFloat* _value);
    void    SetRenderParamVector(ionSize _paramHash, const ionFloat* _value);
    void    SetRenderParamsVector(const eosString& _param, const ionFloat* _values, ionU32 _numValues);
    //void    SetRenderParamsVector(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues);

    void    SetRenderParamFloat(const eosString& _param, const ionFloat _value);
    void    SetRenderParamFloat(ionSize _paramHash, const ionFloat _value);
    void    SetRenderParamsFloat(const eosString& _param, const ionFloat* _values, ionU32 _numValues);
    //void    SetRenderParamsFloat(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues);

    void    SetRenderParamInteger(const eosString& _param, const ionS32 _value);
    void    SetRenderParamInteger(ionSize _paramHash, const ionS32 _value);
    void    SetRenderParamsInteger(const eosString& _param, const ionS32* _values, ionU32 _numValues);
    //void    SetRenderParamsInteger(ionSize _paramHash, const ionS32* _values, ionU32 _numValues);

    //////////////////////////////////////////////////////////////////////////

    // Shader name WITHOUT extension, because is chose by the shader stage!
    ionS32  FindShader(const eosString& _path, const eosString& _name, EShaderStage _stage);
    ionS32  FindShader(const eosString& _path, const eosString& _name, EShaderStage _stage, const eosVector(ionFloat)& _specializationConstantValues);

    void    StartFrame();
    void    EndFrame();
    void    BindProgram(ionS32 _index);
    void    CommitCurrent(const RenderCore& _render, const Material* _material, VkRenderPass _renderPass, ionU64 _stateBits, VkCommandBuffer _commandBuffer);
    ionS32  FindProgram(const Material* _material);

    void    UnloadShader(ionS32 _index);

    void    Restart();

private:
    ShaderProgramManager(const ShaderProgramManager& _Orig) = delete;
    ShaderProgramManager& operator = (const ShaderProgramManager&) = delete;

    void    LoadShader(ionS32 _index);
    void    LoadShader(Shader& _shader);

    void    AllocUniformParametersBlockBuffer(const RenderCore& _render, const UniformBinding& _uniform, UniformBuffer& _ubo);

public:
    eosVector(ShaderProgram) m_shaderPrograms;

private:
    VkDevice                m_vkDevice;
    ionS32                  m_current;
    eosVector(Shader)       m_shaders;

    // are a map where the key is the hash of the name of the uniform in the shader and the value the vector associated
    eosMap(ionSize, Vector)     m_uniformsVector; 
    eosMap(ionSize, Matrix)     m_uniformsMatrix;
    eosMap(ionSize, ionFloat)   m_uniformsFloat; 
    eosMap(ionSize, ionS32)     m_uniformsInteger;

    ionS32                  m_currentDescSet;
    ionSize                 m_currentParmBufferOffset;
    VkDescriptorPool        m_descriptorPool;
    VkDescriptorSet         m_descriptorSets[ION_MAX_DESCRIPTOR_SETS];

    UniformBuffer*          m_skinningUniformBuffer;
    UniformBuffer*          m_uniformBuffer;
private:
    static ShaderProgramManager *s_instance;
};

ION_NAMESPACE_END


#define ionShaderProgramManager() ion::ShaderProgramManager::Instance()