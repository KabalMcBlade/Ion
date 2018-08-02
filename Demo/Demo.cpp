// Demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../Ion/Ion.h"

#include "Objects.h"
#include "Test.h"

//////////////////////////////////////////////////////////////////////////
// COMMON MEMORY

#define MEMORY_1_MB                 1048576
#define MEMORY_4_MB                 MEMORY_1_MB * 4
#define MEMORY_8_MB                 MEMORY_4_MB * 2
#define MEMORY_16_MB                MEMORY_8_MB * 2
#define MEMORY_32_MB                MEMORY_16_MB * 2
#define MEMORY_64_MB                MEMORY_32_MB * 2
#define MEMORY_128_MB               MEMORY_64_MB * 2
#define MEMORY_256_MB               MEMORY_128_MB * 2
#define MEMORY_512_MB               MEMORY_256_MB * 2

#define STL_MAX_HEAP_MEMORY         MEMORY_64_MB
#define STL_MAX_STACK_MEMORY_SIZE   MEMORY_8_MB
#define STL_MAX_LINEAR_MEMORY       MEMORY_8_MB


#define MAX_STACK_MEMORY_BLOCK      128
#define ALL_HEAP_MEMORY             STL_MAX_HEAP_MEMORY + (MEMORY_256_MB)
#define ALL_LINEAR_MEMORY           STL_MAX_LINEAR_MEMORY + (MEMORY_32_MB)
#define ALL_STACK_MEMORY            STL_MAX_STACK_MEMORY_SIZE


//////////////////////////////////////////////////////////////////////////
// VULKAN MEMORY

// just for sample and just to stress :D
#define MEMORY_1_MB                 1048576        
#define VULKAN_BASE_MEMORY_MB       MEMORY_1_MB

#define VULKAN_COMMAND_MEMORY_MB    VULKAN_BASE_MEMORY_MB * 8
#define VULKAN_OBJECT_MEMORY_MB     VULKAN_BASE_MEMORY_MB * 8
#define VULKAN_CACHE_MEMORY_MB      VULKAN_BASE_MEMORY_MB * 8
#define VULKAN_DEVICE_MEMORY_MB     VULKAN_BASE_MEMORY_MB * 32
#define VULKAN_INSTANCE_MEMORY_MB   VULKAN_BASE_MEMORY_MB * 32

#define VULKAN_GPU_MEMORY_MB        VULKAN_BASE_MEMORY_MB * 32

#define VULKAN_GPU_DEVICE_LOCAL_MB  VULKAN_BASE_MEMORY_MB * 512
#define VULKAN_GPU_HOST_VISIBLE_MB  VULKAN_BASE_MEMORY_MB * 256

#define VULKAN_STAGING_BUFFER_MB    VULKAN_BASE_MEMORY_MB * 128

#ifdef _DEBUG
#   define ION_VULKAN_VALIDATION_LAYER true
#else
#   define ION_VULKAN_VALIDATION_LAYER false
#endif

//////////////////////////////////////////////////////////////////////////


#define DEMO_WIDTH 800
#define DEMO_HEIGHT 600

EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE
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

int main()
{
    InitializeAllocators(ALL_HEAP_MEMORY, ALL_LINEAR_MEMORY, ALL_STACK_MEMORY, MAX_STACK_MEMORY_BLOCK);
    InitializeVulkanAllocators(VULKAN_COMMAND_MEMORY_MB, VULKAN_OBJECT_MEMORY_MB, VULKAN_CACHE_MEMORY_MB, VULKAN_DEVICE_MEMORY_MB, VULKAN_INSTANCE_MEMORY_MB, VULKAN_GPU_MEMORY_MB);
    InitializeManagers();

    //////////////////////////////////////////////////////////////////////////
    //
    //  CHOOSE TEST
    //
    //////////////////////////////////////////////////////////////////////////

    ionS32 choice = 0;

    std::cout << "Choose the test to run" << std::endl;
    std::cout << "1 - Colored Triangle" << std::endl;
    std::cout << "2 - Colored Quad" << std::endl;
    std::cout << "3 - Texture Quad" << std::endl;
    std::cout << "4 - Colored Cube" << std::endl;
    std::cout << "5 - Colored Sphere" << std::endl;
    std::cout << "6 - Show BRDF Lookup Texture" << std::endl;
    std::cout << "7 - Model GLTF in white ambient (need a while to load!)" << std::endl;
    std::cout << "8 - Model GLTF using PBR (WORK IN PROGRESS, NEED A WHILE TO LOAD!)" << std::endl;
    std::cout << "Selection: ";

    std::cin >> choice;

    std::cout << std::endl;

    //////////////////////////////////////////////////////////////////////////
    //
    //
    //////////////////////////////////////////////////////////////////////////


    ION_SCOPE_BEGIN


    ionFileSystemManager().Init("Assets", "Shaders", "Textures", "Models");


    ionBool rendererInitialized = false;
    Window window;

    if (window.Create(WndProc, L"Ion Demo", DEMO_WIDTH, DEMO_HEIGHT, false, false))
    {
        window.SetInputMode(true, true);
        rendererInitialized = ionRenderManager().Init(window.GetInstance(), window.GetHandle(), DEMO_WIDTH, DEMO_HEIGHT, false, ION_VULKAN_VALIDATION_LAYER, VULKAN_GPU_DEVICE_LOCAL_MB, VULKAN_GPU_HOST_VISIBLE_MB, VULKAN_STAGING_BUFFER_MB);
    }


    Material* materialSkyBox = ionMaterialManger().CreateMaterial("SkyBox", 0u);


    //////////////////////////////////////////////////////////////////////////

    static const Vector up(0.0f, 1.0f, 0.0f, 0.0f);
    static const Vector right(1.0f, 0.0f, 0.0f, 0.0f);
    static const Vector forward(0.0f, 0.0f, 1.0f, 0.0f);


    Vector cameraPos(0.0f, 0.0f, -3.0f, 0.0f);
    Quaternion cameraRot(NIX_DEG_TO_RAD(0.0f), up);

    Vector entityPos(0.0f, 0.0f, 0.0f, 0.0f);
    Quaternion entityRot(NIX_DEG_TO_RAD(0.0f), up);

    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Generate and load all global texture
    eosString skyboxCubeMapPath = ionFileSystemManager().GetTexturesPath() + "Yokohama3.jpg";
    Texture* skyboxCubeMap = ionTextureManger().CreateTextureFromFile("Yokohama3", skyboxCubeMapPath, ETextureFilter_Default, ETextureRepeat_ClampAlpha, ETextureUsage_Skybox, ETextureType_Cubic, 1);
    
    Texture* brdflut = nullptr;
    Texture* irradiance = nullptr;
    Texture* prefilteredEnvironmentMap = nullptr;
    

    // generate brdf, irradiance and prefiltered cube map
    {
        Vector cameraGenPos(0.0f, 0.0f, 0.0f, 0.0f);

        //
        Camera* cameraPtr = eosNew(Camera, ION_MEMORY_ALIGNMENT_SIZE);
        ObjectHandler camera(cameraPtr);
        cameraPtr->SetCameraType(ion::Camera::ECameraType::ECameraType_FirstPerson);
        cameraPtr->GetTransform().SetPosition(cameraGenPos);
        cameraPtr->GetTransform().SetRotation(cameraRot);
        cameraPtr->SetRenderPassParameters(1.0f, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 1.0f, 1.0f);
        cameraPtr->SetViewportParameters(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
        cameraPtr->SetScissorParameters(0.0f, 0.0f, 1.0f, 1.0f);

        Skybox* skyboxPtr = cameraPtr->AddSkybox();
        skyboxPtr->SetMaterial(materialSkyBox);
        materialSkyBox->GetBasePBR().SetBaseColorTexture(skyboxCubeMap);

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
        sampler.m_texture = materialSkyBox->GetBasePBR().GetBaseColorTexture();

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

        const ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SkyBox", ion::EShaderStage_Vertex, vertexLayout);
        const ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SkyBox", ion::EShaderStage_Fragment, fragmentLayout);

        materialSkyBox->SetShaderProgramName("SkyboxEnvironmentMapGeneration");
        materialSkyBox->SetVertexLayout(skyboxPtr->GetVertexLayout());
        materialSkyBox->SetConstantsShaders(constants);
        materialSkyBox->SetShaders(vertexShaderIndex, fragmentShaderIndex);
        materialSkyBox->GetState().SetCullingMode(ECullingMode_Front);
        materialSkyBox->GetState().SetDepthFunctionMode(EDepthFunction_Less);
        materialSkyBox->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_LesserOrEqual);

        // generation
        brdflut = ionRenderManager().GenerateBRDF(camera);
        //irradiance = ionRenderManager().GenerateIrradianceCubemap(materialSkyBox->GetBasePBR().GetBaseColorTexture(), camera);
        //prefilteredEnvironmentMap = ionRenderManager().GeneratePrefilteredEnvironmentCubemap(materialSkyBox->GetBasePBR().GetBaseColorTexture(), camera);

        ionShaderProgramManager().UnloadShader(vertexShaderIndex);
        ionShaderProgramManager().UnloadShader(fragmentShaderIndex);
        ionShaderProgramManager().Restart();
    }

    //////////////////////////////////////////////////////////////////////////
    // Create Camera
    FPSCamera* camera = eosNew(FPSCamera, ION_MEMORY_ALIGNMENT_SIZE);
    ObjectHandler cameraHandle(camera);
    camera->SetCameraType(ion::Camera::ECameraType::ECameraType_FirstPerson);
    camera->SetPerspectiveProjection(60.0f, (ionFloat)DEMO_WIDTH / (ionFloat)DEMO_HEIGHT, 0.1f, 256.0f);
    camera->GetTransform().SetPosition(cameraPos);
    camera->GetTransform().SetRotation(cameraRot);
    camera->SetRenderPassParameters(1.0f, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 1.0f, 1.0f);
    camera->SetViewportParameters(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    camera->SetScissorParameters(0.0f, 0.0f, 1.0f, 1.0f);
    camera->SetParameters(0.001f, 0.05f, true);

    //////////////////////////////////////////////////////////////////////////
    // Create SkyBox

    Skybox* skyboxPtr = camera->AddSkybox();
    skyboxPtr->SetMaterial(materialSkyBox);
    materialSkyBox->GetBasePBR().SetBaseColorTexture(skyboxCubeMap);

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
    sampler.m_texture = materialSkyBox->GetBasePBR().GetBaseColorTexture();

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

    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SkyBox", ion::EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), "SkyBox", ion::EShaderStage_Fragment, fragmentLayout);

    materialSkyBox->SetShaderProgramName("SkyBox");
    materialSkyBox->SetVertexLayout(skyboxPtr->GetVertexLayout());
    materialSkyBox->SetConstantsShaders(constants);
    materialSkyBox->SetShaders(vertexShaderIndex, fragmentShaderIndex);


    //////////////////////////////////////////////////////////////////////////
    // Create Entity to render
    RotatingEntity* test = eosNew(RotatingEntity, ION_MEMORY_ALIGNMENT_SIZE);
    ObjectHandler testHandle(test);
    test->GetTransform().SetPosition(entityPos);
    test->GetTransform().SetRotation(entityRot);

    //////////////////////////////////////////////////////////////////////////
    //
    //  EXECUTE THE CHOSEN TEST
    //
    //////////////////////////////////////////////////////////////////////////

    switch(choice)
    {
    case 1:
        Test_ColoredTriangle(*test);
        break;
    case 2:
        Test_ColoredQuad(*test);
        break;
    case 3:
        Test_TexturedQuad(*test);
        break;
    case 4:
        Test_ColoredCube(*test);
        break;
    case 5:
        Test_ColoredSphere(*test);
        break;
    case 6:
        Test_TexturedQuadEx(*test, ionRenderManager().GetBRDF());
        break;
    case 7:
        Test_Model_Ambient(*test);
        break;
    case 8:
        Test_ModelPBR_WIP(*test);
        break;
    default:
        std::cout << "Any valid choose made, will run the Colored Triangle test" << std::endl;
        Test_ColoredTriangle(*test);
        break;
    }

    BoundingBoxEntity* boundingBox = eosNew(BoundingBoxEntity, ION_MEMORY_ALIGNMENT_SIZE);
    ObjectHandler boundingBoxHandle(boundingBox);

    BoundingBox bbTransformed = test->GetBoundingBox()->GetTransformed(test->GetTransform().GetMatrix());

    boundingBox->GetTransform().SetPosition(bbTransformed.GetCenter());
    boundingBox->GetTransform().SetScale(bbTransformed.GetSize());

    Test_DrawBoundingBox(*boundingBox);

    boundingBox->AttachToParent(testHandle);

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////

    // first full the scene graph
    ionRenderManager().AddToSceneGraph(cameraHandle);
    ionRenderManager().AddToSceneGraph(testHandle);

    // then set the active node will receive the input
    ionRenderManager().RegisterToInput(cameraHandle);
    ionRenderManager().RegisterToInput(boundingBoxHandle);
    ionRenderManager().RegisterToInput(testHandle);

    if (rendererInitialized)
    {
        window.Loop();
    }

    ionRenderManager().Shutdown();

    ionFileSystemManager().Shutdown();

    ION_SCOPE_END
        
    ShutdownManagers();
    ShutdownVulkanAllocators();
    ShutdownAllocators();

    getchar();

    return 0;
}

