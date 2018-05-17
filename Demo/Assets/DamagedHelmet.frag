#version 450
#pragma shader_stage( fragment )

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inTexCoord;
layout(location = 2) in vec4 inNormal;

layout(location = 0) out vec4 outColor;

void main() 
{
	vec4 normal = normalize(inNormal);
	vec3 viewPos = vec3(0.0, -1.0, 0.0);
	vec3 viewDir = normalize(viewPos - inPosition);
	vec3 ambientLight = vec3(0.1, 0.1, 0.1);
	vec3 lightColor = vec3(0.5, 0.5, 0.5);
	vec4 lightDir = normalize(vec4(0.75, 0.75, 0.75, 1.0f));
	float ndl = max(0.0, dot(normal, lightDir));
	vec3 diffuse = lightColor * ndl;
    outColor = vec4((ambientLight + diffuse), 1.0);
}