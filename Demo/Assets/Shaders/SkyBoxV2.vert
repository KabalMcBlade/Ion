#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform UBO 
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) in vec4 inPosition;

layout (location = 0) out vec3 outUVW;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	outUVW = inPosition.xyz;
	gl_Position = ubo.proj * ubo.model * inPosition;
}