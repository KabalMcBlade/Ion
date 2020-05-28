// Demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../Ion/Ion.h"

#include "Objects.h"



//////////////////////////////////////////////////////////////////////////
// APP VULKAN MEMORY

#define MEMORY_1_MB             1048576ULL
#define MEMORY_4_MB             MEMORY_1_MB * 4ULL
#define MEMORY_8_MB             MEMORY_4_MB * 2ULL
#define MEMORY_16_MB            MEMORY_8_MB * 2ULL
#define MEMORY_32_MB            MEMORY_16_MB * 2ULL
#define MEMORY_64_MB            MEMORY_32_MB * 2ULL
#define MEMORY_128_MB           MEMORY_64_MB * 2ULL
#define MEMORY_256_MB           MEMORY_128_MB * 2ULL
#define MEMORY_512_MB           MEMORY_256_MB * 2ULL
#define MEMORY_1024_MB          MEMORY_512_MB * 2ULL
#define MEMORY_2048_MB          MEMORY_1024_MB * 2ULL
#define MEMORY_4096_MB          MEMORY_2048_MB * 2ULL
#define MEMORY_8192_MB          MEMORY_4096_MB * 2ULL

#define VULKAN_GPU_DEVICE_LOCAL_MB  MEMORY_1024_MB
#define VULKAN_GPU_HOST_VISIBLE_MB  MEMORY_1024_MB

#define VULKAN_STAGING_BUFFER_MB    MEMORY_512_MB

#ifdef _DEBUG
#   define ION_VULKAN_VALIDATION_LAYER true
#else
#   define ION_VULKAN_VALIDATION_LAYER false
#endif

//////////////////////////////////////////////////////////////////////////


#define DEMO_WIDTH 800
#define DEMO_HEIGHT 600

EOS_USING_NAMESPACE
ION_USING_NAMESPACE

EOS_OPTIMIZATION_OFF
ION_OPTIMIZATION_OFF


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        PostMessage(hWnd, ION_KEY_DOWN, wParam, lParam);
        break;
    case WM_SYSKEYUP:
    case WM_KEYUP:
        PostMessage(hWnd, ION_KEY_UP, wParam, lParam);
        break;
    case WM_CLOSE:
        PostMessage(hWnd, ION_WND_CLOSE, wParam, lParam);
        break;
    case WM_SIZE:
    case WM_EXITSIZEMOVE:
        PostMessage(hWnd, ION_WND_RESIZE, wParam, lParam);
        break;
    case WM_MOUSEMOVE:
        PostMessage(hWnd, ION_MOUSE_MOVE, wParam, lParam);
        break;
    case WM_MOUSEWHEEL:
        PostMessage(hWnd, ION_MOUSE_WHEEL, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        PostMessage(hWnd, ION_MOUSE_CLICK, 0, 1);
        break;
    case WM_LBUTTONUP:
        PostMessage(hWnd, ION_MOUSE_CLICK, 0, 0);
        break;
    case WM_RBUTTONDOWN:
        PostMessage(hWnd, ION_MOUSE_CLICK, 1, 1);
        break;
    case WM_RBUTTONUP:
        PostMessage(hWnd, ION_MOUSE_CLICK, 1, 0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// update all shaders in the entity in cascade
void UpdateAllShadersCascade(Node* _node, ionS32 _vertexShaderIndex, ionS32 _fragmentShaderIndex)
{
    if (_node->GetNodeType() == ENodeType_Entity)
    {
        Entity* entity = dynamic_cast<Entity*>(_node);

        const ionU32 meshCount = entity->GetMeshCount();
        for (ionU32 i = 0; i < meshCount; ++i)
        {
            Material* material = entity->GetMesh(i)->GetMaterial();
            if (material != nullptr)
            {
                material->SetShaders(_vertexShaderIndex, _fragmentShaderIndex);
            }
        }
    }

    if (_node->GetChildren().empty())
    {
        return;
    }

    const ionVector<Node*, NodeAllocator, Node::GetAllocator>& children = _node->GetChildren();
    ionVector<Node*, NodeAllocator, Node::GetAllocator>::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;
    for (; it != end; ++it)
    {
        UpdateAllShadersCascade((*it), _vertexShaderIndex, _fragmentShaderIndex);
    }
}

// need to start from false in order to work!
void CheckIfAllMaterialsAreUnlit(Node* _node, ionBool& _areUnlit)
{
    if (_node->GetNodeType() == ENodeType_Entity)
    {
        Entity* entity = dynamic_cast<Entity*>(_node);

        const ionU32 meshCount = entity->GetMeshCount();
        for (ionU32 i = 0; i < meshCount; ++i)
        {
            ion::Material* material = entity->GetMesh(i)->GetMaterial();
            if (material != nullptr)
            {
                if (material->IsUnlit())
                {
                    _areUnlit = true;
                }
            }
        }
    }

    if (_node->GetChildren().empty())
    {
        return;
    }

    const ionVector<Node*, NodeAllocator, Node::GetAllocator>& children = _node->GetChildren();
    ionVector<Node*, NodeAllocator, Node::GetAllocator>::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;
    for (; it != end; ++it)
    {
        CheckIfAllMaterialsAreUnlit((*it), _areUnlit);
    }
}

struct TestMalloc
{
	ionU32 a;
	ionU32 b;
	ionU32 c;
	ionU32 d;
};


int main(int argc, char **argv)
{
    ShaderProgramHelper::Create();

    ionFileSystemManager().Init("Assets", "Shaders", "Textures", "Models");

    ionBool rendererInitialized = false;
    Window window;

    window.GetCommandLineParse().AddWithValue<ionString>("-model", false);
    window.GetCommandLineParse().AddWithValue<ionString>("-primitive", false);
    window.GetCommandLineParse().Add("-usepath", false);

#ifdef _DEBUG
    //window.GetCommandLineParse().AddWithValueAndDefault<ionU32>("-dumpgltf", false, 1);
#endif

    if (!window.ParseCommandLine(argc, argv))
    {
        return false;
    }

    if (window.Create(WndProc, L"Ion Demo - GLTF Viewer"))
    {
        window.SetInputMode(true, true);
        rendererInitialized = ionRenderManager().Init(window.GetInstance(), window.GetHandle(), window.GetWidth(), window.GetHeight(), window.IsFullscreen(), ION_VULKAN_VALIDATION_LAYER, VULKAN_GPU_DEVICE_LOCAL_MB, VULKAN_GPU_HOST_VISIBLE_MB, VULKAN_STAGING_BUFFER_MB);
    }

    //////////////////////////////////////////////////////////////////////////
    // Generate and load all global texture
	ionString texturePath = ionFileSystemManager().GetTexturesPath();
	texturePath.append("misty_pines_4k.hdr");
    const Texture* nullTextureMap = ionRenderManager().GenerateNullTexture();
    const Texture* skyboxCubeMap = ionTextureManger().CreateTextureFromFile("misty_pines_4k", texturePath, ETextureFilterMin_Linear_MipMap_Linear, ETextureFilterMag_Linear, ETextureRepeat_ClampAlpha, ETextureUsage_SkyboxHDR, ETextureType_Cubic, 1);

    Material* skyboxMaterial = ionMaterialManger().CreateMaterial("SkyBox", 0u);

    ionS32 skyboxVertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SkyBox", ion::EShaderStage_Vertex);
    ionS32 skyboxFragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SkyBox", ion::EShaderStage_Fragment);

    //////////////////////////////////////////////////////////////////////////
    // Create Camera
    static const Vector4 cameraPos(0.0f, 0.0f, -3.0f, 0.0f);

    
    MainCamera* camera = CreateNode(MainCamera);
    camera->SetCameraType(ion::Camera::ECameraType::ECameraType_LookAt);
    camera->SetPerspectiveProjection(60.0f, (ionFloat)DEMO_WIDTH / (ionFloat)DEMO_HEIGHT, 0.1f, 100.0f);
    camera->SetRenderPassParameters(1.0f, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 1.0f, 1.0f);
    camera->SetViewportParameters(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    camera->SetScissorParameters(0.0f, 0.0f, 1.0f, 1.0f);
    camera->GetTransform().SetPosition(cameraPos);

    //////////////////////////////////////////////////////////////////////////
    // Create SkyBox
    Skybox* skyboxPtr = camera->AddSkybox();
    skyboxPtr->SetMaterial(skyboxMaterial);
    skyboxMaterial->GetBasePBR().SetBaseColorTexture(skyboxCubeMap);

    // one uniform structure bound in the index 0 in the shader stage
    ion::UniformBinding uniform;
    uniform.m_bindingIndex = 0;
    uniform.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EBufferParameterType_Matrix);
    uniform.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EBufferParameterType_Matrix);
    uniform.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
    uniform.m_type.push_back(ion::EBufferParameterType_Matrix);

    // one sampler bound in the index 1 in the shader stage
    ion::SamplerBinding sampler;
    sampler.m_bindingIndex = 1;
    sampler.m_texture = skyboxMaterial->GetBasePBR().GetBaseColorTexture();

    // constants
    ConstantsBindingDef constants;
    constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
    constants.m_values.push_back(4.5f);
    constants.m_values.push_back(2.2f);

    // set the shaders layout
    ion::ShaderLayoutDef vertexLayout;
    vertexLayout.m_uniforms.push_back(uniform);

    ion::ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_samplers.push_back(sampler);

    skyboxMaterial->SetVertexShaderLayout(vertexLayout);
    skyboxMaterial->SetFragmentShaderLayout(fragmentLayout);
    skyboxMaterial->SetVertexLayout(skyboxPtr->GetVertexLayout());
    skyboxMaterial->SetConstantsShaders(constants);
    skyboxMaterial->SetShaders(skyboxVertexShaderIndex, skyboxFragmentShaderIndex);

    // check here
    skyboxMaterial->GetState().SetCullingMode(ECullingMode_Front);
    skyboxMaterial->GetState().SetDepthFunctionMode(EDepthFunction_Less);
    skyboxMaterial->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_LesserOrEqual);


    //////////////////////////////////////////////////////////////////////////
    // Continue texture generation
    const Texture* brdflut = ionRenderManager().GenerateBRDF(camera);
    const Texture* irradiance = ionRenderManager().GenerateIrradianceCubemap(camera);
    const Texture* prefilteredEnvironmentMap = ionRenderManager().GeneratePrefilteredEnvironmentCubemap(camera);

    // reset camera pos after cubemap generations
    const Quaternion identity;
    camera->GetTransform().SetRotation(identity);
    camera->Update(0.0f);
    camera->UpdateView();

    
    //////////////////////////////////////////////////////////////////////////
    // Create Entity to render
    RotatingEntity* test = CreateNode(RotatingEntity, "GameEntity");

    //////////////////////////////////////////////////////////////////////////

    if (window.GetCommandLineParse().HasValue("-model"))
    {
        const ionString modelVar = window.GetCommandLineParse().GetValue<ionString>("-model");
        ionString model;
        if (window.GetCommandLineParse().IsSet("-usepath"))
        {
            model = modelVar;
        }
        else
        {
            model = ionFileSystemManager().GetModelsPath();
			model.append(modelVar);
        }

        ionRenderManager().LoadModelFromFile(model, camera, (Node*&)test);

#ifdef _DEBUG
		/*
        if (window.GetCommandLineParse().IsSet("-dumpgltf"))
        {
            const ionU32 serializationLevelNum = window.GetCommandLineParse().GetValue<ionU32>("-dumpgltf");
            const LoaderGLTF::ESerializationLevel serializationLevel = static_cast<LoaderGLTF::ESerializationLevel>(serializationLevelNum);

            ionRenderManager().DumpModelToFile(model, test, serializationLevel);
            
            ionString dumpFile = model + ".json";
            std::cout << "Model " << test->GetName() << " dumped in " << dumpFile << std::endl;
        }
		*/
#endif
    }
    else if (window.GetCommandLineParse().HasValue("-primitive"))
    {
        std::srand(static_cast<ionU32>(std::time(nullptr)));
        ionFloat r = static_cast<ionFloat>(std::rand()) / static_cast<ionFloat>(RAND_MAX);
        ionFloat g = static_cast<ionFloat>(std::rand()) / static_cast<ionFloat>(RAND_MAX);
        ionFloat b = static_cast<ionFloat>(std::rand()) / static_cast<ionFloat>(RAND_MAX);
        ionFloat a = 1.0f;

        const ionString primitive = window.GetCommandLineParse().GetValue<ionString>("-primitive");
        if (primitive == "triangle")
        {
            ionRenderManager().LoadColoredTriangle((Entity*&)test, r, g, b, a);
        }
        else if (primitive == "quad")
        {
            ionRenderManager().LoadColoredQuad((Entity*&)test, r, g, b, a);
        }
        else if (primitive == "cube")
        {
            ionRenderManager().LoadColoredCube((Entity*&)test, r, g, b, a);
        }
        else if (primitive == "sphere")
        {
            ionRenderManager().LoadColoredSphere((Entity*&)test, r, g, b, a);
        }
        else if (primitive == "pyramid")
        {
            ionRenderManager().LoadColoredPyramid((Entity*&)test, r, g, b, a);
        }
        else
        {
            std::cout << "primitive is not valid: use -primitive [triangle, quad, cube, sphere]" << std::endl;
            std::cout << "Will be generate a simple triangle now" << std::endl;
            ionRenderManager().LoadColoredTriangle((Entity*&)test, r, g, b, a);
        }
    }
    else
    {
        std::cout << "model or primitive does not supply: use -model \"modelname\" or -primitive [triangle, quad, cube, sphere]" << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////

    // set camera to rotating object
    test->SetCameraReference(camera);

    // first full the scene graph
    ionRenderManager().AddToSceneGraph(camera);
    ionRenderManager().AddToSceneGraph(test);


    // then set the active node will receive the input
    ionRenderManager().RegisterToInput(camera);
    ionRenderManager().RegisterToInput(test);

    ionRenderManager().AddDirectionalLight();
    DirectionalLight* directionalLight = ionRenderManager().GetDirectionalLight();


    const Vector4 lightCol(1.0f, 1.0f, 1.0f, 1.0f);
    Quaternion lightRot(NIX_DEG_TO_RAD(45.0f), NIX_DEG_TO_RAD(20.0f), 0.0f);

    directionalLight->GetTransform().SetRotation(lightRot);
    directionalLight->SetColor(lightCol);
    //////////////////////////////////////////////////////////////////////////

    
    //////////////////////////////////////////////////////////////////////////
    // Create new camera for the arrow to debug light if something is not unlit!
    // If all are unlit, does not use light!
    ionBool areAllModelMaterialsUnlit = false;
    CheckIfAllMaterialsAreUnlit(test, areAllModelMaterialsUnlit);

	ion::Camera* cameraLightDebug = nullptr;
	DirectionalLightDebugEntity* dirlLightDebugEntity = nullptr;
    if (!areAllModelMaterialsUnlit && test->GetMeshRenderer() != nullptr)
    {
        cameraLightDebug = CreateNode(ion::Camera, "DebugLightCamera", false);
        cameraLightDebug->SetCameraType(ion::Camera::ECameraType::ECameraType_LookAt);
        cameraLightDebug->SetPerspectiveProjection(60.0f, (ionFloat)DEMO_WIDTH / (ionFloat)DEMO_HEIGHT, 0.1f, 100.0f);
        cameraLightDebug->SetRenderPassParameters(1.0f, 0, 0.0f, 1.0f, 0.0f);
        cameraLightDebug->SetViewportParameters(0.0f, 0.0f, 0.25f, 0.25f, 0.0f, 1.0f);
        cameraLightDebug->SetScissorParameters(0.0f, 0.0f, 0.25f, 0.25f);
        cameraLightDebug->GetTransform().SetPosition(cameraPos);
        cameraLightDebug->RemoveFromRenderLayer(ENodeRenderLayer_Default);
        cameraLightDebug->AddToRenderLayer(ENodeRenderLayer_1);

        //
        // Arrow for directional lighting debug
        dirlLightDebugEntity = CreateNode(DirectionalLightDebugEntity);
        ionRenderManager().LoadColoredPyramid((Entity*&)dirlLightDebugEntity, 0.8f, 0.2f, 0.2f, 1.0f);
        dirlLightDebugEntity->RemoveFromRenderLayer(ENodeRenderLayer_Default);
        dirlLightDebugEntity->AddToRenderLayer(ENodeRenderLayer_1);
        dirlLightDebugEntity->SetVisible(false);
        dirlLightDebugEntity->GetTransform().SetScale(1.0f, 1.0f, 2.0f);


        // override all shader with an unlit
        ionS32 unlitVertexShader = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_PBR_SHADER_NAME, EShaderStage_Vertex);
        ionS32 unlitFragmentShader = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_UNLIT_SHADER_NAME, EShaderStage_Fragment);
        UpdateAllShadersCascade(dirlLightDebugEntity, unlitVertexShader, unlitFragmentShader);


        ionRenderManager().AddToSceneGraph(cameraLightDebug);
        ionRenderManager().AddToSceneGraph(dirlLightDebugEntity);

        ionRenderManager().RegisterToInput(dirlLightDebugEntity);

        // update directional lighting debug rotation
        dirlLightDebugEntity->GetTransform().SetRotation(lightRot);
    }

    if (rendererInitialized)
    {
        window.Loop();
    }
    
	if (!areAllModelMaterialsUnlit && test->GetMeshRenderer() != nullptr)
	{
		DestroyNode(cameraLightDebug);
		DestroyNode(dirlLightDebugEntity);
	}

	DestroyNode(test);
	DestroyNode(camera);



    ionRenderManager().PrepareToShutDown();
    ionRenderManager().RemoveAllSceneGraph();

    ionRenderManager().Shutdown();

    ionFileSystemManager().Shutdown();

    ShaderProgramHelper::Destroy();

    return 0;
}

