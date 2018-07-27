#include "RenderManager.h"

#include "VertexCacheManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"

#include "../Utilities/LoaderGLTF.h"

#include "../Geometry/Mesh.h"
#include "../Geometry/PrimitiveFactory.h"

#include "../Texture/TextureManager.h"

#include "../Core/FileSystemManager.h"

#include "../Shader/ShaderProgramManager.h"

#include "../Material/MaterialManager.h"


#define ION_BRDFLUT_TEXTURENAME    "BRDFLUT"
#define ION_BRDFLUT_SHADER_NAME    "GenerateBRDFLUT"

//#define SHADOW_MAP_SIZE                    1024

#define ION_CACHE_LINE_SIZE        128
#define ION_MAX_FRAME_MEMORY    67305472    //64 * 1024 * 1024


#define ION_FPS_LIMIT  0.01666666666666666666666666666667f     // 1.0 / 60.0f



EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager() : m_deltaTime(0.0f), m_running(false)
{

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

ionBool RenderManager::LoadModelFromFile(const eosString& _fileName, Entity& _entity, ionBool _generateNormalWhenMissing /*= false*/, ionBool _generateTangentWhenMissing /*= false*/, ionBool _setBitangentSign /*= false*/)
{
    LoaderGLTF loader;
    return loader.Load(_fileName, _entity, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);
}

void RenderManager::LoadPrimitive(EVertexLayout _layout, EPrimitiveType _type, Entity& _entity)
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

void RenderManager::AddToSceneGraph(NodeHandle _node)
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

void RenderManager::RegisterToInput(const NodeHandle& _node)
{
    m_sceneGraph.RegisterToInput(_node);
}

void RenderManager::UnregisterFromInput(const NodeHandle& _node)
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

Texture* RenderManager::GenerateBRDF()
{
    const Vector up(0.0f, 1.0f, 0.0f, 0.0f);

    const Vector cameraPos(0.0f, 0.0f, -1.0f, 0.0f);
    const Quaternion cameraRot(NIX_DEG_TO_RAD(0.0f), up);

    const Vector entityPos(0.0f, 0.0f, 0.0f, 0.0f);
    const Quaternion entityRot(NIX_DEG_TO_RAD(0.0f), up);

    Texture* brdflut = ionTextureManger().GenerateTexture(ION_BRDFLUT_TEXTURENAME, 512, 512, ETextureFormat_BRDF, ETextureFilter_Default, ETextureRepeat_Clamp);

    CameraHandle camera = eosNew(Camera, ION_MEMORY_ALIGNMENT_SIZE);
    camera->SetCameraType(Camera::ECameraType::ECameraType_FirstPerson);
    camera->SetPerspectiveProjection(60.0f, static_cast<ionFloat>(brdflut->GetWidth()) / static_cast<ionFloat>(brdflut->GetHeight()), 0.1f, 256.0f);
    camera->GetTransformHandle()->SetPosition(cameraPos);
    camera->GetTransformHandle()->SetRotation(cameraRot);
    camera->SetRenderPassParameters(1.0f, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 1.0f, 1.0f);
    camera->SetViewportParameters(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    camera->SetScissorParameters(0.0f, 0.0f, 1.0f, 1.0f);

    EntityHandle brdflutEntity = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE);
    brdflutEntity->GetTransformHandle()->SetPosition(entityPos);
    brdflutEntity->GetTransformHandle()->SetRotation(entityRot);

    LoadPrimitive(EVertexLayout_Pos, EPrimitiveType_Quad, *brdflutEntity);

    Material* material = ionMaterialManger().CreateMaterial("BRDFLUT", 0u);
    brdflutEntity->GetMesh(0)->SetMaterial(material);

    ShaderLayoutDef vertexLayout;      // empty
    ShaderLayoutDef fragmentLayout;    // empty

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_BRDFLUT_SHADER_NAME, EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_BRDFLUT_SHADER_NAME, EShaderStage_Fragment, fragmentLayout);

    brdflutEntity->GetMesh(0)->GetMaterial()->SetShaderProgramName(ION_BRDFLUT_SHADER_NAME);
    //brdflutEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(EVertexLayout_Empty);
    brdflutEntity->GetMesh(0)->GetMaterial()->SetVertexLayout(brdflutEntity->GetMesh(0)->GetLayout());

    brdflutEntity->GetMesh(0)->GetMaterial()->SetShaders(vertexShaderIndex, fragmentShaderIndex);

    brdflutEntity->GetMesh(0)->GetMaterial()->GetState().SetCullingMode(ECullingMode_TwoSide);

    VkRenderPass renderPass = m_renderCore.CreateTexturedRenderPass(brdflut);
    VkFramebuffer framebuffer = m_renderCore.CreateTexturedFrameBuffer(renderPass, brdflut);

    VkCommandBuffer cmdBuffer = m_renderCore.CreateCustomCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    if (m_renderCore.BeginCustomCommandBuffer(cmdBuffer))
    {
        eosVector(VkClearValue) clearValues;
        clearValues.resize(1);
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

        camera->StartRenderPass(m_renderCore, renderPass, framebuffer, cmdBuffer, clearValues, static_cast<ionU32>(brdflut->GetWidth()), static_cast<ionU32>(brdflut->GetHeight()));
        
        camera->SetViewport(m_renderCore, cmdBuffer, 0, 0, brdflut->GetWidth(), brdflut->GetHeight());
        camera->SetScissor(m_renderCore, cmdBuffer, 0, 0, brdflut->GetWidth(), brdflut->GetHeight());

        const Matrix& projection = camera->GetPerspectiveProjection();
        const Matrix& view = camera->GetView();

        const Matrix& model = brdflutEntity->GetTransformHandle()->GetMatrixWS();

        DrawSurface drawSurface;

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

        drawSurface.m_visible = brdflutEntity->IsVisible();
        drawSurface.m_indexStart = brdflutEntity->GetMesh(0)->GetIndexStart();
        drawSurface.m_indexCount = brdflutEntity->GetMesh(0)->GetIndexCount();
        drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(brdflutEntity->GetMesh(0)->GetVertexData(), brdflutEntity->GetMesh(0)->GetVertexSize());
        drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(brdflutEntity->GetMesh(0)->GetIndexData(), brdflutEntity->GetMesh(0)->GetIndexSize());
        drawSurface.m_material = brdflutEntity->GetMesh(0)->GetMaterial();

        m_renderCore.SetState(drawSurface.m_material->GetState().GetStateBits());
        m_renderCore.Draw(cmdBuffer, drawSurface);

        camera->EndRenderPass(m_renderCore, cmdBuffer);

        m_renderCore.EndCustomCommandBuffer(cmdBuffer);
        m_renderCore.FlushCustomCommandBuffer(cmdBuffer);
    }

    return brdflut;
}

Texture* RenderManager::GetBRDF()
{
    Texture* brdflut = ionTextureManger().GetTexture(ION_BRDFLUT_TEXTURENAME);
    if (brdflut == nullptr)
    {
        brdflut = GenerateBRDF();
    }
    return brdflut;
}

void RenderManager::SaveBRDF(const eosString& _path)
{
    Texture* brdflut = ionTextureManger().GetTexture(ION_BRDFLUT_TEXTURENAME);
    ionTextureManger().SaveTexture(_path, brdflut);
}

ION_NAMESPACE_END