#pragma once

#pragma once

#include "CoreDefs.h"


ION_NAMESPACE_BEGIN

#define ION_MEMORY_1_MB             (1u << 20u)
#define ION_MEMORY_2_MB             (1u << 21u)
#define ION_MEMORY_4_MB             (1u << 22u)
#define ION_MEMORY_8_MB             (1u << 23u)
#define ION_MEMORY_16_MB            (1u << 24u)
#define ION_MEMORY_32_MB            (1u << 25u)			// from this point on, the real size is bigger, but is a good approximation the define name given
#define ION_MEMORY_64_MB            (1u << 26u)
#define ION_MEMORY_128_MB           (1u << 27u)
#define ION_MEMORY_256_MB			(1u << 28u)
#define ION_MEMORY_512_MB           (1u << 29u)
#define ION_MEMORY_1024_MB          (1u << 30u)
#define ION_MEMORY_2048_MB          (1u << 31u)


namespace Settings
{
	static constexpr ionU32 kCommonAllocatorSize = ION_MEMORY_8_MB;
	static constexpr ionU32 kAnimationSampleAllocatorSize = ION_MEMORY_1_MB;
	static constexpr ionU32 kAnimationAllocatorSize = ION_MEMORY_1_MB;
	static constexpr ionU32 kAnimationRendererAllocatorSize = ION_MEMORY_1_MB;
	static constexpr ionU32 kAnimationChannelAllocatorSize = ION_MEMORY_1_MB;
	static constexpr ionU32 kBoundingBoxAllocatorSize = ION_MEMORY_1_MB;
	static constexpr ionU32 kMeshRendererAllocatorSize = ION_MEMORY_256_MB;
	static constexpr ionU32 kPrimitiveFactoryAllocatorSize = ION_MEMORY_2_MB;
	static constexpr ionU32 kGPUAllocatorSize = ION_MEMORY_2_MB;
	static constexpr ionU32 kShaderVertexLayoutAllocatorSize = ION_MEMORY_4_MB;
	static constexpr ionU32 kRenderCoreAllocatorSize = ION_MEMORY_16_MB;
	static constexpr ionU32 kCameraAllocatorSize = ION_MEMORY_2_MB;
	static constexpr ionU32 kNodeAllocatorSize = ION_MEMORY_8_MB;
	static constexpr ionU32 kEntityAllocatorSize = ION_MEMORY_4_MB;
	static constexpr ionU32 kSceneGraphAllocatorSize = ION_MEMORY_16_MB;
	static constexpr ionU32 kSkyBoxAllocatorSize = ION_MEMORY_2_MB;
	static constexpr ionU32 kShaderProgamAllocatorSize = ION_MEMORY_16_MB;
	static constexpr ionU32 kShaderProgamHelperAllocatorSize = ION_MEMORY_16_MB;
	static constexpr ionU32 kTextureAllocatorSize = ION_MEMORY_512_MB;
	static constexpr ionU32 kLoaderGLTFAllocatorSize = ION_MEMORY_512_MB;
	static constexpr ionU32 kSerializeAllocatorSize = ION_MEMORY_8_MB;
	static constexpr ionU32 kOptionAllocatorSize = ION_MEMORY_1_MB;
	static constexpr ionU32 kMaterialManagerAllocatorSize = ION_MEMORY_16_MB;
	static constexpr ionU32 kRenderManagerAllocatorSize = ION_MEMORY_8_MB;
	static constexpr ionU32 kCubeMapHelperAllocatorSize = ION_MEMORY_256_MB;
	static constexpr ionU32 kTextureManagerAllocatorSize = ION_MEMORY_4_MB;
	static constexpr ionU32 kGeometryHelperAllocatorSize = ION_MEMORY_64_MB;
	static constexpr ionU32 kShaderHelperAllocatorSize = ION_MEMORY_8_MB;

	// Vulkan specific
	static constexpr ionU32 kVulkanAllocatorSize = ION_MEMORY_16_MB;
	static constexpr ionU32 kGPUListNodeAllocatorSize = ION_MEMORY_8_MB;
}

ION_NAMESPACE_END