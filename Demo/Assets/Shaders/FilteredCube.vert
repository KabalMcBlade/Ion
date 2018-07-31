#version 450

layout (location = 0) in vec4 inPosition;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) out vec4 outUVW;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	outUVW = inPosition;
	gl_Position = ubo.proj * ubo.view * ubo.model * inPosition;
}
