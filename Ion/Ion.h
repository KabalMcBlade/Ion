// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ION_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ION_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.


#include "Dependencies/Eos/Eos/Eos.h"
#include "Dependencies/vkMemoryAllocator/vkMemoryAllocator/vkMemoryAllocator.h"
#include "Dependencies/Nix/Nix/Nix.h"

#include "Dependencies/Miscellaneous/stb_image.h"
#include "Dependencies/Miscellaneous/json.hpp"
#include "Dependencies/Miscellaneous/tiny_gltf.h"

#include "Core/StandardIncludes.h"
#include "Core/CoreDefs.h"

#include "Renderer/GPUMemoryManager.h"
#include "Renderer/StagingBufferManager.h"
#include "Renderer/VertexCacheManager.h"

#include "Texture/TextureCommon.h"
#include "Texture/TextureOptions.h"
#include "Texture/Texture.h"
#include "Texture/TextureManager.h"

#include "Renderer/RenderDefs.h"
#include "Renderer/RenderCommon.h"
#include "Renderer/GPU.h"
#include "Renderer/RenderCore.h"
#include "Renderer/BaseBufferObject.h"
#include "Renderer/VertexBufferObject.h"
#include "Renderer/IndexBufferObject.h"
#include "Renderer/UniformBufferObject.h"
#include "Renderer/ShaderProgramHelper.h"
#include "Renderer/ShaderProgram.h"
#include "Renderer/ShaderProgramManager.h"
#include "Renderer/RenderManager.h"

#include "Scene/Transform.h"
#include "Scene/Node.h"
#include "Scene/BaseCamera.h"

#include "Wrapper/Memory.h"

#include "App/Window.h"

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