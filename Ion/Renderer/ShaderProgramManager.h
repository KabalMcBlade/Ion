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

class ShaderProgramManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static ShaderProgramManager& Instance();

    ionBool Init(const eosString& _shaderFolderPath);
    void    Shutdown();

    ShaderProgramManager();
    ~ShaderProgramManager();

    const   Vector& GetRenderParm(const eosString& _param);
    const   Vector& GetRenderParm(ionSize _paramHash);
    void	SetRenderParm(const eosString& _param, const ionFloat* _value);
    void	SetRenderParm(ionSize _paramHash, const ionFloat* _value);
    void	SetRenderParms(const eosString& _param, const ionFloat* _values, ionS32 _numValues);
    void	SetRenderParms(ionSize _paramHash, const ionFloat* _values, ionS32 _numValues);

    // Shader name WITHOUT extension!!
    ionS32	FindShader(const eosString& _name, EShaderStage _stage);

    void	StartFrame();
    void    EndFrame();

private:
    ShaderProgramManager(const ShaderProgramManager& _Orig) = delete;
    ShaderProgramManager& operator = (const ShaderProgramManager&) = delete;

    void	LoadShader(ionS32 _index);
    void	LoadShader(Shader& _shader);

public:
    eosList(ShaderProgram) m_shaderPrograms;

private:
    ionS32	                m_current;
    eosString               m_shaderFolderPath;
    eosVector(Shader)	    m_shaders;
    eosMap(ionSize, Vector) m_uniforms; // is a map where the key is the hash of the name of the uniform in the shader and the value the vector associated

    ionS32				m_counter;
    ionS32				m_currentData;
    ionS32				m_currentDescSet;
    ionS32				m_currentParmBufferOffset;
    VkDescriptorPool	m_descriptorPools[ION_RENDER_BUFFER_COUNT];
    VkDescriptorSet		m_descriptorSets[ION_RENDER_BUFFER_COUNT][ION_MAX_DESCRIPTOR_SETS];

    UniformBuffer*	    m_parmBuffers[ION_RENDER_BUFFER_COUNT];

private:
    static ShaderProgramManager *s_instance;
};

ION_NAMESPACE_END


#define ionShaderProgramManager() ion::ShaderProgramManager::Instance()