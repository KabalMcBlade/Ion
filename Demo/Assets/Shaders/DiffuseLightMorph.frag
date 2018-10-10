#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec4 inColor;


layout (binding = 3) uniform UBOParams 
{
	vec4 mainCameraPos;
	vec4 directionalLight;
	vec4 directionalLightColor;
} uboParams;


layout (binding = 4) uniform sampler2D albedoMap;
layout (binding = 5) uniform sampler2D normalMap;


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
	const vec3 lightDir = normalize(uboParams.directionalLight.xyz);
	
	vec3 diffuse;
	vec3 ambient;
	if (material.hasBaseColorTexture == 1.0) 
	{
		const vec3 baseColor = texture(albedoMap, inUV).rgb * baseColorFactor.rgb;
		ambient = uboParams.directionalLightColor.rgb * baseColor;
		
		const float diff = max(dot(norm, lightDir), 0.0);
		diffuse = baseColor * diff * texture(albedoMap, inUV).rgb;
	} 
	else 
	{
		const vec3 baseColor = inColor.rgb * baseColorFactor.rgb;
		ambient = uboParams.directionalLightColor.rgb * baseColor;
		
		const float diff = max(dot(norm, lightDir), 0.0);
		diffuse = baseColor * diff * inColor.rgb;
	}
	
	const vec3 viewDir = normalize(uboParams.mainCameraPos.xyz - inWorldPos);
	const vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 specular;	
	if (material.hasNormalTexture == 1.0f) 
	{
		specular = uboParams.directionalLightColor.rgb * spec * texture(normalMap, inUV).rgb;  
    }
	else
	{
		specular = uboParams.directionalLightColor.rgb * spec;  
	}
        
    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, alpha);
}