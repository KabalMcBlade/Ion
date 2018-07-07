#include "Skybox.h"

#include "../Renderer/RenderManager.h"
#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"
#include "../Shader/ShaderProgramManager.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Skybox::Skybox(const eosString& _filepath, const eosString& _filename, EVertexLayout _layout,
    const eosString& _vertexShaderPath, const eosString& _vertexShadername, const eosString& _fragmentShaderPath, const eosString& _fragmentShadername) :
    Entity(_filename)
{
    m_nodeType = ENodeType_Skybox;


    ionRenderManager().LoadPrimitive(_layout, ion::EPrimitiveType_Cube, *this);

    eosString skyboxFilePatg = _filepath;
    skyboxFilePatg.append(_filename);
    ion::Texture* skyBoxTexture = ionTextureManger().CreateTextureFromFile(_filename, skyboxFilePatg, ETextureFilter_Default, ETextureRepeat_Clamp, ETextureUsage_Skybox, ETextureType_Cubic, 1);

    ion::Material* material = ionMaterialManger().CreateMaterial(_filename, 0u);
    material->GetBasePBR().SetBaseColorTexture(skyBoxTexture);

    GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // one sampler bound in the index 1 in the shader stage
    ion::SamplerBinding sampler;
    sampler.m_bindingIndex = 1;
    sampler.m_texture = GetMesh(0)->GetMaterial()->GetBasePBR().GetBaseColorTexture();

    // coinstants
    ConstantsBindingDef constants;
    constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
    constants.m_values.push_back(4.5f);
    constants.m_values.push_back(2.2f);
    GetMesh(0)->GetMaterial()->SetConstantsShaders(constants);


    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(_vertexShaderPath, _vertexShadername, ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(_fragmentShaderPath, _fragmentShadername, ion::EShaderStage_Fragment, fragmentLayout);

    GetMesh(0)->GetMaterial()->SetShaderProgramName(_vertexShadername);
    GetMesh(0)->GetMaterial()->SetVertexLayout(GetMesh(0)->GetLayout());

    GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_Front);
    GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

Skybox::~Skybox()
{

}

MaterialState& Skybox::GetMaterialState()
{
    return GetMesh(0)->GetMaterial()->GetState();
}

ION_NAMESPACE_END