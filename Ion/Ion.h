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
#include "Dependencies/Miscellaneous/stb_image_write.h"
#include "Dependencies/Miscellaneous/json.hpp"
#include "Dependencies/Miscellaneous/tiny_gltf.h"

#include "Core/StandardIncludes.h"
#include "Core/CoreDefs.h"
#include "Core/FileSystemManager.h"
#include "Core/UUID.h"

#include "Renderer/GPUMemoryManager.h"
#include "Renderer/StagingBufferManager.h"
#include "Renderer/VertexCacheManager.h"
#include "Renderer/RenderState.h"

#include "Texture/TextureCommon.h"
#include "Texture/Texture.h"
#include "Texture/TextureManager.h"
#include "Texture/CubemapHelper.h"

#include "Material/MaterialState.h"
#include "Material/Material.h"
#include "Material/MaterialManager.h"

#include "Renderer/RenderDefs.h"
#include "Renderer/RenderCommon.h"
#include "Renderer/GPU.h"
#include "Renderer/RenderCore.h"
#include "Renderer/BaseBufferObject.h"
#include "Renderer/VertexBufferObject.h"
#include "Renderer/IndexBufferObject.h"
#include "Renderer/UniformBufferObject.h"
#include "Renderer/StorageBufferObject.h"
#include "Renderer/RenderManager.h"

#include "Shader/ShaderProgramHelper.h"
#include "Shader/ShaderProgram.h"
#include "Shader/ShaderProgramManager.h"

#include "Material/Material.h"

#include "Geometry/Ray.h"
#include "Geometry/BoundingBox.h"
#include "Geometry/Mesh.h"
#include "Geometry/MeshRenderer.h"
#include "Geometry/Intersection.h"
#include "Geometry/Frustum.h"
#include "Geometry/PrimitiveFactory.h"

#include "Animation/Animation.h"
#include "Animation/AnimationRenderer.h"

#include "Scene/Transform.h"
#include "Scene/Node.h"
#include "Scene/Entity.h"
#include "Scene/Skybox.h"
#include "Scene/Camera.h"
#include "Scene/DirectionalLight.h"
#include "Scene/SceneGraph.h"

#include "Utilities/LoaderGLTF.h"
#include "Utilities/GeometryHelper.h"
#include "Utilities/Serializer.h"

#include "Wrapper/Memory.h"

#include "App/Mode.h"
#include "App/CommandLineParser.h"
#include "App/Window.h"
