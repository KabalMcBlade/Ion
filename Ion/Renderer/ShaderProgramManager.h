#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "RenderCommon.h"
#include "ShaderProgram.h"
#include "UniformBufferObject.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderCore;

struct ION_DLL ShaderLayoutDef final
{
    eosVector(eosString) m_uniforms;
    eosVector(EShaderBinding) m_bindings;
};

class ION_DLL ShaderProgramManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static ShaderProgramManager& Instance();

    ionBool Init(VkDevice _vkDevice, const eosString& _shaderFolderPath);
    void    Shutdown();

    ShaderProgramManager();
    ~ShaderProgramManager();

    const   Vector& GetRenderParm(const eosString& _param);
    const   Vector& GetRenderParm(ionSize _paramHash);
    void	SetRenderParm(const eosString& _param, const ionFloat* _value);
    void	SetRenderParm(ionSize _paramHash, const ionFloat* _value);
    void	SetRenderParms(const eosString& _param, const ionFloat* _values, ionU32 _numValues);
    void	SetRenderParms(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues);

    // Shader name WITHOUT extension!!
    ionS32	FindShader(const eosString& _name, EShaderStage _stage, const ShaderLayoutDef& _defines);

    void	StartFrame();
    void	BindProgram(ionS32 _index);
    void	CommitCurrent(const RenderCore& _render, ionU64 _stateBits, VkCommandBuffer _commandBuffer);

private:
    ShaderProgramManager(const ShaderProgramManager& _Orig) = delete;
    ShaderProgramManager& operator = (const ShaderProgramManager&) = delete;

    void	LoadShader(ionS32 _index, const ShaderLayoutDef& _defines);
    void	LoadShader(Shader& _shader, const ShaderLayoutDef& _defines);

    void	AllocParametersBlockBuffer(const RenderCore& _render, const eosVector(ionSize) & paramsHash, UniformBuffer& _ubo);

public:
    eosVector(ShaderProgram) m_shaderPrograms;

private:
    VkDevice                m_vkDevice;
    ionS32	                m_current;
    eosString               m_shaderFolderPath;
    eosVector(Shader)	    m_shaders;
    eosMap(ionSize, Vector) m_uniforms; // is a map where the key is the hash of the name of the uniform in the shader and the value the vector associated

    ionS32				m_counter;
    ionS32				m_currentData;
    ionS32				m_currentDescSet;
    ionSize				m_currentParmBufferOffset;
    VkDescriptorPool	m_descriptorPools[ION_RENDER_BUFFER_COUNT];
    VkDescriptorSet		m_descriptorSets[ION_RENDER_BUFFER_COUNT][ION_MAX_DESCRIPTOR_SETS];

    UniformBuffer*	    m_skinningUniformBuffer;
    UniformBuffer*	    m_parmBuffers[ION_RENDER_BUFFER_COUNT];

private:
    static ShaderProgramManager *s_instance;
};

ION_NAMESPACE_END


#define ionShaderProgramManager() ion::ShaderProgramManager::Instance()