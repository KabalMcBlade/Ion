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

class RenderCore;

struct ION_DLL ShaderLayoutDef final
{
    eosVector(eosString)			m_uniforms;
	eosVector(EUniformParameterType)m_uniformTypes;
    eosVector(EShaderBinding)		m_bindings;
};

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

    const   Vector& GetRenderParmVector(const eosString& _param);
    const   Vector& GetRenderParmVector(ionSize _paramHash);

	const   Matrix& GetRenderParmMatrix(const eosString& _param);
	const   Matrix& GetRenderParmMatrix(ionSize _paramHash);

    void	SetRenderParmVector(const eosString& _param, const ionFloat* _value);
    void	SetRenderParmVector(ionSize _paramHash, const ionFloat* _value);
    void	SetRenderParmsVector(const eosString& _param, const ionFloat* _values, ionU32 _numValues);
    void	SetRenderParmsVector(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues);

	void	SetRenderParmMatrix(const eosString& _param, const ionFloat* _value);
	void	SetRenderParmMatrix(ionSize _paramHash, const ionFloat* _value);
	void	SetRenderParmsMatrix(const eosString& _param, const ionFloat* _values, ionU32 _numValues);
	void	SetRenderParmsMatrix(ionSize _paramHash, const ionFloat* _values, ionU32 _numValues);

    // Shader name WITHOUT extension, because is chose by the shader stage!
    ionS32	FindShader(const eosString& _name, EShaderStage _stage, const ShaderLayoutDef& _defines);

    void	StartFrame();
    void	EndFrame();
    void	BindProgram(ionS32 _index);
    void	CommitCurrent(const RenderCore& _render, ionU64 _stateBits, VkCommandBuffer _commandBuffer);
    ionS32	FindProgram(const eosString& _name, EVertexLayout _vertexLayout, ionS32 _vertexIndex, ionS32 _fragmentIndex = -1, ionS32 _tessellationControlIndex = -1, ionS32 _tessellationEvaluationIndex = -1, ionS32 _geometryIndex = -1, ionBool _useJoint = false, ionBool _useSkinning = false);

    void    Restart();

private:
    ShaderProgramManager(const ShaderProgramManager& _Orig) = delete;
    ShaderProgramManager& operator = (const ShaderProgramManager&) = delete;

    void	LoadShader(ionS32 _index, const ShaderLayoutDef& _defines);
    void	LoadShader(Shader& _shader, const ShaderLayoutDef& _defines);

    void	AllocParametersBlockBuffer(const RenderCore& _render, const eosVector(ionSize) & paramsHash, const eosVector(EUniformParameterType) & paramsType, UniformBuffer& _ubo);

public:
    eosVector(ShaderProgram) m_shaderPrograms;

private:
    VkDevice                m_vkDevice;
    ionS32	                m_current;
    eosVector(Shader)	    m_shaders;
    eosMap(ionSize, Vector) m_uniformsVector; // is a map where the key is the hash of the name of the uniform in the shader and the value the vector associated
	eosMap(ionSize, Matrix) m_uniformsMatrix;

    ionS32				    m_currentDescSet;
    ionSize				    m_currentParmBufferOffset;
    VkDescriptorPool	    m_descriptorPool;
    VkDescriptorSet		    m_descriptorSets[ION_MAX_DESCRIPTOR_SETS];

    UniformBuffer*	        m_skinningUniformBuffer;
    UniformBuffer*	        m_parmBufferVector;
    UniformBuffer*	        m_parmBufferMatrix;

private:
    static ShaderProgramManager *s_instance;
};

ION_NAMESPACE_END


#define ionShaderProgramManager() ion::ShaderProgramManager::Instance()