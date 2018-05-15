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


#define DEMO_WIDTH 640
#define DEMO_HEIGHT 480



EOS_USING_NAMESPACE
VK_ALLOCATOR_USING_NAMESPACE
ION_USING_NAMESPACE

EOS_OPTIMIZATION_OFF
ION_OPTIMIZATION_OFF


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            //PostQuitMessage(0);
            PostMessage(hWnd, WM_USER + 1, wParam, lParam);
            break;
        }
    case WM_CLOSE:
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

    ionBool rendererInitialized = false;
    Window window;

    ionTextureManger().Init(ion::ETextureSamplesPerBit_16); // init AFTER render core (not important)

    if (window.Create(WndProc, L"Ion Demo", DEMO_WIDTH, DEMO_HEIGHT, false))
    {
        rendererInitialized = ionRenderManager().Init(window.GetInstance(), window.GetHandle(), DEMO_WIDTH, DEMO_HEIGHT, false, ION_VULKAN_VALIDATION_LAYER, "Shaders\\", VULKAN_GPU_DEVICE_LOCAL_MB, VULKAN_GPU_HOST_VISIBLE_MB, VULKAN_STAGING_BUFFER_MB);
    }


    Vector cameraPos(0.0f, 0.0f, -10.0f);
    Quaternion cameraRot;   // no rot

    Vector directLightPos(0.0f, 100.0f, 0.0f);  // just to have something related to a "gizmo" in scene, even if is not yet implemented
    Quaternion directLightRot(NIX_DEG_TO_RAD(180.0f), Vector(1.0f, 0.0f, 0.0f));
    Vector directLightCol(0.2f, 0.1f, 0.1f, 1.0f);

    Vector entityPos(0.0f, 20.0f, 0.0f);
    Quaternion entityRot;   // no rot

    //
    Entity *pRoot = eosNew(Entity, EOS_MEMORY_ALIGNMENT_SIZE);
    EntityHandle root(pRoot);  // set to 0 all

    //
    DirectionalLight *pLight = eosNew(DirectionalLight, EOS_MEMORY_ALIGNMENT_SIZE);
    DirectionalLightHandle directLight(pLight);
    directLight->SetColor(directLightCol);
    directLight->SetPercentageCloserFilteringSize(3);
    directLight->GetTransformHandle()->SetPosition(directLightPos);
    directLight->GetTransformHandle()->SetRotation(directLightCol);

    //
    BaseCamera *pBaseCamera = eosNew(BaseCamera, EOS_MEMORY_ALIGNMENT_SIZE);
    BaseCameraHandle camera(pBaseCamera);
    camera->SetCameraType(BaseCamera::ECameraType::ECameraType_FirstPerson);
    camera->SetPerspectiveProjection(90.0f, (ionFloat)DEMO_WIDTH / (ionFloat)DEMO_HEIGHT, 0.1f, 100.0f);
    camera->GetTransformHandle()->SetPosition(cameraPos);
    camera->GetTransformHandle()->SetRotation(cameraRot);

    //
    Entity *pTest = eosNew(Entity, EOS_MEMORY_ALIGNMENT_SIZE);
    EntityHandle test(pTest);
    ionRenderManager().LoadModelFromFile("E:/Projects/Ion/Demo/Assets/DamagedHelmet.gltf", *test);
	//ionRenderManager().LoadModelFromFile("C:/Projects/Ion/Demo/Assets/DamagedHelmet.gltf", test);

    test->GetTransformHandle()->SetPosition(entityPos);
    test->GetTransformHandle()->SetRotation(entityRot);

    directLight->AttachToParent(*root);
    camera->AttachToParent(*root);
    test->AttachToParent(*root);

    ionRenderManager().AddScene(*root);

    if (rendererInitialized)
    {
        window.Loop();
    }

    ionTextureManger().Shutdown();  // Shutdown BEFORE render core (VERY IMPORTANT)

    ionRenderManager().Shutdown();

    ION_SCOPE_END
        
    ShutdownManagers();
    ShutdownVulkanAllocators();
    ShutdownAllocators();

    getchar();

    return 0;
}

