#include "Test.h"

#include "stdafx.h"

void Test_ColoredTriangle(ion::Entity& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Triangle, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptyTriangle", 0u);
    _entity.GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

void Test_ColoredQuad(ion::Entity& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Quad, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptyQuad", 0u);
    _entity.GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

void Test_TexturedQuad(ion::Entity& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_UV, ion::EPrimitiveType_Quad, _entity);

    eosString dvaTextureFile = ionFileSystemManager().GetTexturesPath();
    dvaTextureFile.append("dva.png");
    ion::Texture* dvaTexture = ionTextureManger().CreateTextureFromFile("dva", dvaTextureFile, ion::ETextureFilter_Default, ion::ETextureRepeat_Clamp, ion::ETextureUsage_RGBA, ion::ETextureType_2D);

    ion::Material* material = ionMaterialManger().CreateMaterial("dva", 0u);
    material->GetBasePBR().SetBaseColorTexture(dvaTexture);

    _entity.GetMesh(0)->SetMaterial(material);

    //
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
    sampler.m_texture = _entity.GetMesh(0)->GetMaterial()->GetBasePBR().GetBaseColorTexture();

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColorTextured", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColorTextured", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColorTextured");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

void Test_ColoredCube(ion::Entity& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Cube, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptyCube", 0u);
    _entity.GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_Back);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}


void Test_ColoredSphere(ion::Entity& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Sphere, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptySphere", 0u);
    _entity.GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_Back);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

void Test_Model_Ambient(ion::Entity& _entity)
{
    eosString damagedHelmetModelPath = ionFileSystemManager().GetModelsPath();
    damagedHelmetModelPath.append("DamagedHelmet.gltf");
    ionRenderManager().LoadModelFromFile(damagedHelmetModelPath, _entity);

    //
    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "DamagedHelmetWhitePlain", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "DamagedHelmetWhitePlain", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("DamagedHelmetWhitePlain");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_Back);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetBlendStateMode(ion::EBlendState_Source_One);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetBlendStateMode(ion::EBlendState_Dest_Zero);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetBlendOperatorMode(ion::EBlendOperator_Add);
}

void Test_ModelPBR_WIP(ion::Entity& _entity)
{
    eosString damagedHelmetModelPath = ionFileSystemManager().GetModelsPath();
    damagedHelmetModelPath.append("DamagedHelmet.gltf");
    ionRenderManager().LoadModelFromFile(damagedHelmetModelPath, _entity);

    //
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
    sampler.m_texture = _entity.GetMesh(0)->GetMaterial()->GetBasePBR().GetBaseColorTexture();

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "DamagedHelmet", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "DamagedHelmet", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("DamagedHelmet");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_Back);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetBlendStateMode(ion::EBlendState_Source_One);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetBlendStateMode(ion::EBlendState_Dest_Zero);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetBlendOperatorMode(ion::EBlendOperator_Add);
}


void Test_DrawBoundingBox(ion::Entity& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Cube, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("BoundingBox", 0u);
    _entity.GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity.GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetRasterizationMode(ion::ERasterization_PolygonMode_Line);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity.GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);

    _entity.SetVisible(false);
}