#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inNormal;

layout(location = 0) out vec4 outColor;

void main() 
{
	vec4 diffuse = texture(texSampler, inTexCoord);

	vec4 normal = normalize(inNormal);
	vec4 viewDir = normalize(normal - inPosition);
	vec4 ambientLight = vec4(0.1, 0.1, 0.1, 1.0);
	vec4 lightColor = vec4(0.5, 0.5, 0.5, 1.0);
	vec4 lightDir = normalize(vec4(0.75, 0.75, 0.75, 1.0f));
	float ndl = max(0.0, dot(viewDir, lightDir));
	 
	diffuse *= lightColor * ndl;
	
    outColor = (ambientLight + diffuse);
}