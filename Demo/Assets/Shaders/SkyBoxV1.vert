#version 450

#extension GL_ARB_separate_shader_objects : enable

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
	mat4 modelView = ubo.view * ubo.model;
	vec3 position = mat3(modelView) * inPosition.xyz;
	
	outUVW = inPosition.xyz;
	
	gl_Position = (ubo.proj * vec4(position, 0.0)).xyzz;
}