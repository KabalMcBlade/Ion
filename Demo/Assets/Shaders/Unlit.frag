#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec4 inColor;

layout (binding = 1) uniform UBOParams 
{
	vec4 directionalLightColor;
} uboParams;

layout (binding = 2) uniform sampler2D albedoMap;


layout (push_constant) uniform Material {
	float baseColorFactorR;
	float baseColorFactorG;
	float baseColorFactorB;
	float baseColorFactorA;
	float hasBaseColorTexture;
	float hasNormalTexture;		
	float alphaMask;	
	float alphaMaskCutoff;
} material;

layout (location = 0) out vec4 outColor;


void main()
{
	if (material.alphaMask == 1.0 && material.hasBaseColorTexture == 1.0)
	{
		if (texture(albedoMap, inUV).a < material.alphaMaskCutoff)
		{
			discard;
		}
	}
	
	const float alpha = texture(albedoMap, inUV).a;
	
	const vec4 baseColorFactor = vec4(material.baseColorFactorR, material.baseColorFactorG, material.baseColorFactorB, material.baseColorFactorA);
	
	const vec3 norm = normalize(inNormal);
	
	vec3 diffuse;
	vec3 ambient;
	if (material.hasBaseColorTexture == 1.0) 
	{
		const vec3 baseColor = texture(albedoMap, inUV).rgb * baseColorFactor.rgb;
		ambient = uboParams.directionalLightColor.rgb * baseColor;
		
		diffuse = baseColor * texture(albedoMap, inUV).rgb;
	} 
	else 
	{
		const vec3 baseColor = inColor.rgb * baseColorFactor.rgb;
		ambient = uboParams.directionalLightColor.rgb * baseColor;
		
		diffuse = baseColor * inColor.rgb;
	}
	
    vec3 result = ambient + diffuse;
    outColor = vec4(result, alpha);
}