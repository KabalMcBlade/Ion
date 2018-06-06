#include "ShaderProgram.h"

#include "ShaderProgramHelper.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN



ShaderProgram::ShaderProgram() :
    m_usesJoints(false),
    m_usesSkinning(false),
    m_vertexShaderIndex(-1),
    m_fragmentShaderIndex(-1),
    m_tessellationControlShaderIndex(-1),
    m_tessellationEvaluatorShaderIndex(-1),
    m_geometryShaderIndex(-1),
    m_vertextLayoutType(EVertexLayout_Full),
    m_pipelineLayout(VK_NULL_HANDLE),
    m_descriptorSetLayout(VK_NULL_HANDLE) 
{

}

VkPipeline ShaderProgram::GetPipeline(const RenderCore& _render, ionU64 _stateBits, VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkShaderModule _tessellationControlShader  /*= VK_NULL_HANDLE*/, VkShaderModule _tessellationEvaluatorShader /*= VK_NULL_HANDLE*/, VkShaderModule _geometryShader /*= VK_NULL_HANDLE*/)
{
    for (ionU32 i = 0; i < m_pipelines.size(); ++i) 
    {
        if (_stateBits == m_pipelines[i].m_stateBits)
        {
            return m_pipelines[i].m_pipeline;
        }
    }

    VkPipeline pipeline = ShaderProgramHelper::CreateGraphicsPipeline(_render, m_vertextLayoutType, m_pipelineLayout, _stateBits, _vertexShader, _fragmentShader, _tessellationControlShader, _tessellationEvaluatorShader, _geometryShader);

    PipelineState pipelineState;
    pipelineState.m_pipeline = pipeline;
    pipelineState.m_stateBits = _stateBits;
    m_pipelines.push_back(pipelineState);

    return pipeline;
}

ShaderProgram::PipelineState::PipelineState() :
    m_stateBits(0),
    m_pipeline(VK_NULL_HANDLE) 
{

}

ION_NAMESPACE_END
