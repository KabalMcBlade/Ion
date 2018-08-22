#include "Test.h"

#include "stdafx.h"

void Test_ColoredTriangle(ion::ObjectHandler& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Triangle, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptyTriangle", 0u);
    _entity->GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    _entity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment);

    _entity->GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity->GetMesh(0)->GetMaterial()->SetVertexLayout(_entity->GetMesh(0)->GetLayout());

    _entity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

void Test_ColoredQuad(ion::ObjectHandler& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Quad, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptyQuad", 0u);
    _entity->GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    _entity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment);

    _entity->GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity->GetMesh(0)->GetMaterial()->SetVertexLayout(_entity->GetMesh(0)->GetLayout());

    _entity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

void Test_TexturedQuad(ion::ObjectHandler& _entity)
{
    eosString textureFile = ionFileSystemManager().GetTexturesPath();
    textureFile.append("Yokohama3_front.jpg");
    ion::Texture* texture = ionTextureManger().CreateTextureFromFile("Yokohama3_front", textureFile, ion::ETextureFilter_Default, ion::ETextureRepeat_Clamp, ion::ETextureUsage_RGBA, ion::ETextureType_2D);

    Test_TexturedQuadEx(_entity, texture);
}

void Test_TexturedQuadEx(ion::ObjectHandler& _entity, ion::Texture* _texture)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_UV, ion::EPrimitiveType_Quad, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("ashcanyon_front", 0u);
    material->GetBasePBR().SetBaseColorTexture(_texture);

    _entity->GetMesh(0)->SetMaterial(material);

    //
    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // one sampler bound in the index 1 in the shader stage
    ion::SamplerBinding sampler;
    sampler.m_bindingIndex = 1;
    sampler.m_texture = _entity->GetMesh(0)->GetMaterial()->GetBasePBR().GetBaseColorTexture();

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    _entity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);
    _entity->GetMesh(0)->GetMaterial()->SetFragmentShaderLayout(fragmentLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColorTextured", ion::EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColorTextured", ion::EShaderStage_Fragment);

    _entity->GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColorTextured");
    _entity->GetMesh(0)->GetMaterial()->SetVertexLayout(_entity->GetMesh(0)->GetLayout());

    _entity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}


void Test_ColoredCube(ion::ObjectHandler& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Cube, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptyCube", 0u);
    _entity->GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    _entity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment);

    _entity->GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity->GetMesh(0)->GetMaterial()->SetVertexLayout(_entity->GetMesh(0)->GetLayout());

    _entity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_Back);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}


void Test_ColoredSphere(ion::ObjectHandler& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Sphere, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("EmptySphere", 0u);
    _entity->GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    _entity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment);

    _entity->GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity->GetMesh(0)->GetMaterial()->SetVertexLayout(_entity->GetMesh(0)->GetLayout());

    _entity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_Back);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);
}

void Test_ModelPBR_Recursive(ion::ObjectHandler& _entity, ion::Texture* _brdf, ion::Texture* _irradiance, ion::Texture* _prefiltered)
{
    const ionU32 meshCount = _entity->GetMeshCount();
    for (ionU32 i = 0; i < meshCount; i++)
    {
        const ion::BaseMesh* mesh = _entity->GetMesh(i);

        //
        ion::UniformBinding uniformVertex;
        uniformVertex.m_bindingIndex = 0;
        uniformVertex.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
        uniformVertex.m_type.push_back(ion::EUniformParameterType_Matrix);
        uniformVertex.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
        uniformVertex.m_type.push_back(ion::EUniformParameterType_Matrix);
        uniformVertex.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
        uniformVertex.m_type.push_back(ion::EUniformParameterType_Matrix);

        //
        ion::UniformBinding uniformFragment;
        uniformFragment.m_bindingIndex = 1;
        uniformFragment.m_parameters.push_back(ION_MAIN_CAMERA_POSITION_VECTOR_PARAM);
        uniformFragment.m_type.push_back(ion::EUniformParameterType_Vector);
        uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_DIR_VECTOR_PARAM);
        uniformFragment.m_type.push_back(ion::EUniformParameterType_Vector);
        uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_COL_VECTOR_PARAM);
        uniformFragment.m_type.push_back(ion::EUniformParameterType_Vector);
        uniformFragment.m_parameters.push_back(ION_EXPOSURE_FLOAT_PARAM);
        uniformFragment.m_type.push_back(ion::EUniformParameterType_Float);
        uniformFragment.m_parameters.push_back(ION_GAMMA_FLOAT_PARAM);
        uniformFragment.m_type.push_back(ion::EUniformParameterType_Float);
        uniformFragment.m_parameters.push_back(ION_PREFILTERED_CUBE_MIP_LEVELS_FLOAT_PARAM);
        uniformFragment.m_type.push_back(ion::EUniformParameterType_Float);

        //
        ion::SamplerBinding samplerIrradiance;
        samplerIrradiance.m_bindingIndex = 2;
        samplerIrradiance.m_texture = _irradiance;

        ion::SamplerBinding prefilteredMap;
        prefilteredMap.m_bindingIndex = 3;
        prefilteredMap.m_texture = _prefiltered;

        ion::SamplerBinding samplerBRDFLUT;
        samplerBRDFLUT.m_bindingIndex = 4;
        samplerBRDFLUT.m_texture = _brdf;

        //
        ionBool usingSpecularGlossiness = mesh->GetMaterial()->IsUsingSpecularGlossiness();

        //
        ion::SamplerBinding albedoMap;
        albedoMap.m_bindingIndex = 5;
        albedoMap.m_texture = usingSpecularGlossiness ? mesh->GetMaterial()->GetSpecularGlossiness().GetBaseColorTexture() : mesh->GetMaterial()->GetBasePBR().GetBaseColorTexture();

        ion::SamplerBinding normalMap;
        normalMap.m_bindingIndex = 6;
        normalMap.m_texture = mesh->GetMaterial()->GetAdvancePBR().GetNormalTexture();

        ion::SamplerBinding aoMap;
        aoMap.m_bindingIndex = 7;
        aoMap.m_texture = mesh->GetMaterial()->GetAdvancePBR().GetOcclusionTexture();

        ion::SamplerBinding physicalDescriptorMap;
        physicalDescriptorMap.m_bindingIndex = 8;
        physicalDescriptorMap.m_texture = usingSpecularGlossiness ? mesh->GetMaterial()->GetSpecularGlossiness().GetSpecularGlossinessTexture() : mesh->GetMaterial()->GetBasePBR().GetMetalRoughnessTexture();

        ion::SamplerBinding emissiveMap;
        emissiveMap.m_bindingIndex = 9;
        emissiveMap.m_texture = mesh->GetMaterial()->GetAdvancePBR().GetEmissiveTexture();


        // set the shaders layout
        ion::ShaderLayoutDef vertexLayout;
        vertexLayout.m_uniforms.push_back(uniformVertex);

        ion::ShaderLayoutDef fragmentLayout;
        fragmentLayout.m_uniforms.push_back(uniformFragment);
        fragmentLayout.m_samplers.push_back(samplerIrradiance);
        fragmentLayout.m_samplers.push_back(prefilteredMap);
        fragmentLayout.m_samplers.push_back(samplerBRDFLUT);
        fragmentLayout.m_samplers.push_back(albedoMap);
        fragmentLayout.m_samplers.push_back(normalMap);
        fragmentLayout.m_samplers.push_back(aoMap);
        fragmentLayout.m_samplers.push_back(physicalDescriptorMap);
        fragmentLayout.m_samplers.push_back(emissiveMap);

        //
        /*
        layout (push_constant) uniform Material
        {
        float baseColorFactorR;
        float baseColorFactorG;
        float baseColorFactorB;
        float baseColorFactorA;
        float emissiveFactorR;
        float emissiveFactorG;
        float emissiveFactorB;
        float emissiveFactorA;
        float diffuseFactorR;
        float diffuseFactorG;
        float diffuseFactorB;
        float diffuseFactorA;
        float specularFactorFactorR;
        float specularFactorFactorG;
        float specularFactorFactorB;
        float specularFactorFactorA;
        float usingSpecularGlossiness;
        float hasBaseColorTexture;
        float hasPhysicalDescriptorTexture;
        float hasNormalTexture;
        float hasOcclusionTexture;
        float hasEmissiveTexture;
        float metallicFactor;
        float roughnessFactor;
        float alphaMask;
        float alphaMaskCutoff;
        } material;
        */

        ion::ConstantsBindingDef constants;
        constants.m_shaderStages = ion::EPushConstantStage::EPushConstantStage_Fragment;
        constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetColor()[0]);
        constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetColor()[1]);
        constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetColor()[2]);
        constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetColor()[3]);
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetEmissiveColor()[0]);
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetEmissiveColor()[1]);
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetEmissiveColor()[2]);
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetEmissiveColor()[3]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetBaseColor()[0]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetBaseColor()[1]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetBaseColor()[2]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetBaseColor()[3]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetSpecularGlossinessColor()[0]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetSpecularGlossinessColor()[1]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetSpecularGlossinessColor()[2]);
        constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetSpecularGlossinessColor()[3]);
        constants.m_values.push_back(usingSpecularGlossiness);
        if (usingSpecularGlossiness)
        {
            constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetBaseColorTexture() != nullptr ? 1.0f : 0.0f);
            constants.m_values.push_back(mesh->GetMaterial()->GetSpecularGlossiness().GetSpecularGlossinessColor() != nullptr ? 1.0f : 0.0f);
        }
        else
        {
            constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetBaseColorTexture() != nullptr ? 1.0f : 0.0f);
            constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetMetalRoughnessTexture() != nullptr ? 1.0f : 0.0f);
        }
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetNormalTexture() != nullptr ? 1.0f : 0.0f);
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetOcclusionTexture() != nullptr ? 1.0f : 0.0f);
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetEmissiveTexture() != nullptr ? 1.0f : 0.0f);
        constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetMetallicFactor());
        constants.m_values.push_back(mesh->GetMaterial()->GetBasePBR().GetRoughnessFactor());
        constants.m_values.push_back(mesh->GetMaterial()->GetAlphaMode() == ion::EAlphaMode_Mask ? 1.0f : 0.0f);
        constants.m_values.push_back(mesh->GetMaterial()->GetAdvancePBR().GetAlphaCutoff());

        mesh->GetMaterial()->SetVertexShaderLayout(vertexLayout);
        mesh->GetMaterial()->SetFragmentShaderLayout(fragmentLayout);

        ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "pbr", ion::EShaderStage_Vertex);
        ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "pbr", ion::EShaderStage_Fragment);

        mesh->GetMaterial()->SetShaderProgramName("pbr");
        mesh->GetMaterial()->SetVertexLayout(mesh->GetLayout());

        mesh->GetMaterial()->SetConstantsShaders(constants);
        mesh->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);
    }

    if (_entity->GetChildren().empty())
    {
        return;
    }

    eosVector(ion::ObjectHandler)::const_iterator begin = _entity->GetChildIteratorBegin(), end = _entity->GetChildIteratorEnd(), it = begin;
    for (; it != end; ++it)
    {
        ion::ObjectHandler node = (*it);
        Test_ModelPBR_Recursive(node, _brdf, _irradiance, _prefiltered);
    }
}

void Test_ModelPBR(ion::ObjectHandler& _entity, ion::Texture* _brdf, ion::Texture* _irradiance, ion::Texture* _prefiltered)
{
    eosString damagedHelmetModelPath = ionFileSystemManager().GetModelsPath();
    damagedHelmetModelPath.append("DamagedHelmet.gltf");
    ionRenderManager().LoadModelFromFile(damagedHelmetModelPath, _entity);

    static const Vector right(1.0f, 0.0f, 0.0f, 0.0f);
    _entity->GetTransform().SetRotation(NIX_DEG_TO_RAD(90.0f), right);

    Test_ModelPBR_Recursive(_entity, _brdf, _irradiance, _prefiltered);
}


void Test_DrawBoundingBox(ion::ObjectHandler& _entity)
{
    ionRenderManager().LoadPrimitive(ion::EVertexLayout_Pos_Color, ion::EPrimitiveType_Cube, _entity);

    ion::Material* material = ionMaterialManger().CreateMaterial("BoundingBox", 0u);
    _entity->GetMesh(0)->SetMaterial(material);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EUniformParameterType_Matrix);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    _entity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SimplePosColor", ion::EShaderStage_Fragment);

    _entity->GetMesh(0)->GetMaterial()->SetShaderProgramName("SimplePosColor");
    _entity->GetMesh(0)->GetMaterial()->SetVertexLayout(_entity->GetMesh(0)->GetLayout());

    _entity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _entity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ion::ECullingMode_TwoSide);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetRasterizationMode(ion::ERasterization_PolygonMode_Line);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(ion::EDepthFunction_Less);
    _entity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(ion::EStencilFrontFunction_LesserOrEqual);

    _entity->SetVisible(false);
}
