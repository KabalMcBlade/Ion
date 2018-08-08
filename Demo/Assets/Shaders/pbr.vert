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

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	vec3 locPos = vec3(ubo.model * inPosition);
	outWorldPos = locPos;
	outNormal = normalize(inNormal.xyz);
	outUV = inTexCoord0;
	gl_Position =  ubo.proj * ubo.view * vec4(outWorldPos, 1.0);
}