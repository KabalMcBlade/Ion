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

#define STL_MAX_HEAP_MEMORY         MEMORY_16_MB
#define STL_MAX_STACK_MEMORY_SIZE   MEMORY_4_MB
#define STL_MAX_LINEAR_MEMORY       MEMORY_8_MB


#define MAX_STACK_MEMORY_BLOCK      1024
#define ALL_HEAP_MEMORY             STL_MAX_HEAP_MEMORY + (MEMORY_16_MB)
#define ALL_LINEAR_MEMORY           STL_MAX_LINEAR_MEMORY + (MEMORY_16_MB * 2)
#define ALL_STACK_MEMORY            STL_MAX_STACK_MEMORY_SIZE


//////////////////////////////////////////////////////////////////////////
// VULKAN MEMORY

// just for sample and just to stress :D
#define MEMORY_1_MB                 1048576        
#define VULKAN_BASE_MEMORY_MB       MEMORY_1_MB

#define VULKAN_COMMAND_MEMORY_MB    VULKAN_BASE_MEMORY_MB * 64
#define VULKAN_OBJECT_MEMORY_MB     VULKAN_BASE_MEMORY_MB * 4
#define VULKAN_CACHE_MEMORY_MB      VULKAN_BASE_MEMORY_MB
#define VULKAN_DEVICE_MEMORY_MB     VULKAN_BASE_MEMORY_MB
#define VULKAN_INSTANCE_MEMORY_MB   VULKAN_BASE_MEMORY_MB * 16


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
    InitializeVulkanAllocators(VULKAN_COMMAND_MEMORY_MB, VULKAN_OBJECT_MEMORY_MB, VULKAN_CACHE_MEMORY_MB, VULKAN_DEVICE_MEMORY_MB, VULKAN_INSTANCE_MEMORY_MB);

    ION_SCOPE_BEGIN
        
    Window window;

    if (!window.Create(WndProc, L"Ion Demo"))
    {
        return -1;
    }

    if (!window.Loop())
    {
        return -1;
    }


    ION_SCOPE_END

    ShutdownVulkanAllocators();
    ShutdownAllocators();

    return 0;
}

