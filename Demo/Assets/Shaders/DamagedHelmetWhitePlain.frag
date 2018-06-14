#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;		// not used

layout(location = 0) out vec4 outColor;

void main() 
{
	vec4 normal = normalize(inNormal);
	vec4 ambientLight = vec4(0.3, 0.3, 0.3, 1.0);
	vec4 lightColor = vec4(0.8, 0.8, 0.8, 1.0);
	vec4 lightDir = normalize(vec4(0.85, 0.8, 0.75, 1.0));

	float normalDirLigh = max(0.0, dot(normal, lightDir));
	vec4 diffuse = lightColor * normalDirLigh;
	
	outColor = vec4(ambientLight + diffuse);
}