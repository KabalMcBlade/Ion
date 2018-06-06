#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inNormal;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec4 outNormal;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() 
{
	gl_Position = ubo.proj * ubo.view * ubo.model * inPosition;
	outPosition = gl_Position;
	mat4 normalMatrix = transpose(inverse(ubo.model));
	outNormal = normalMatrix * inNormal;
	outTexCoord = inTexCoord;
}