// Demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//#include "../Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"
#include "../Ion/Ion.h"

//////////////////////////////////////////////////////////////////////////
// COMMON MEMORY

#define MEMORY_1_MB                 1048576
#define MEMORY_4_MB                 MEMORY_1_MB * 4
#define MEMORY_8_MB                 MEMORY_4_MB * 2
#define MEMORY_16_MB                MEMORY_8_MB * 2
#define MEMORY_32_MB                MEMORY_16_MB * 2
#define MEMORY_64_MB                MEMORY_32_MB * 2
#define MEMORY_128_MB               MEMORY_64_MB * 2


#define STL_MAX_HEAP_MEMORY         MEMORY_32_MB
#define STL_MAX_STACK_MEMORY_SIZE   MEMORY_8_MB
#define STL_MAX_LINEAR_MEMORY       MEMORY_8_MB


#define MAX_STACK_MEMORY_BLOCK      128
#define ALL_HEAP_MEMORY             STL_MAX_HEAP_MEMORY + (MEMORY_128_MB)
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

#define VULKAN_GPU_MEMORY_MB        VULKAN_BASE_MEMORY_MB * 8

#define VULKAN_GPU_DEVICE_LOCAL_MB  VULKAN_BASE_MEMORY_MB * 128
#define VULKAN_GPU_HOST_VISIBLE_MB  VULKAN_BASE_MEMORY_MB * 64

#define VULKAN_STAGING_BUFFER_MB    VULKAN_BASE_MEMORY_MB * 64

#ifdef _DEBUG
#   define ION_VULKAN_VALIDATION_LAYER true
#else
#   define ION_VULKAN_VALIDATION_LAYER false
#endif

//////////////////////////////////////////////////////////////////////////


#define DEMO_WIDTH 800
#define DEMO_HEIGHT 600

#define DEMO_SHADER_MODEL   "DamagedHelmet"
#define DEMO_SHADER_PROG    "DamagedHelmet"

EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE
ION_USING_NAMESPACE

EOS_OPTIMIZATION_OFF
ION_OPTIMIZATION_OFF



ionBool GetFullPath(const eosString& partialPath, eosString& fullPath)
{
    char full[_MAX_PATH];
    if (_fullpath(full, partialPath.c_str(), _MAX_PATH) != NULL)
    {
        fullPath = full;
        return true;
    }
    return false;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            //PostQuitMessage(0);
            PostMessage(hWnd, WM_USER + 2, wParam, lParam);
            break;
        }
    case WM_CLOSE:
        PostMessage(hWnd, WM_USER + 2, wParam, lParam);
        break;
	case WM_SIZE:
	case WM_EXITSIZEMOVE:
		PostMessage(hWnd, WM_USER + 1, wParam, lParam);
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

    ION_SCOPE_BEGIN


    eosString demoPath;
    GetFullPath("./", demoPath);
    demoPath.append("Assets/");

    eosString damagedHelmetModelPath = demoPath;
    damagedHelmetModelPath.append("DamagedHelmet.gltf");


    ionBool rendererInitialized = false;
    Window window;

    if (window.Create(WndProc, L"Ion Demo", DEMO_WIDTH, DEMO_HEIGHT, false))
    {
        rendererInitialized = ionRenderManager().Init(window.GetInstance(), window.GetHandle(), DEMO_WIDTH, DEMO_HEIGHT, false, ION_VULKAN_VALIDATION_LAYER, demoPath, VULKAN_GPU_DEVICE_LOCAL_MB, VULKAN_GPU_HOST_VISIBLE_MB, VULKAN_STAGING_BUFFER_MB);
    }


    static const Vector up(0.0f, 1.0f, 0.0f, 1.0f);
    static const Vector right(1.0f, 0.0f, 0.0f, 1.0f);
    static const Vector forward(0.0f, 0.0f, 1.0f, 1.0f);

    Vector rootPos(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion rootRot(NIX_DEG_TO_RAD(0.0f), up);

    Vector cameraPos(0.0f, 0.0f, -1.0f, 1.0f);
    Quaternion cameraRot(NIX_DEG_TO_RAD(0.0f), up);

    Vector entityPos(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion entityRot(NIX_DEG_TO_RAD(0.0f), up);


    //
    Entity *pRoot = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE);
    EntityHandle root(pRoot);
    root->GetTransformHandle()->SetPosition(rootPos);
    root->GetTransformHandle()->SetRotation(rootRot);

    //
    BaseCamera *pBaseCamera = eosNew(BaseCamera, ION_MEMORY_ALIGNMENT_SIZE);
    BaseCameraHandle camera(pBaseCamera);
    camera->SetCameraType(BaseCamera::ECameraType::ECameraType_FirstPerson);
    camera->SetPerspectiveProjection(90.0f, (ionFloat)DEMO_WIDTH / (ionFloat)DEMO_HEIGHT, 0.1f, 100.0f);
    camera->GetTransformHandle()->SetPosition(cameraPos);
    camera->GetTransformHandle()->SetRotation(cameraRot);



    //
    Entity *pTest = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE);
    EntityHandle test(pTest);
    test->GetTransformHandle()->SetPosition(entityPos);
    test->GetTransformHandle()->SetRotation(entityRot);

    
    /*
    //
    // this model has:
    // Position, Normal, TextCoord
    //
    ionRenderManager().LoadModelFromFile(damagedHelmetModelPath, *test);

    //
    ShaderLayoutDef vertexLayout;
    vertexLayout.m_bindings.push_back(EShaderBinding_Uniform);
    vertexLayout.m_uniforms.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    vertexLayout.m_uniforms.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    vertexLayout.m_uniforms.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    ShaderLayoutDef fragmentLayout;
    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(DEMO_SHADER_MODEL, EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(DEMO_SHADER_MODEL, EShaderStage_Fragment, fragmentLayout);
    test->GetMaterial(0, 0)->SetShaderProgramName(DEMO_SHADER_PROG);
    test->GetMaterial(0, 0)->SetVertexLayout(EVertexLayout_Vertices_Simple);
    test->GetMaterial(0, 0)->SetVertexShaderIndex(vertexShaderIndex);
    test->GetMaterial(0, 0)->SetFragmentShaderIndex(fragmentShaderIndex);
    */
    



    //
    /*
    ShaderLayoutDef vertexLayout;
    ShaderLayoutDef fragmentLayout;
    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("BaseTriangle1", EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("BaseTriangle1", EShaderStage_Fragment, fragmentLayout);
    */

    //
    /*
    ShaderLayoutDef vertexLayout;
    ShaderLayoutDef fragmentLayout;
    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("BaseTriangle2", EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("BaseTriangle2", EShaderStage_Fragment, fragmentLayout);
    */

    /*
    //
    ShaderLayoutDef vertexLayout;
    vertexLayout.m_bindings.push_back(EShaderBinding_Uniform);
    vertexLayout.m_uniforms.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    vertexLayout.m_uniforms.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    vertexLayout.m_uniforms.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    ShaderLayoutDef fragmentLayout;
    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("BaseTriangle3", EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("BaseTriangle3", EShaderStage_Fragment, fragmentLayout);
   */

    
    ShaderLayoutDef vertexLayout;
    vertexLayout.m_bindings.push_back(EShaderBinding_Uniform);
    vertexLayout.m_uniforms.push_back(ION_MODEL_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    vertexLayout.m_uniforms.push_back(ION_VIEW_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    vertexLayout.m_uniforms.push_back(ION_PROJ_MATRIX_PARAM_TEXT);
    vertexLayout.m_uniformTypes.push_back(EUniformParameterType_Matrix);
    ShaderLayoutDef fragmentLayout;
    fragmentLayout.m_bindings.push_back(EShaderBinding_Sampler);
    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader("BaseTriangleTextured", EShaderStage_Vertex, vertexLayout);
    ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader("BaseTriangleTextured", EShaderStage_Fragment, fragmentLayout);

    eosString textureDVA = demoPath;
    textureDVA.append("dva.png");
    ionTextureManger().CreateTextureFromFile(ionRenderManager().GetRenderer().GetDevice(), "dva", textureDVA, ETextureFilter_Nearest, ETextureRepeat_Clamp, ETextureUsage_RGBA, ETextureType_2D);
    
    


    //
    camera->AttachToParent(*root);
    test->AttachToParent(*root);

    //
    ionRenderManager().AddScene(*root);

    if (rendererInitialized)
    {
        window.Loop();
    }

    ionRenderManager().Shutdown();

    ION_SCOPE_END
        
    ShutdownManagers();
    ShutdownVulkanAllocators();
    ShutdownAllocators();

    getchar();

    return 0;
}

