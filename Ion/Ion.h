// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ION_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ION_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef ION_EXPORTS
#define ION_API __declspec(dllexport)
#else
#define ION_API __declspec(dllimport)
#endif


#include "Dependencies/Eos/Eos/Eos.h"
#include "Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"
#include "Dependencies/Nix/Nix/Nix.h"

#include "App/VulkanWrapper.h"
#include "App/Window.h"

#include "Wrapper/Memory.h"


ION_NAMESPACE_BEGIN

// External Linkage Class 
class ION_DLL Linkage
{
private:
    ionS32 m_iCounterLinks;

    void IncrementLinkage();

public:
    Linkage();
};

ION_NAMESPACE_END