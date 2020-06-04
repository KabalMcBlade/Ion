#include "RenderManager.h"

#include "VertexCacheManager.h"
#include "StagingBufferManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"
#include "../Scene/DirectionalLight.h"
#include "../Scene/Skybox.h"

#include "../Geometry/Mesh.h"
#include "../Geometry/PrimitiveFactory.h"

#include "../Texture/TextureManager.h"

#include "../Core/FileSystemManager.h"
#include "../Core/UUID.h"

#include "../Shader/ShaderProgramManager.h"

#include "../Material/MaterialManager.h"


#define ION_BRDFLUT_TEXTURENAME    "BRDFLUT"
#define ION_BRDFLUT_SHADER_NAME    "GenerateBRDFLUT"

#define ION_IRRADIANCE_TEXTURENAME                  "IRRADIANCE"
#define ION_PREFILTEREDENVIRONMENT_TEXTURENAME      "PREFILTERED_ENVIRONMENT"

#define ION_IRRADIANCE_TEXTURENAME_OFFSCREEN                "IRRADIANCE_OFFSCREEN"
#define ION_PREFILTEREDENVIRONMENT_TEXTURENAME_OFFSCREEN    "PREFILTERED_ENVIRONMENT_OFFSCREEN"

#define ION_IRRADIANCE_PREFILTERED_VERTEX_SHADER_NAME       "FilteredCube"
#define ION_IRRADIANCE_FRAGMENT_SHADER_NAME                 "IrradianceCube"
#define ION_PREFILTEREDENVIRONMENT_FRAGMENT_SHADER_NAME     "PrefilteredEnvironmentMap"

//#define SHADOW_MAP_SIZE                    1024

#define ION_CACHE_LINE_SIZE        128
#define ION_MAX_FRAME_MEMORY    67305472    //64 * 1024 * 1024


#define ION_FPS_LIMIT  0.01666666666666666666666666666667f     // 1.0 / 60.0f



EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderManagerAllocator* RenderManager::GetAllocator()
{
	static HeapArea<Settings::kRenderManagerAllocatorSize> memoryArea;
	static RenderManagerAllocator memoryAllocator(memoryArea, "RenderManagerFreeListAllocator");

	return &memoryAllocator;
}

RenderManager::RenderManager() : m_deltaTime(ION_FPS_LIMIT), m_running(false)
{
    m_exposure = 4.5f;
    m_gamma = 2.2f;
    m_prefilteredCubeMipLevels = 32.0f;
}

RenderManager::~RenderManager()
{

}

ionBool RenderManager::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    if (m_renderCore.Init(_instance, _handle, _width, _height, _fullScreen, _enableValidationLayer, _vkDeviceLocalSize, _vkHostVisibleSize, _vkStagingBufferSize))
    {
        m_running = true;
        return true;
    }
    else
    {
        return false;
    }
}

void RenderManager::Shutdown()
{
    //m_sceneGraph.RemoveAll();
    m_renderCore.Shutdown();
}

RenderManager& RenderManager::Instance()
{
    static RenderManager instance;
    return instance;
}

ionBool RenderManager::LoadModelFromFile(const ionString& _filePath, Camera* _camToUpdate, Node*& _entity)
{
    return m_loader.Load(_filePath, _camToUpdate, _entity);
}

void RenderManager::DumpModelToFile(const ionString& _filePath, Node* _entity, LoaderGLTF::ESerializationLevel _level )
{
    m_loader.Dump(_filePath, _entity, _level);
}

void RenderManager::GeneratePrimitive(EVertexLayout _layout, EPrimitiveType _type, Entity*& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	switch (_type)
	{
	case EPrimitiveType_Triangle:
		PrimitiveFactory::GenerateTriangle(_layout, _entity, _r, _g, _b, _a);
		break;
	case EPrimitiveType_Quad:
		PrimitiveFactory::GenerateQuad(_layout, _entity, _r, _g, _b, _a);
		break;
	case EPrimitiveType_Cube:
		PrimitiveFactory::GenerateCube(_layout, _entity, _r, _g, _b, _a);
		break;
	case EPrimitiveType_Sphere:
		PrimitiveFactory::GenerateSphere(_layout, _entity, _r, _g, _b, _a);
		break;
	case EPrimitiveType_Pyramid:
		PrimitiveFactory::GeneratePyramd(_layout, _entity, _r, _g, _b, _a);
		break;
	}
}

void RenderManager::LoadColoredTriangle(Entity*& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    GeneratePrimitive(EVertexLayout_Full, EPrimitiveType_Triangle, _entity, _r, _g, _b, _a);

    Material* material = ionMaterialManger().CreateMaterial("ION#Triangle");
    _entity->GetMesh(0)->SetMaterial(material);

    LoadCommonMaterialForIntegratedPrimitive(_entity, material);

    material->GetState().UnsetCullingMode(ECullingMode_Back);
    material->GetState().SetCullingMode(ECullingMode_TwoSide);
}

void RenderManager::LoadColoredQuad(Entity*& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    GeneratePrimitive(EVertexLayout_Full, EPrimitiveType_Quad, _entity, _r, _g, _b, _a);

    Material* material = ionMaterialManger().CreateMaterial("ION#Quad");
    _entity->GetMesh(0)->SetMaterial(material);

    LoadCommonMaterialForIntegratedPrimitive(_entity, material);

    material->GetState().UnsetCullingMode(ECullingMode_Back);
    material->GetState().SetCullingMode(ECullingMode_TwoSide);
}

void RenderManager::LoadColoredCube(Entity*& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    GeneratePrimitive(EVertexLayout_Full, EPrimitiveType_Cube, _entity, _r, _g, _b, _a);

    Material* material = ionMaterialManger().CreateMaterial("ION#Cube");
    _entity->GetMesh(0)->SetMaterial(material);

    LoadCommonMaterialForIntegratedPrimitive(_entity, material);
}

void RenderManager::LoadColoredSphere(Entity*& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    GeneratePrimitive(EVertexLayout_Full, EPrimitiveType_Sphere, _entity, _r, _g, _b, _a);

    Material* material = ionMaterialManger().CreateMaterial("ION#Sphere");
    _entity->GetMesh(0)->SetMaterial(material);

    LoadCommonMaterialForIntegratedPrimitive(_entity, material);
}

void RenderManager::LoadColoredPyramid(Entity*& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    GeneratePrimitive(EVertexLayout_Full, EPrimitiveType_Pyramid, _entity, _r, _g, _b, _a);

    Material* material = ionMaterialManger().CreateMaterial("ION#Pyramid");
    _entity->GetMesh(0)->SetMaterial(material);

    LoadCommonMaterialForIntegratedPrimitive(_entity, material);
}

void RenderManager::LoadCommonMaterialForIntegratedPrimitive(Entity*& _entity, Material* _material)
{
    _material->GetBasePBR().SetBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
    _material->GetBasePBR().SetMetallicFactor(1.0f);
    _material->GetBasePBR().SetRoughnessFactor(1.0f);
    _material->GetAdvancePBR().SetEmissiveColor(1.0f, 1.0f, 1.0f);
    _material->GetAdvancePBR().SetAlphaCutoff(0.5f);


    //
    UniformBinding uniformVertex;
    uniformVertex.m_bindingIndex = 0;
    uniformVertex.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
    uniformVertex.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
    uniformVertex.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniformVertex.m_type.push_back(EBufferParameterType_Matrix);

    //
    UniformBinding uniformFragment;
    uniformFragment.m_bindingIndex = 1;
    uniformFragment.m_parameters.push_back(ION_MAIN_CAMERA_POSITION_VECTOR_PARAM);
    uniformFragment.m_type.push_back(EBufferParameterType_Vector);
    uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_DIR_VECTOR_PARAM);
    uniformFragment.m_type.push_back(EBufferParameterType_Vector);
    uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_COL_VECTOR_PARAM);
    uniformFragment.m_type.push_back(EBufferParameterType_Vector);

    //
    SamplerBinding albedoMap;
    albedoMap.m_bindingIndex = 2;
    albedoMap.m_texture = ionRenderManager().GetNullTexure();

    SamplerBinding normalMap;
    normalMap.m_bindingIndex = 3;
    normalMap.m_texture = ionRenderManager().GetNullTexure();


    // set the shaders layout
    ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniformVertex);

    ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_uniforms.push_back(uniformFragment);
    fragmentLayout.m_samplers.push_back(albedoMap);
    fragmentLayout.m_samplers.push_back(normalMap);

    //
    ConstantsBindingDef constants;
    constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
    constants.m_values.push_back(_material->GetBasePBR().GetColor()[0]);
    constants.m_values.push_back(_material->GetBasePBR().GetColor()[1]);
    constants.m_values.push_back(_material->GetBasePBR().GetColor()[2]);
    constants.m_values.push_back(_material->GetBasePBR().GetColor()[3]);
    constants.m_values.push_back(0.0f);
    constants.m_values.push_back(0.0f);
    constants.m_values.push_back(_material->GetAlphaMode() == EAlphaMode_Mask ? 1.0f : 0.0f);
    constants.m_values.push_back(_material->GetAdvancePBR().GetAlphaCutoff());

    _material->SetVertexShaderLayout(vertexLayout);
    _material->SetFragmentShaderLayout(fragmentLayout);
    _material->SetVertexLayout(_entity->GetMeshRenderer()->GetLayout());
    _material->SetConstantsShaders(constants);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_PBR_SHADER_NAME, EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_DIFFUSE_LIGHT_SHADER_NAME, EShaderStage_Fragment);

    _material->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    _material->GetState().SetCullingMode(ECullingMode_Back);
    _material->GetState().SetDepthFunctionMode(EDepthFunction_Less);
    _material->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_LesserOrEqual);
    _material->GetState().SetBlendStateMode(EBlendState_SourceBlend_One);
    _material->GetState().SetBlendStateMode(EBlendState_DestBlend_Zero);
    _material->GetState().SetBlendOperatorMode(EBlendOperator_Add);
}

void RenderManager::AddToSceneGraph(Node* _node)
{
    // A camera has the render pass and the frames buffer
    // So create them!
    if (_node->GetNodeType() == ENodeType_Camera)
    {
        Camera* camera = dynamic_cast<Camera*>(_node);
        camera->CreateRenderPassAndFrameBuffers(m_renderCore);
    }

    m_sceneGraph.AddToScene(_node);
}

void RenderManager::RemoveFromSceneGraph(Node* _node)
{
    // A camera has the render pass and the frames buffer
    // So destroy them!
    if (_node->GetNodeType() == ENodeType_Camera)
    {
        Camera* camera = dynamic_cast<Camera*>(_node);
        camera->DestroyRenderPassAndFrameBuffers(m_renderCore);
    }

    m_sceneGraph.RemoveFromScene(_node);
}

void RenderManager::RemoveAllSceneGraph()
{
    m_sceneGraph.RemoveAll(
        [&](Node* _node)
    { 
        RemoveFromSceneGraph(_node);
    }
    );
}

void RenderManager::Recreate()
{
    m_renderCore.Recreate();
    m_sceneGraph.UpdateAllCameraAspectRatio(m_renderCore);
}

void RenderManager::Resize(ionS32& _outNewWidth, ionS32 _outNewHeight)
{
    Recreate();

    _outNewWidth = m_renderCore.GetWidth();
    _outNewHeight = m_renderCore.GetHeight();
}

void RenderManager::PrepareToShutDown()
{
    m_renderCore.DestroyCommandBuffers();
}

void RenderManager::Begin()
{
    m_sceneGraph.Begin();
}

void RenderManager::End()
{
    m_sceneGraph.End();
}

void RenderManager::CoreLoop()
{
    if (m_running)
    {
        static auto lastTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (std::chrono::duration<ionFloat, std::chrono::seconds::period>(currentTime - lastTime).count() > ION_FPS_LIMIT)
        {
            currentTime = std::chrono::high_resolution_clock::now();

            m_deltaTime = std::chrono::duration<ionFloat, std::chrono::seconds::period>(currentTime - lastTime).count();

            Update(m_deltaTime);
            Frame();

            lastTime = currentTime;
        }
    }
}

void RenderManager::Update(ionFloat _deltaTime)
{
    m_sceneGraph.Update(_deltaTime);
}

void RenderManager::Frame()
{
    const ionU32 width = m_renderCore.GetWidth();
    const ionU32 height = m_renderCore.GetHeight();

    EFrameStatus endFrameStatus = EFrameStatus_Success;
    EFrameStatus startFrameStatus = m_renderCore.StartFrame();
    
    //////////////////////////////////////////////////////////////////////////
    switch(startFrameStatus)
    {
    case EFrameStatus_Success:
    {
        m_sceneGraph.Render(m_renderCore, 0, 0, width, height);
        endFrameStatus = m_renderCore.EndFrame();
    }
    break;

    case EFrameStatus_NeedUpdate:
        Recreate();
    break;

    case EFrameStatus_Error:
    default:
        ionAssertReturnVoid(false, "Start frame status error!");
        break;
    }

    //////////////////////////////////////////////////////////////////////////
    switch (endFrameStatus)
    {
    case EFrameStatus_Success:
    break;

    case EFrameStatus_NeedUpdate:
        Recreate();
    break;

    case EFrameStatus_Error:
    default:
        ionAssertReturnVoid(false, "End frame status error!");
        break;
    }
}

void RenderManager::SendMouseInput(const MouseState& _mouseState)
{
    m_sceneGraph.UpdateMouseInput(_mouseState, m_deltaTime);
}

void RenderManager::SendKeyboardInput(const KeyboardState& _keyboardState)
{
    m_sceneGraph.UpdateKeyboardInput(_keyboardState, m_deltaTime);
}

void RenderManager::RegisterToInput(Node* _node)
{
    m_sceneGraph.RegisterToInput(_node);
}

void RenderManager::UnregisterFromInput(Node* _node)
{
    m_sceneGraph.UnregisterFromInput(_node);
}

void RenderManager::Quit()
{
    m_running = false;
}

ionBool RenderManager::IsRunning()
{
    return m_running;
}

const Texture* RenderManager::GenerateBRDF(Node* _camera)
{
    Texture* brdflut = ionTextureManger().GenerateTexture(ION_BRDFLUT_TEXTURENAME, 512, 512, ETextureFormat_BRDF, ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag_Linear, ETextureRepeat_Clamp);

	Camera* cameraPtr = dynamic_cast<Camera*>(_camera);
	cameraPtr->SetPerspectiveProjection(60.0f, static_cast<ionFloat>(brdflut->GetWidth()) / static_cast<ionFloat>(brdflut->GetHeight()), 0.1f, 512.0f);

	Entity brdflutEntityObj;
	Entity* brdflutEntity = &brdflutEntityObj;

    GeneratePrimitive(EVertexLayout_Empty, EPrimitiveType_Quad, brdflutEntity);

    Material* material = ionMaterialManger().CreateMaterial(ION_BRDFLUT_TEXTURENAME);
    brdflutEntity->GetMesh(0)->SetMaterial(material);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_BRDFLUT_SHADER_NAME, EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_BRDFLUT_SHADER_NAME, EShaderStage_Fragment);

    brdflutEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(brdflutEntity->GetMeshRenderer()->GetLayout());

    brdflutEntity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    brdflutEntity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ECullingMode_TwoSide);

    VkRenderPass renderPass = m_renderCore.CreateTexturedRenderPass(brdflut, VK_IMAGE_LAYOUT_GENERAL);
    VkFramebuffer framebuffer = m_renderCore.CreateTexturedFrameBuffer(renderPass, brdflut);

    VkCommandBuffer cmdBuffer = m_renderCore.CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    ionStagingBufferManager().Submit();
    ionShaderProgramManager().StartFrame();

    DrawSurface drawSurface;
    drawSurface.m_indexStart = brdflutEntity->GetMesh(0)->GetIndexStart();
    drawSurface.m_indexCount = brdflutEntity->GetMesh(0)->GetIndexCount();
    drawSurface.m_material = brdflutEntity->GetMesh(0)->GetMaterial();
    drawSurface.m_visible = brdflutEntity->IsVisible();
    drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(brdflutEntity->GetMeshRenderer()->GetVertexData(), brdflutEntity->GetMeshRenderer()->GetVertexDataCount(), brdflutEntity->GetMeshRenderer()->GetSizeOfVertex());
    drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(brdflutEntity->GetMeshRenderer()->GetIndexData(), brdflutEntity->GetMeshRenderer()->GetIndexDataCount(), brdflutEntity->GetMeshRenderer()->GetSizeOfIndex());

    if (m_renderCore.BeginCustomCommandBuffer(cmdBuffer))
    {
        ionVector<VkClearValue, RenderCoreAllocator, RenderCore::GetAllocator> clearValues;
        clearValues.resize(1);
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

		cameraPtr->ConputeRenderAreaViewportScissor(0, 0, brdflut->GetWidth(), brdflut->GetHeight());
		cameraPtr->SetViewport(m_renderCore, cmdBuffer);
		cameraPtr->SetScissor(m_renderCore, cmdBuffer);

		cameraPtr->StartRenderPass(m_renderCore, renderPass, framebuffer, cmdBuffer, clearValues);

        drawSurface.m_projectionMatrix = cameraPtr->GetPerspectiveProjection();
        drawSurface.m_viewMatrix = cameraPtr->GetView();
        drawSurface.m_modelMatrix = brdflutEntity->GetTransform().GetMatrixWS();

        m_renderCore.SetState(drawSurface.m_material->GetState().GetStateBits());

        m_renderCore.DrawNoBinding(cmdBuffer, renderPass, drawSurface, 3, 1, 0, 0);

        cameraPtr->EndRenderPass(m_renderCore, cmdBuffer);

        m_renderCore.EndCustomCommandBuffer(cmdBuffer);
        m_renderCore.FlushCustomCommandBuffer(cmdBuffer);
    }

    m_renderCore.DestroyRenderPass(renderPass);
    m_renderCore.DestroyFrameBuffer(framebuffer);

    ionShaderProgramManager().Restart();

    return brdflut;
}

const Texture* RenderManager::GetBRDF() const
{
    return ionTextureManger().GetTexture(ION_BRDFLUT_TEXTURENAME);
}

const Texture* RenderManager::GenerateIrradianceCubemap(Node* _camera)
{
	const ionU32 mipMapsLevel = static_cast<ionU32>(std::floor(std::log2(512))) + 1;

	Texture* irradiance = ionTextureManger().GenerateTexture(ION_IRRADIANCE_TEXTURENAME, 512, 512, ETextureFormat_Irradiance, ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag_Linear, ETextureRepeat_Clamp, ETextureType_Cubic, mipMapsLevel);
	Texture* offscreen = ionTextureManger().GenerateTexture(ION_IRRADIANCE_TEXTURENAME_OFFSCREEN, 512, 512, ETextureFormat_Irradiance, ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag_Linear, ETextureRepeat_Clamp, ETextureType_2D);

	//
	// Transition between irradiance and offscreen
	VkRenderPass renderPass = m_renderCore.CreateTexturedRenderPass(irradiance, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkFramebuffer framebuffer = m_renderCore.CreateTexturedFrameBuffer(renderPass, offscreen);  // frame buffer on the offscreen

	VkCommandBuffer layoutCmd = m_renderCore.CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	if (m_renderCore.BeginCustomCommandBuffer(layoutCmd))
	{
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.image = offscreen->GetImage();
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCmdPipelineBarrier(layoutCmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		m_renderCore.EndCustomCommandBuffer(layoutCmd);
		m_renderCore.FlushCustomCommandBuffer(layoutCmd);
	}

	//
    // generation of the entity and camera render
    Camera* cameraPtr = dynamic_cast<Camera*>(_camera);
	cameraPtr->SetPerspectiveProjection(60.0f, static_cast<ionFloat>(irradiance->GetWidth()) / static_cast<ionFloat>(irradiance->GetHeight()), 0.1f, 512.0f);

    Entity* irradianceEntity = CreateNode(Entity, "Irradiance");
	irradianceEntity->AttachToParent(_camera);

    // shader has position input
    GeneratePrimitive(EVertexLayout_Pos, EPrimitiveType_Quad, irradianceEntity);


	Material* material = ionMaterialManger().CreateMaterial(ION_IRRADIANCE_TEXTURENAME);
	irradianceEntity->GetMesh(0)->SetMaterial(material);

	//
	UniformBinding uniform;
	uniform.m_bindingIndex = 0;
	uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
	uniform.m_type.push_back(EBufferParameterType_Matrix);
	uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
	uniform.m_type.push_back(EBufferParameterType_Matrix);
	uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
	uniform.m_type.push_back(EBufferParameterType_Matrix);

	//
	SamplerBinding sampler;
	sampler.m_bindingIndex = 1;
	sampler.m_texture = cameraPtr->GetSkybox()->GetMaterial()->GetBasePBR().GetBaseColorTexture();

	//
	ConstantsBindingDef constants;
	constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
	constants.m_values.push_back((2.0f * NIX_PI) / 180.0f);
	constants.m_values.push_back((0.5f * NIX_PI) / 64.0f);


	ShaderLayoutDef vertexLayout;
	vertexLayout.m_uniforms.push_back(uniform);

	ShaderLayoutDef fragmentLayout;
	fragmentLayout.m_samplers.push_back(sampler);

	irradianceEntity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);
	irradianceEntity->GetMesh(0)->GetMaterial()->SetFragmentShaderLayout(fragmentLayout);

	ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_IRRADIANCE_PREFILTERED_VERTEX_SHADER_NAME, EShaderStage_Vertex);
	ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_IRRADIANCE_FRAGMENT_SHADER_NAME, EShaderStage_Fragment);

	irradianceEntity->GetMesh(0)->GetMaterial()->SetConstantsShaders(constants);
	irradianceEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(irradianceEntity->GetMeshRenderer()->GetLayout());

	irradianceEntity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

	irradianceEntity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ECullingMode_TwoSide);
	irradianceEntity->GetMesh(0)->GetMaterial()->GetState().SetColorMaskMode(EColorMask_Depth);
	irradianceEntity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(EDepthFunction_Less);
	irradianceEntity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_Always);

	//
	// render phase
	VkCommandBuffer cmdBuffer = m_renderCore.CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	ionStagingBufferManager().Submit();
	ionShaderProgramManager().StartFrame();

	DrawSurface drawSurface;
	drawSurface.m_indexStart = irradianceEntity->GetMesh(0)->GetIndexStart();
	drawSurface.m_indexCount = irradianceEntity->GetMesh(0)->GetIndexCount();
	drawSurface.m_material = irradianceEntity->GetMesh(0)->GetMaterial();
	drawSurface.m_visible = irradianceEntity->IsVisible();
	drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(irradianceEntity->GetMeshRenderer()->GetVertexData(), irradianceEntity->GetMeshRenderer()->GetVertexDataCount(), irradianceEntity->GetMeshRenderer()->GetSizeOfVertex());
	drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(irradianceEntity->GetMeshRenderer()->GetIndexData(), irradianceEntity->GetMeshRenderer()->GetIndexDataCount(), irradianceEntity->GetMeshRenderer()->GetSizeOfIndex());

	if (m_renderCore.BeginCustomCommandBuffer(cmdBuffer))
	{
		ionVector<VkClearValue, RenderCoreAllocator, RenderCore::GetAllocator> clearValues;
		clearValues.resize(1);
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };


		// Swap
		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = mipMapsLevel;
		subresourceRange.layerCount = 6;

		{
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.image = irradiance->GetImage();
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.subresourceRange = subresourceRange;
			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
		}

		ionVector<Quaternion, RenderManagerAllocator, GetAllocator> rotations;
		rotations.push_back(Quaternion(0.0f, 0.0f, NIX_DEG_TO_RAD(-90.0f)));
		rotations.push_back(Quaternion(0.0f, 0.0f, NIX_DEG_TO_RAD(90.0f)));
		rotations.push_back(Quaternion(NIX_DEG_TO_RAD(-90.0f), 0.0f, 0.0f));
		rotations.push_back(Quaternion(NIX_DEG_TO_RAD(90.0f), 0.0f, 0.0f));
		rotations.push_back(Quaternion(0.0f, 0.0f, 0.0f));
		rotations.push_back(Quaternion(0.0f, 0.0f, NIX_DEG_TO_RAD(-180.0f)));

		for (ionU32 m = 0; m < mipMapsLevel; ++m)
		{
			for (ionU32 f = 0; f < 6; ++f)
			{
				cameraPtr->ConputeRenderAreaViewportScissor(0, 0, static_cast<ionS32>(irradiance->GetWidth() * std::powf(0.5f, static_cast<ionFloat>(m))), static_cast<ionS32>(irradiance->GetHeight() * std::powf(0.5f, static_cast<ionFloat>(m))));
				cameraPtr->SetViewport(m_renderCore, cmdBuffer);
				cameraPtr->SetScissor(m_renderCore, cmdBuffer);

				cameraPtr->StartRenderPass(m_renderCore, renderPass, framebuffer, cmdBuffer, clearValues);

				cameraPtr->GetTransform().SetRotation(rotations[f]);
				cameraPtr->Update(0.0f);
				cameraPtr->UpdateView();

				// draw irradiance
				{
					drawSurface.m_projectionMatrix = cameraPtr->GetPerspectiveProjection();
					drawSurface.m_viewMatrix = cameraPtr->GetView();
					drawSurface.m_modelMatrix = irradianceEntity->GetTransform().GetMatrixWS();

					m_renderCore.SetState(drawSurface.m_material->GetState().GetStateBits());
					m_renderCore.Draw(cmdBuffer, renderPass, drawSurface);
				}

				cameraPtr->CustomRenderSkybox(m_renderCore, cmdBuffer, renderPass);

				cameraPtr->EndRenderPass(m_renderCore, cmdBuffer);



				VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseMipLevel = 0;
				subresourceRange.levelCount = mipMapsLevel;
				subresourceRange.layerCount = 6;

				{
					VkImageMemoryBarrier imageMemoryBarrier{};
					imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					imageMemoryBarrier.image = offscreen->GetImage();
					imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
					vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
				}

				// Copy region for transfer from framebuffer to cube face
				VkImageCopy copyRegion{};

				copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.srcSubresource.baseArrayLayer = 0;
				copyRegion.srcSubresource.mipLevel = 0;
				copyRegion.srcSubresource.layerCount = 1;
				copyRegion.srcOffset = { 0, 0, 0 };

				copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.dstSubresource.baseArrayLayer = f;
				copyRegion.dstSubresource.mipLevel = m;
				copyRegion.dstSubresource.layerCount = 1;
				copyRegion.dstOffset = { 0, 0, 0 };

				copyRegion.extent.width = static_cast<ionU32>(irradiance->GetWidth()* std::powf(0.5f, static_cast<ionFloat>(m)));
				copyRegion.extent.height = static_cast<ionU32>(irradiance->GetHeight()* std::powf(0.5f, static_cast<ionFloat>(m)));
				copyRegion.extent.depth = 1;

				vkCmdCopyImage(
					cmdBuffer,
					offscreen->GetImage(),
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					irradiance->GetImage(),
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&copyRegion);

				{
					VkImageMemoryBarrier imageMemoryBarrier{};
					imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					imageMemoryBarrier.image = offscreen->GetImage();
					imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
					vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
				}
			}
		}


		// Swap back
		{
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.image = irradiance->GetImage();
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.subresourceRange = subresourceRange;
			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
		}


		m_renderCore.EndCustomCommandBuffer(cmdBuffer);
		m_renderCore.FlushCustomCommandBuffer(cmdBuffer);
	}

	m_renderCore.DestroyRenderPass(renderPass);
	m_renderCore.DestroyFrameBuffer(framebuffer);

	ionTextureManger().DestroyTexture(ION_IRRADIANCE_TEXTURENAME_OFFSCREEN);

	irradianceEntity->DetachFromParent();

	DestroyNode(irradianceEntity);

	ionShaderProgramManager().Restart();

	return irradiance;
}

const Texture* RenderManager::GetIrradianceCubemap() const
{
    return ionTextureManger().GetTexture(ION_IRRADIANCE_TEXTURENAME);
}

const Texture* RenderManager::GeneratePrefilteredEnvironmentCubemap(Node* _camera)
{
    const ionU32 mipMapsLevel = static_cast<ionU32>(std::floor(std::log2(1024))) + 1;

    Texture* prefilteredEnvironment = ionTextureManger().GenerateTexture(ION_PREFILTEREDENVIRONMENT_TEXTURENAME, 1024, 1024, ETextureFormat_PrefilteredEnvironment, ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag_Linear, ETextureRepeat_Clamp, ETextureType_Cubic, mipMapsLevel);
    Texture* offscreen = ionTextureManger().GenerateTexture(ION_PREFILTEREDENVIRONMENT_TEXTURENAME_OFFSCREEN, 1024, 1024, ETextureFormat_PrefilteredEnvironment, ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag_Linear, ETextureRepeat_Clamp, ETextureType_2D);

    //
    // Transition between prefilteredEnvironment and offscreen
    VkRenderPass renderPass = m_renderCore.CreateTexturedRenderPass(prefilteredEnvironment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkFramebuffer framebuffer = m_renderCore.CreateTexturedFrameBuffer(renderPass, offscreen);  // frame buffer on the offscreen

    VkCommandBuffer layoutCmd = m_renderCore.CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    if (m_renderCore.BeginCustomCommandBuffer(layoutCmd))
    {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.image = offscreen->GetImage();
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(layoutCmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

        m_renderCore.EndCustomCommandBuffer(layoutCmd);
        m_renderCore.FlushCustomCommandBuffer(layoutCmd);
    }

	//
	// generation of the entity and camera render
	Camera* cameraPtr = dynamic_cast<Camera*>(_camera);
	cameraPtr->SetPerspectiveProjection(60.0f, static_cast<ionFloat>(prefilteredEnvironment->GetWidth()) / static_cast<ionFloat>(prefilteredEnvironment->GetHeight()), 0.1f, 1024.0f);

	Entity* prefilteredEntity = CreateNode(Entity, "Prefiltered");

	prefilteredEntity->AttachToParent(_camera);

	// shader has position input
	GeneratePrimitive(EVertexLayout_Pos, EPrimitiveType_Quad, prefilteredEntity);

    Material* material = ionMaterialManger().CreateMaterial(ION_IRRADIANCE_TEXTURENAME);
    prefilteredEntity->GetMesh(0)->SetMaterial(material);

    //
    UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(EBufferParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(EBufferParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(EBufferParameterType_Matrix);

    //
    UniformBinding uniformFragment;
    uniformFragment.m_bindingIndex = 1;
    uniformFragment.m_parameters.push_back("roughness");
    uniformFragment.m_type.push_back(EBufferParameterType_Float);
   
    SamplerBinding sampler;
    sampler.m_bindingIndex = 2;
    sampler.m_texture = cameraPtr->GetSkybox()->GetMaterial()->GetBasePBR().GetBaseColorTexture();

    //
    ConstantsBindingDef constants;
    constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
    constants.m_values.push_back(m_prefilteredCubeMipLevels);


    ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_uniforms.push_back(uniformFragment);
    fragmentLayout.m_samplers.push_back(sampler);

    prefilteredEntity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);
    prefilteredEntity->GetMesh(0)->GetMaterial()->SetFragmentShaderLayout(fragmentLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_IRRADIANCE_PREFILTERED_VERTEX_SHADER_NAME, EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_PREFILTEREDENVIRONMENT_FRAGMENT_SHADER_NAME, EShaderStage_Fragment);

    prefilteredEntity->GetMesh(0)->GetMaterial()->SetConstantsShaders(constants);
    prefilteredEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(prefilteredEntity->GetMeshRenderer()->GetLayout());

    prefilteredEntity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    prefilteredEntity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ECullingMode_TwoSide);
    prefilteredEntity->GetMesh(0)->GetMaterial()->GetState().SetColorMaskMode(EColorMask_Depth);
    prefilteredEntity->GetMesh(0)->GetMaterial()->GetState().SetDepthFunctionMode(EDepthFunction_Less);
    prefilteredEntity->GetMesh(0)->GetMaterial()->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_Always);

    //
    // render phase
    VkCommandBuffer cmdBuffer = m_renderCore.CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    ionStagingBufferManager().Submit();
    ionShaderProgramManager().StartFrame();


    DrawSurface drawSurface;
    drawSurface.m_indexStart = prefilteredEntity->GetMesh(0)->GetIndexStart();
    drawSurface.m_indexCount = prefilteredEntity->GetMesh(0)->GetIndexCount();
    drawSurface.m_material = prefilteredEntity->GetMesh(0)->GetMaterial();
    drawSurface.m_visible = prefilteredEntity->IsVisible();
    drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(prefilteredEntity->GetMeshRenderer()->GetVertexData(), prefilteredEntity->GetMeshRenderer()->GetVertexDataCount(), prefilteredEntity->GetMeshRenderer()->GetSizeOfVertex());
    drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(prefilteredEntity->GetMeshRenderer()->GetIndexData(), prefilteredEntity->GetMeshRenderer()->GetIndexDataCount(), prefilteredEntity->GetMeshRenderer()->GetSizeOfIndex());

    if (m_renderCore.BeginCustomCommandBuffer(cmdBuffer))
    {
        ionVector<VkClearValue, RenderCoreAllocator, RenderCore::GetAllocator> clearValues;
        clearValues.resize(1);
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };


        // Swap
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = mipMapsLevel;
        subresourceRange.layerCount = 6;

        {
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = prefilteredEnvironment->GetImage();
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = subresourceRange;
            vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
        }

		ionVector<Quaternion, RenderManagerAllocator, GetAllocator> rotations;
		rotations.push_back(Quaternion(0.0f, 0.0f, NIX_DEG_TO_RAD(-90.0f)));
		rotations.push_back(Quaternion(0.0f, 0.0f, NIX_DEG_TO_RAD(90.0f)));
		rotations.push_back(Quaternion(NIX_DEG_TO_RAD(-90.0f), 0.0f, 0.0f));
		rotations.push_back(Quaternion(NIX_DEG_TO_RAD(90.0f), 0.0f, 0.0f));
		rotations.push_back(Quaternion(0.0f, 0.0f, 0.0f));
		rotations.push_back(Quaternion(0.0f, 0.0f, NIX_DEG_TO_RAD(-180.0f)));

        for (ionU32 m = 0; m < mipMapsLevel; ++m)
        {
            for (ionU32 f = 0; f < 6; ++f)
			{
				cameraPtr->ConputeRenderAreaViewportScissor(0, 0, static_cast<ionS32>(prefilteredEnvironment->GetWidth() * std::powf(0.5f, static_cast<ionFloat>(m))), static_cast<ionS32>(prefilteredEnvironment->GetHeight() * std::powf(0.5f, static_cast<ionFloat>(m))));
				cameraPtr->SetViewport(m_renderCore, cmdBuffer);
				cameraPtr->SetScissor(m_renderCore, cmdBuffer);

				cameraPtr->StartRenderPass(m_renderCore, renderPass, framebuffer, cmdBuffer, clearValues);

				cameraPtr->GetTransform().SetRotation(rotations[f]);
				cameraPtr->Update(0.0f);
                cameraPtr->UpdateView();

                // draw prefilteredEnvironment
                {
                    // custom draw uniform
                    ionShaderProgramManager().SetRenderParamFloat("roughness", (ionFloat)m / (ionFloat)(mipMapsLevel - 1));

                    drawSurface.m_projectionMatrix = cameraPtr->GetPerspectiveProjection();
                    drawSurface.m_viewMatrix = cameraPtr->GetView();
                    drawSurface.m_modelMatrix = prefilteredEntity->GetTransform().GetMatrixWS();

                    m_renderCore.SetState(drawSurface.m_material->GetState().GetStateBits());
                    m_renderCore.Draw(cmdBuffer, renderPass, drawSurface);
                }

                cameraPtr->CustomRenderSkybox(m_renderCore, cmdBuffer, renderPass);

                cameraPtr->EndRenderPass(m_renderCore, cmdBuffer);



                VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                subresourceRange.baseMipLevel = 0;
                subresourceRange.levelCount = mipMapsLevel;
                subresourceRange.layerCount = 6;

                {
                    VkImageMemoryBarrier imageMemoryBarrier{};
                    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    imageMemoryBarrier.image = offscreen->GetImage();
                    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
                }

                // Copy region for transfer from framebuffer to cube face
                VkImageCopy copyRegion{};

                copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                copyRegion.srcSubresource.baseArrayLayer = 0;
                copyRegion.srcSubresource.mipLevel = 0;
                copyRegion.srcSubresource.layerCount = 1;
                copyRegion.srcOffset = { 0, 0, 0 };

                copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                copyRegion.dstSubresource.baseArrayLayer = f;
                copyRegion.dstSubresource.mipLevel = m;
                copyRegion.dstSubresource.layerCount = 1;
                copyRegion.dstOffset = { 0, 0, 0 };

                copyRegion.extent.width = static_cast<ionU32>(prefilteredEnvironment->GetWidth()* std::powf(0.5f, static_cast<ionFloat>(m)));
                copyRegion.extent.height = static_cast<ionU32>(prefilteredEnvironment->GetHeight()* std::powf(0.5f, static_cast<ionFloat>(m)));
                copyRegion.extent.depth = 1;

                vkCmdCopyImage(
                    cmdBuffer,
                    offscreen->GetImage(),
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    prefilteredEnvironment->GetImage(),
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1,
                    &copyRegion);

                {
                    VkImageMemoryBarrier imageMemoryBarrier{};
                    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    imageMemoryBarrier.image = offscreen->GetImage();
                    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
                    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
                }
            }
        }


        // Swap back
        {
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.image = prefilteredEnvironment->GetImage();
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.subresourceRange = subresourceRange;
            vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
        }


        m_renderCore.EndCustomCommandBuffer(cmdBuffer);
        m_renderCore.FlushCustomCommandBuffer(cmdBuffer);
    }

    m_renderCore.DestroyRenderPass(renderPass);
    m_renderCore.DestroyFrameBuffer(framebuffer);

    ionTextureManger().DestroyTexture(ION_PREFILTEREDENVIRONMENT_TEXTURENAME_OFFSCREEN);

    prefilteredEntity->DetachFromParent();

	DestroyNode(prefilteredEntity);

    ionShaderProgramManager().Restart();

    return prefilteredEnvironment;
}

const Texture* RenderManager::GetPrefilteredEnvironmentCubemap() const
{
    return ionTextureManger().GetTexture(ION_PREFILTEREDENVIRONMENT_TEXTURENAME);
}

const Texture* RenderManager::GenerateNullTexture()
{
    return ionTextureManger().GenerateTexture(ION_NULL_TEXTURENAME, 1, 1, ETextureFormat_RGBA8, ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag_Linear, ETextureRepeat_Repeat, ETextureType_2D);
}

const Texture* RenderManager::GetNullTexure() const
{
    return ionTextureManger().GetTexture(ION_NULL_TEXTURENAME);
}

void RenderManager::AddDirectionalLight()
{
    m_sceneGraph.CreateDirectionalLightToScene();
}

void RenderManager::RemoveDirectionalLight()
{
    m_sceneGraph.DestroyDirectionalLightToScene();
}

DirectionalLight* RenderManager::GetDirectionalLight()
{
    return m_sceneGraph.GetDirectionalLight();
}

Node* RenderManager::GetObjectByName(const ionString& _name)
{
    return m_sceneGraph.GetObjectByName(_name);
}

Node* RenderManager::GetObjectByUUID(const UUID& _uuid)
{
    return m_sceneGraph.GetObjectByUUID(_uuid);
}

VkCommandBuffer RenderManager::InstantiateCommandBuffer(VkCommandBufferLevel _level)
{
    VkCommandBuffer cmdBuffer = m_renderCore.CreateCustomCommandBuffer(_level);
    m_renderCore.BeginCustomCommandBuffer(cmdBuffer);
    return cmdBuffer;
}

void RenderManager::ShutdownCommandBuffer(VkCommandBuffer _commandBuffer)
{
    m_renderCore.EndCustomCommandBuffer(_commandBuffer);
    m_renderCore.FlushCustomCommandBuffer(_commandBuffer);
}

ION_NAMESPACE_END