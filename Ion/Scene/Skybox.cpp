#include "Skybox.h"

#include "../Renderer/RenderManager.h"
#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"
#include "../Shader/ShaderProgramManager.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Skybox::Skybox() : Entity()
{
    m_nodeType = ENodeType_Skybox;
}

Skybox::Skybox(const eosString& _name) : Entity(_name)
{
    m_nodeType = ENodeType_Skybox;
}

Skybox::~Skybox()
{

}

void Skybox::SetLayout(EVertexLayout _layout)
{
    ionRenderManager().LoadPrimitive(_layout, EPrimitiveType_Cube, *this);

    Material* material = ionMaterialManger().CreateMaterial(GetName(), 0u);

    GetMesh(0)->SetMaterial(material);
}

void Skybox::SetCubemap(const Texture* _cubemap)
{
    Material* material = ionMaterialManger().GetMaterial(GetName());
    material->GetBasePBR().SetBaseColorTexture(_cubemap);
}

void Skybox::SetShaders(const eosString& _shaderProgramName, const ConstantsBindingDef& _constants, ionS32 _vertexIndex /*= -1*/, ionS32 _fragmentIndex /*= -1*/, ionS32 _tessellationControlIndex /*= -1*/, ionS32 _tessellationEvaluationIndex /*= -1*/, ionS32 _geometryIndex /*= -1*/)
{
    GetMesh(0)->GetMaterial()->SetShaderProgramName(_shaderProgramName);
    GetMesh(0)->GetMaterial()->SetVertexLayout(GetMesh(0)->GetLayout());

    GetMesh(0)->GetMaterial()->SetConstantsShaders(_constants);

    GetMesh(0)->GetMaterial()->SetShaders(_vertexIndex, _fragmentIndex, _tessellationControlIndex, _tessellationEvaluationIndex, _geometryIndex);

    // default setting
    GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ECullingMode_Front);
    GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(EDepthFunction_Less);
    GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_LesserOrEqual);
}

const Texture* Skybox::GetCubemapTexture() const
{
    return GetMesh(0)->GetMaterial()->GetBasePBR().GetBaseColorTexture();
}

const MaterialState& Skybox::GetMaterialState() const
{
    return GetMesh(0)->GetMaterial()->GetState();
}

MaterialState& Skybox::GetMaterialState()
{
    return GetMesh(0)->GetMaterial()->GetState();
}

ION_NAMESPACE_END