#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord0;
layout(location = 2) in vec2 inTexCoord1;
layout(location = 3) in vec4 inJoints;
layout(location = 4) in vec4 inNormal;
layout(location = 5) in vec4 inTangent;
layout(location = 6) in vec4 inColor;
layout(location = 7) in vec4 inWeights;

layout (binding = 0) uniform UBO 
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


#define MAX_WEIGHTS 8

layout(binding = 1) uniform UBOMorphTargets
 {
	float weights[MAX_WEIGHTS];
} uboMorphTargets;



struct SMorphTarget
{
	vec4 position;
	vec4 normal;
	vec4 tangent;
};


layout(binding = 2) readonly buffer MorphTargets 
{
	SMorphTarget data[];
} morphTargets;



layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec4 outColor;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
    vec4 inMorphPosition = inPosition;
	vec4 inMorphNormal = inNormal;
	vec4 inMorphTangent = inTangent;
	
	for(uint i = 0; i < MAX_WEIGHTS; ++i)
	{
		inMorphPosition += morphTargets.data[gl_VertexIndex].position * uboMorphTargets.weights[i];
		inMorphNormal += morphTargets.data[gl_VertexIndex].normal * uboMorphTargets.weights[i];
		inMorphTangent += morphTargets.data[gl_VertexIndex].tangent * uboMorphTargets.weights[i];
	}

	vec3 locPos = vec3(ubo.model * inMorphPosition);
	outWorldPos = locPos;
	outNormal = normalize(inMorphNormal.xyz);
	outUV = inTexCoord0;
	outColor = inColor;
	gl_Position =  ubo.proj * ubo.view * vec4(outWorldPos, 1.0);
}