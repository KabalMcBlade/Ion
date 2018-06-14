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

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("SimplePosColor", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("SimplePosColor", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);
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

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("SimplePosColor", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("SimplePosColor", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);
}

void Test_TexturedQuad(ion::Entity& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_UV, ion::EPrimitiveType_Quad, _entity);

    eosString dvaTextureFile = ionFileSystemManager().GetTexturesPath();
    dvaTextureFile.append("dva.png");
    ion::Texture* dvaTexture = ionTextureManger().CreateTextureFromFile("dva", dvaTextureFile, ion::ETextureFilter_Default, ion::ETextureRepeat_Clamp, ion::ETextureUsage_RGBA, ion::ETextureType_2D);

    ion::Material* material = ionMaterialManger().CreateMaterial("dva", 0u);
    material->SetAlbedoMap(dvaTexture);

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
    sampler.m_texture = _entity.GetMesh(0)->GetMaterial()->GetAlbedoMap();

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("SimplePosColorTextured", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("SimplePosColorTextured", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColorTextured");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);
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

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("DamagedHelmetWhitePlain", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("DamagedHelmetWhitePlain", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("DamagedHelmetWhitePlain");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);
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
    sampler.m_texture = _entity.GetMesh(0)->GetMaterial()->GetRoughnessMap();

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("DamagedHelmet", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("DamagedHelmet", ion::EShaderStage_Fragment, fragmentLayout);

    _entity.GetMesh(0)->GetMaterial()->SetShaderProgramName("DamagedHelmet");
    _entity.GetMesh(0)->GetMaterial()->SetVertexLayout(_entity.GetMesh(0)->GetLayout());

    _entity.GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);
}
