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



layout(binding = 2) readonly buffer MorphTargets 
{
	vec4 position[MAX_WEIGHTS];
	vec4 normal[MAX_WEIGHTS];
	vec4 tangent[MAX_WEIGHTS];
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
	vec4 morphPos = inPosition;
	vec4 morphNormal = inNormal;
	vec4 morphTangent = inTangent;
	
	for(int i = 0; i < MAX_WEIGHTS; ++i)
	{
		morphPos += uboMorphTargets.weights[i] * morphTargets.position[i];
		morphNormal += uboMorphTargets.weights[i] * morphTargets.normal[i];
		morphTangent += uboMorphTargets.weights[i] * morphTargets.tangent[i];
	}
	
	vec3 locPos = vec3(ubo.model * morphPos);
	outWorldPos = locPos;
	outNormal = normalize(morphNormal.xyz);
	outUV = inTexCoord0;
	outColor = inColor;
	gl_Position =  ubo.proj * ubo.view * vec4(outWorldPos, 1.0);
}