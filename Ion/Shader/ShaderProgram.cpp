#include "ShaderProgram.h"

#include "ShaderProgramHelper.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


ShaderProgram::ShaderProgram() :
    m_vertextLayoutType(EVertexLayout_Full),
    m_pipelineLayout(VK_NULL_HANDLE),
    m_descriptorSetLayout(VK_NULL_HANDLE),
    m_material(nullptr)
{

}

ShaderProgram::~ShaderProgram()
{
    m_material = nullptr;
}

VkPipeline ShaderProgram::GetPipeline(const RenderCore& _render, VkRenderPass _renderPass, ionU64 _stateBits, VkPrimitiveTopology _topology, 
    VkShaderModule _vertexShader /*= VK_NULL_HANDLE*/, VkShaderModule _fragmentShader /*= VK_NULL_HANDLE*/, VkShaderModule _tessellationControlShader /*= VK_NULL_HANDLE*/, VkShaderModule _tessellationEvaluatorShader /*= VK_NULL_HANDLE*/, VkShaderModule _geometryShader /*= VK_NULL_HANDLE*/,
    SpecializationConstants* _vertexSpecConst /*= nullptr*/, SpecializationConstants* _fragmentSpecConst /*= nullptr*/, SpecializationConstants* _tessCtrlSpecConst /*= nullptr*/, SpecializationConstants* _tessEvalSpecConst /*= nullptr*/, SpecializationConstants* _geomSpecConst /*= nullptr*/)
{
    for (ionU32 i = 0; i < m_pipelines.size(); ++i) 
    {
        // same state and same renderpass
        if (_stateBits == m_pipelines[i].m_stateBits && _renderPass == m_pipelines[i].m_renderpass)
        {
            return m_pipelines[i].m_pipeline;
        }
    }

    VkPipeline pipeline = ShaderProgramHelper::CreateGraphicsPipeline(_render, _renderPass, _topology, m_vertextLayoutType, m_pipelineLayout, _stateBits, _vertexShader, _fragmentShader, _tessellationControlShader, _tessellationEvaluatorShader, _geometryShader,
        _vertexSpecConst, _fragmentSpecConst, _tessCtrlSpecConst, _tessEvalSpecConst, _geomSpecConst);

    PipelineState pipelineState;
    pipelineState.m_pipeline = pipeline;
    pipelineState.m_stateBits = _stateBits;
    pipelineState.m_renderpass = _renderPass;
    m_pipelines.push_back(pipelineState);

    return pipeline;
}

ShaderProgram::PipelineState::PipelineState() :
    m_stateBits(0),
    m_pipeline(VK_NULL_HANDLE) 
{

}

ION_NAMESPACE_END
