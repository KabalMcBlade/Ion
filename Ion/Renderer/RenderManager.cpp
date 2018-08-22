#include "RenderManager.h"

#include "VertexCacheManager.h"
#include "StagingBufferManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"
#include "../Scene/DirectionalLight.h"

#include "../Geometry/Mesh.h"
#include "../Geometry/PrimitiveFactory.h"

#include "../Texture/TextureManager.h"

#include "../Core/FileSystemManager.h"

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

RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager() : m_deltaTime(0.0f), m_running(false)
{
#ifdef ION_PBR_DEBUG
    m_exposure = 4.5f;
    m_gamma = 2.2f;
    m_prefilteredCubeMipLevels = 10.0f;
#endif // ION_PBR_DEBUG
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
    m_sceneGraph.RemoveAll();
    m_renderCore.Shutdown();
}

void RenderManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(RenderManager, ION_MEMORY_ALIGNMENT_SIZE);
    }
}

void RenderManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

RenderManager& RenderManager::Instance()
{
    return *s_instance;
}

ionBool RenderManager::LoadModelFromFile(const eosString& _fileName, ObjectHandler& _entity, ionBool _generateNormalWhenMissing /*= false*/, ionBool _generateTangentWhenMissing /*= false*/, ionBool _setBitangentSign /*= false*/)
{
    return m_loader.Load(_fileName, _entity, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);
}

void RenderManager::LoadPrimitive(EVertexLayout _layout, EPrimitiveType _type, ObjectHandler& _entity)
{
    switch(_type)
    {
    case EPrimitiveType_Triangle:
        PrimitiveFactory::GenerateTriangle(_layout, _entity);
        break;
    case EPrimitiveType_Quad:
        PrimitiveFactory::GenerateQuad(_layout, _entity);
        break;
    case EPrimitiveType_Cube:
        PrimitiveFactory::GenerateCube(_layout, _entity);
        break;
    case EPrimitiveType_Sphere:
        PrimitiveFactory::GenerateSphere(_layout, _entity);
        break;
    }
}

void RenderManager::AddToSceneGraph(ObjectHandler _node)
{
    m_sceneGraph.AddToScene(_node);
}

void RenderManager::Resize(ionS32& _outNewWidth, ionS32 _outNewHeight)
{
    m_renderCore.Recreate();
    m_sceneGraph.UpdateAllCameraAspectRatio(m_renderCore);


    _outNewWidth = m_renderCore.GetWidth();
    _outNewHeight = m_renderCore.GetHeight();
}

void RenderManager::Prepare()
{
    m_sceneGraph.Prepare();
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

    if (m_renderCore.StartFrame())
    {
        m_sceneGraph.Render(m_renderCore, 0, 0, width, height);

        m_renderCore.EndFrame();
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

void RenderManager::RegisterToInput(const ObjectHandler& _node)
{
    m_sceneGraph.RegisterToInput(_node);
}

void RenderManager::UnregisterFromInput(const ObjectHandler& _node)
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

Texture* RenderManager::GenerateBRDF(ObjectHandler _camera)
{
    Texture* brdflut = ionTextureManger().GenerateTexture(ION_BRDFLUT_TEXTURENAME, 512, 512, ETextureFormat_BRDF, ETextureFilter_Default, ETextureRepeat_Clamp);

    Camera* cameraPtr = dynamic_cast<Camera*>(_camera.GetPtr());

    cameraPtr->SetPerspectiveProjection(60.0f, static_cast<ionFloat>(brdflut->GetWidth()) / static_cast<ionFloat>(brdflut->GetHeight()), 0.1f, 256.0f);

    Entity* brdflutEntity = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE);
    ObjectHandler brdflutEntityHandle(brdflutEntity);

    LoadPrimitive(EVertexLayout_Empty, EPrimitiveType_Quad, brdflutEntityHandle);

    Material* material = ionMaterialManger().CreateMaterial(ION_BRDFLUT_TEXTURENAME, 0u);
    brdflutEntity->GetMesh(0)->SetMaterial(material);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_BRDFLUT_SHADER_NAME, EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_BRDFLUT_SHADER_NAME, EShaderStage_Fragment);

    brdflutEntity->GetMesh(0)->GetMaterial()->SetShaderProgramName(ION_BRDFLUT_SHADER_NAME);
    brdflutEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(brdflutEntity->GetMesh(0)->GetLayout());

    brdflutEntity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    brdflutEntity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ECullingMode_TwoSide);

    VkRenderPass renderPass = m_renderCore.CreateTexturedRenderPass(brdflut, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    VkFramebuffer framebuffer = m_renderCore.CreateTexturedFrameBuffer(renderPass, brdflut);

    VkCommandBuffer cmdBuffer = m_renderCore.CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    ionStagingBufferManager().Submit();
    ionShaderProgramManager().StartFrame();

    DrawSurface drawSurface;
    drawSurface.m_visible = brdflutEntity->IsVisible();
    drawSurface.m_indexStart = brdflutEntity->GetMesh(0)->GetIndexStart();
    drawSurface.m_indexCount = brdflutEntity->GetMesh(0)->GetIndexCount();
    drawSurface.m_material = brdflutEntity->GetMesh(0)->GetMaterial();

    if (m_renderCore.BeginCustomCommandBuffer(cmdBuffer))
    {
        eosVector(VkClearValue) clearValues;
        clearValues.resize(1);
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

        cameraPtr->StartRenderPass(m_renderCore, renderPass, framebuffer, cmdBuffer, clearValues, static_cast<ionU32>(brdflut->GetWidth()), static_cast<ionU32>(brdflut->GetHeight()));
        
        cameraPtr->SetViewport(m_renderCore, cmdBuffer, 0, 0, brdflut->GetWidth(), brdflut->GetHeight());
        cameraPtr->SetScissor(m_renderCore, cmdBuffer, 0, 0, brdflut->GetWidth(), brdflut->GetHeight());

        const Matrix& projection = cameraPtr->GetPerspectiveProjection();
        const Matrix& view = cameraPtr->GetView();

        const Matrix& model = brdflutEntity->GetTransform().GetMatrixWS();

        _mm_storeu_ps(&drawSurface.m_modelMatrix[0], model[0]);
        _mm_storeu_ps(&drawSurface.m_modelMatrix[4], model[1]);
        _mm_storeu_ps(&drawSurface.m_modelMatrix[8], model[2]);
        _mm_storeu_ps(&drawSurface.m_modelMatrix[12], model[3]);

        _mm_storeu_ps(&drawSurface.m_viewMatrix[0], view[0]);
        _mm_storeu_ps(&drawSurface.m_viewMatrix[4], view[1]);
        _mm_storeu_ps(&drawSurface.m_viewMatrix[8], view[2]);
        _mm_storeu_ps(&drawSurface.m_viewMatrix[12], view[3]);

        _mm_storeu_ps(&drawSurface.m_projectionMatrix[0], projection[0]);
        _mm_storeu_ps(&drawSurface.m_projectionMatrix[4], projection[1]);
        _mm_storeu_ps(&drawSurface.m_projectionMatrix[8], projection[2]);
        _mm_storeu_ps(&drawSurface.m_projectionMatrix[12], projection[3]);

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

Texture* RenderManager::GetBRDF()
{
    return ionTextureManger().GetTexture(ION_BRDFLUT_TEXTURENAME);
}

Texture* RenderManager::GenerateIrradianceCubemap(const Texture* _environmentCubeMap, ObjectHandler _camera)
{
    const ionU32 mipMapsLevel = static_cast<ionU32>(std::floor(std::log2(64))) + 1;

    Texture* irradiance = ionTextureManger().GenerateTexture(ION_IRRADIANCE_TEXTURENAME, 64, 64, ETextureFormat_Irradiance, ETextureFilter_Default, ETextureRepeat_Clamp, ETextureType_Cubic, mipMapsLevel);
    Texture* offscreen = ionTextureManger().GenerateTexture(ION_IRRADIANCE_TEXTURENAME_OFFSCREEN, 64, 64, ETextureFormat_Irradiance, ETextureFilter_NoSampler, ETextureRepeat_NoSampler, ETextureType_2D);

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
    Camera* cameraPtr = dynamic_cast<Camera*>(_camera.GetPtr());

    cameraPtr->SetPerspectiveProjection(60.0f, static_cast<ionFloat>(irradiance->GetWidth()) / static_cast<ionFloat>(irradiance->GetHeight()), 0.1f, 256.0f);

    Entity* irradianceEntity = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE);
    ObjectHandler irradianceEntityHandle(irradianceEntity);

    irradianceEntityHandle->AttachToParent(_camera);

    // shader has position input
    LoadPrimitive(EVertexLayout_Pos, EPrimitiveType_Quad, irradianceEntityHandle);

    Material* material = ionMaterialManger().CreateMaterial(ION_IRRADIANCE_TEXTURENAME, 0u);
    irradianceEntity->GetMesh(0)->SetMaterial(material);

    //
    UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(EUniformParameterType_Matrix);

    //
    SamplerBinding sampler;
    sampler.m_bindingIndex = 1;
    sampler.m_texture = _environmentCubeMap;

    //
    ConstantsBindingDef constants;
    constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
    constants.m_values.push_back((2.0f * MathHelper::kPI) / 180.0f);
    constants.m_values.push_back((0.5f * MathHelper::kPI) / 64.0f);


    ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    irradianceEntity->GetMesh(0)->GetMaterial()->SetVertexShaderLayout(vertexLayout);
    irradianceEntity->GetMesh(0)->GetMaterial()->SetFragmentShaderLayout(fragmentLayout);

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_IRRADIANCE_PREFILTERED_VERTEX_SHADER_NAME, EShaderStage_Vertex);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_IRRADIANCE_FRAGMENT_SHADER_NAME, EShaderStage_Fragment);

    irradianceEntity->GetMesh(0)->GetMaterial()->SetConstantsShaders(constants);
    irradianceEntity->GetMesh(0)->GetMaterial()->SetShaderProgramName(ION_IRRADIANCE_FRAGMENT_SHADER_NAME);
    irradianceEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(irradianceEntity->GetMesh(0)->GetLayout());

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
    drawSurface.m_visible = irradianceEntity->IsVisible();
    drawSurface.m_indexStart = irradianceEntity->GetMesh(0)->GetIndexStart();
    drawSurface.m_indexCount = irradianceEntity->GetMesh(0)->GetIndexCount();
    drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(irradianceEntity->GetMesh(0)->GetVertexData(), irradianceEntity->GetMesh(0)->GetVertexSize(), irradianceEntity->GetMesh(0)->GetSize());
    drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(irradianceEntity->GetMesh(0)->GetIndexData(), irradianceEntity->GetMesh(0)->GetIndexSize());
    drawSurface.m_material = irradianceEntity->GetMesh(0)->GetMaterial();


    if (m_renderCore.BeginCustomCommandBuffer(cmdBuffer))
    {
        eosVector(VkClearValue) clearValues;
        clearValues.resize(1);
        clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 0.0f } };

        cameraPtr->SetViewport(m_renderCore, cmdBuffer, 0, 0, irradiance->GetWidth(), irradiance->GetHeight());
        cameraPtr->SetScissor(m_renderCore, cmdBuffer, 0, 0, irradiance->GetWidth(), irradiance->GetHeight());


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

        // TODO: find the proper rotation order!
        eosVector(Quaternion) rotations;
        rotations.push_back(Quaternion(0.0f, -0.707f, 0.0f, 0.707f));    // right in camera poi
        rotations.push_back(Quaternion(0.0f, 0.707f, 0.0f, 0.707f));     // left in camera poi
        rotations.push_back(Quaternion(-0.707f, 0.0f, 0.0f, 0.707f));    // up in camera poi
        rotations.push_back(Quaternion(0.707f, 0.0f, 0.0f, 0.707f));    // down in camera poi
        rotations.push_back(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));    // rear in camera poi
        rotations.push_back(Quaternion());                              // front in camera poi

        for (ionU32 m = 0; m < mipMapsLevel; ++m)
        {
            for (ionU32 f = 0; f < 6; ++f)
            {
                cameraPtr->SetViewport(m_renderCore, cmdBuffer, 0, 0, static_cast<ionS32>(irradiance->GetWidth() * std::powf(0.5f, static_cast<ionFloat>(m))), static_cast<ionS32>(irradiance->GetHeight() * std::powf(0.5f, static_cast<ionFloat>(m))));

                cameraPtr->StartRenderPass(m_renderCore, renderPass, framebuffer, cmdBuffer, clearValues, static_cast<ionU32>(irradiance->GetWidth()), static_cast<ionU32>(irradiance->GetHeight()));

                // rotate the camera here
                cameraPtr->GetTransform().SetRotation(rotations[f]);
                //cameraPtr->Update(0.0f);
                cameraPtr->UpdateView();

                // draw irradiance
                {
                    const Matrix& projection = cameraPtr->GetPerspectiveProjection();
                    const Matrix& view = cameraPtr->GetView();
                    const Matrix& model = irradianceEntity->GetTransform().GetMatrixWS();

                    _mm_storeu_ps(&drawSurface.m_modelMatrix[0], model[0]);
                    _mm_storeu_ps(&drawSurface.m_modelMatrix[4], model[1]);
                    _mm_storeu_ps(&drawSurface.m_modelMatrix[8], model[2]);
                    _mm_storeu_ps(&drawSurface.m_modelMatrix[12], model[3]);

                    _mm_storeu_ps(&drawSurface.m_viewMatrix[0], view[0]);
                    _mm_storeu_ps(&drawSurface.m_viewMatrix[4], view[1]);
                    _mm_storeu_ps(&drawSurface.m_viewMatrix[8], view[2]);
                    _mm_storeu_ps(&drawSurface.m_viewMatrix[12], view[3]);

                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[0], projection[0]);
                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[4], projection[1]);
                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[8], projection[2]);
                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[12], projection[3]);

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
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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

    irradianceEntityHandle->DetachFromParent();

    ionShaderProgramManager().Restart();

    return irradiance;
}

Texture* RenderManager::GetIrradianceCubemap()
{
    return ionTextureManger().GetTexture(ION_IRRADIANCE_TEXTURENAME);
}

Texture* RenderManager::GeneratePrefilteredEnvironmentCubemap(const Texture* _environmentCubeMap, ObjectHandler _camera)
{
    const ionU32 mipMapsLevel = static_cast<ionU32>(std::floor(std::log2(512))) + 1;

    Texture* prefilteredEnvironment = ionTextureManger().GenerateTexture(ION_PREFILTEREDENVIRONMENT_TEXTURENAME, 512, 512, ETextureFormat_PrefilteredEnvironment, ETextureFilter_Default, ETextureRepeat_Clamp, ETextureType_Cubic, mipMapsLevel);
    Texture* offscreen = ionTextureManger().GenerateTexture(ION_PREFILTEREDENVIRONMENT_TEXTURENAME_OFFSCREEN, 512, 512, ETextureFormat_PrefilteredEnvironment, ETextureFilter_NoSampler, ETextureRepeat_NoSampler, ETextureType_2D);

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
    Camera* cameraPtr = dynamic_cast<Camera*>(_camera.GetPtr());

    cameraPtr->SetPerspectiveProjection(60.0f, static_cast<ionFloat>(prefilteredEnvironment->GetWidth()) / static_cast<ionFloat>(prefilteredEnvironment->GetHeight()), 0.1f, 256.0f);

    Entity* prefilteredEntity = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE);
    ObjectHandler prefilteredEntityHandle(prefilteredEntity);

    prefilteredEntityHandle->AttachToParent(_camera);

    // shader has position input
    LoadPrimitive(EVertexLayout_Pos, EPrimitiveType_Quad, prefilteredEntityHandle);

    Material* material = ionMaterialManger().CreateMaterial(ION_IRRADIANCE_TEXTURENAME, 0u);
    prefilteredEntity->GetMesh(0)->SetMaterial(material);

    //
    UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(EUniformParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(EUniformParameterType_Matrix);

    //
    UniformBinding uniformFragment;
    uniformFragment.m_bindingIndex = 1;
    uniformFragment.m_parameters.push_back("roughness");
    uniformFragment.m_type.push_back(EUniformParameterType_Float);
   
    SamplerBinding sampler;
    sampler.m_bindingIndex = 2;
    sampler.m_texture = _environmentCubeMap;

    //
    ConstantsBindingDef constants;
    constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
    constants.m_values.push_back(32.0f);


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
    prefilteredEntity->GetMesh(0)->GetMaterial()->SetShaderProgramName(ION_PREFILTEREDENVIRONMENT_FRAGMENT_SHADER_NAME);
    prefilteredEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(prefilteredEntity->GetMesh(0)->GetLayout());

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
    drawSurface.m_visible = prefilteredEntity->IsVisible();
    drawSurface.m_indexStart = prefilteredEntity->GetMesh(0)->GetIndexStart();
    drawSurface.m_indexCount = prefilteredEntity->GetMesh(0)->GetIndexCount();
    drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(prefilteredEntity->GetMesh(0)->GetVertexData(), prefilteredEntity->GetMesh(0)->GetVertexSize(), prefilteredEntity->GetMesh(0)->GetSize());
    drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(prefilteredEntity->GetMesh(0)->GetIndexData(), prefilteredEntity->GetMesh(0)->GetIndexSize());
    drawSurface.m_material = prefilteredEntity->GetMesh(0)->GetMaterial();

    if (m_renderCore.BeginCustomCommandBuffer(cmdBuffer))
    {
        eosVector(VkClearValue) clearValues;
        clearValues.resize(1);
        clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 0.0f } };

        cameraPtr->SetViewport(m_renderCore, cmdBuffer, 0, 0, prefilteredEnvironment->GetWidth(), prefilteredEnvironment->GetHeight());
        cameraPtr->SetScissor(m_renderCore, cmdBuffer, 0, 0, prefilteredEnvironment->GetWidth(), prefilteredEnvironment->GetHeight());


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

        eosVector(Quaternion) rotations;
        rotations.push_back(Quaternion(0.0f, -0.707f, 0.0f, 0.707f));    // right in camera poi
        rotations.push_back(Quaternion(0.0f, 0.707f, 0.0f, 0.707f));     // left in camera poi
        rotations.push_back(Quaternion(-0.707f, 0.0f, 0.0f, 0.707f));    // up in camera poi
        rotations.push_back(Quaternion(0.707f, 0.0f, 0.0f, 0.707f));    // down in camera poi
        rotations.push_back(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));    // rear in camera poi
        rotations.push_back(Quaternion());                              // front in camera poi

        for (ionU32 m = 0; m < mipMapsLevel; ++m)
        {
            for (ionU32 f = 0; f < 6; ++f)
            {
                cameraPtr->SetViewport(m_renderCore, cmdBuffer, 0, 0, static_cast<ionS32>(prefilteredEnvironment->GetWidth() * std::powf(0.5f, static_cast<ionFloat>(m))), static_cast<ionS32>(prefilteredEnvironment->GetHeight() * std::powf(0.5f, static_cast<ionFloat>(m))));

                cameraPtr->StartRenderPass(m_renderCore, renderPass, framebuffer, cmdBuffer, clearValues, static_cast<ionU32>(prefilteredEnvironment->GetWidth()), static_cast<ionU32>(prefilteredEnvironment->GetHeight()));

                // rotate the camera here
                cameraPtr->GetTransform().SetRotation(rotations[f]);
                //cameraPtr->Update(0.0f);
                cameraPtr->UpdateView();

                // draw prefilteredEnvironment
                {
                    // custom draw uniform
                    ionShaderProgramManager().SetRenderParamFloat("roughness", (ionFloat)m / (ionFloat)(mipMapsLevel - 1));

                    const Matrix& projection = cameraPtr->GetPerspectiveProjection();
                    const Matrix& view = cameraPtr->GetView();
                    const Matrix& model = prefilteredEntity->GetTransform().GetMatrixWS();

                    _mm_storeu_ps(&drawSurface.m_modelMatrix[0], model[0]);
                    _mm_storeu_ps(&drawSurface.m_modelMatrix[4], model[1]);
                    _mm_storeu_ps(&drawSurface.m_modelMatrix[8], model[2]);
                    _mm_storeu_ps(&drawSurface.m_modelMatrix[12], model[3]);

                    _mm_storeu_ps(&drawSurface.m_viewMatrix[0], view[0]);
                    _mm_storeu_ps(&drawSurface.m_viewMatrix[4], view[1]);
                    _mm_storeu_ps(&drawSurface.m_viewMatrix[8], view[2]);
                    _mm_storeu_ps(&drawSurface.m_viewMatrix[12], view[3]);

                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[0], projection[0]);
                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[4], projection[1]);
                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[8], projection[2]);
                    _mm_storeu_ps(&drawSurface.m_projectionMatrix[12], projection[3]);

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
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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

    prefilteredEntityHandle->DetachFromParent();

    ionShaderProgramManager().Restart();

    return prefilteredEnvironment;
}

Texture* RenderManager::GetPrefilteredEnvironmentCubemap()
{
    return ionTextureManger().GetTexture(ION_PREFILTEREDENVIRONMENT_TEXTURENAME);
}

void RenderManager::AddDirectionalLight()
{
    m_sceneGraph.AddDirectionalLightToScene();
}

void RenderManager::RemoveDirectionalLight()
{
    m_sceneGraph.RemoveDirectionalLightToScene();
}

DirectionalLight* RenderManager::GetDirectionalLight()
{
    return m_sceneGraph.GetDirectionalLightPtr();
}

ION_NAMESPACE_END