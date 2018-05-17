#version 450
#pragma shader_stage( vertex )

#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UBO
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outTexCoord;
layout(location = 2) out vec4 outNormal;

void main() 
{
	gl_Position = proj * view * model * inPosition;
	outPosition = gl_Position.xyz;
	mat4 normalMatrix = transpose(inverse(model));
	outNormal = normalMatrix * inNormal;
	outTexCoord = vec3(inTexCoord, 1.0f);
}