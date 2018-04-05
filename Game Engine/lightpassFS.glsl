#version 440 core
layout(location = 0) out vec3 FragColor;
  
in vec2 TexCoords;
uniform vec3 view_position;

struct Light {
    vec3 Position;
    vec3 Color;
	int lightType;
	float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gSpecular;
uniform sampler2D gMetallic;
uniform sampler2D gAO;
uniform sampler2D ssao;
uniform sampler2D depthMap;
uniform samplerCube cubeMapdepthMap;
uniform mat4 LightSpaceMatrix;

// ----------========== DIRECTIONAL LIGHT SHADOW CALCULATION ==========----------
float DirectionalShadowMapCalculation(vec3 FragPos, vec3 Normal)
{
	vec3 lightDirForShadow = normalize(vec3(0.0, 7.0, 0.0) - FragPos);
	vec4 shadowCoordinates = LightSpaceMatrix * vec4(FragPos, 1.0);
	vec3 projectionCoordinates = shadowCoordinates.xyz / shadowCoordinates.w;
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	float closestDepth = texture(depthMap, projectionCoordinates.xy).r;
	float bias = max(0.005 * (1.0 - dot(Normal, lightDirForShadow)), 0.005);
	float directionalLightshadowFactor = 0.0f;

	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r;
			directionalLightshadowFactor += projectionCoordinates.z - bias > pcfDepth ? 0.7 : 0.0;
		}
	}
	directionalLightshadowFactor /= 9.0;

	if (projectionCoordinates.z > 1.0)
		directionalLightshadowFactor = 0.0;

	return directionalLightshadowFactor;
}

// ----------========== POINT LIGHT SHADOW CALCULATION ==========----------
float PointLightShadowMapCalculation(vec3 FragPos, vec3 Normal)
{
	vec3 fragPositionToLightPosition = FragPos - lights[1].Position;
	float cubeMapClosestDepth = texture(cubeMapdepthMap, fragPositionToLightPosition).r;
	cubeMapClosestDepth *= 25.0f; //Far Plane
	float cubeMapCurrentDepth = length(fragPositionToLightPosition);
	//float cubeMapBias = max(0.005 * (1.0 - dot(Normal, lightDirForShadow)), 0.005);
	float cubeMapBias = 0.005;
	float CubeMapShadowFactor = cubeMapCurrentDepth - cubeMapBias > cubeMapClosestDepth ? 0.5 : 0.0;

	return CubeMapShadowFactor;
}

void main()
{ 
	// retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 Specular = texture(gSpecular, TexCoords).rgb;
	vec3 Metallic = texture(gMetallic, TexCoords).rgb;
	vec3 AO = texture(gAO, TexCoords).rgb;
	float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    // then calculate lighting as usual
    vec3 lighting = vec3(0.0 * Albedo * AmbientOcclusion * AO);
    vec3 viewDir = normalize(view_position + 1 - FragPos);

    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * lights[i].Color * AmbientOcclusion;

		vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
		vec3 metallic = lights[i].Color * spec * Metallic;
        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular + metallic;
    }


	float shadowFactor = 0.0f;
	//if (lights[1].lightType == 0)
	//shadowFactor = DirectionalShadowMapCalculation(FragPos, Normal);
	//else if (lights[1].lightType == 1)
	shadowFactor = PointLightShadowMapCalculation(FragPos, Normal);

	FragColor = lighting * (1.0f - shadowFactor);
}