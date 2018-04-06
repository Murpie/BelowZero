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

	vec3 drColor = vec3(0.9f, 1.0f, 0.84f);
	vec3 drPosition = vec3(0.0f, 200.0f, 400.0f);


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

vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);

// ----------========== DIRECTIONAL LIGHT SHADOW CALCULATION ==========----------
float DirectionalShadowMapCalculation(vec3 FragPos, vec3 Normal, vec3 lightPos)
{
	vec3 lightDirForShadow = normalize(vec3(0.0, 7.0, 0.0) - FragPos);
	vec4 shadowCoordinates = LightSpaceMatrix * vec4(FragPos, 1.0);
	vec3 projectionCoordinates = shadowCoordinates.xyz / shadowCoordinates.w;
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	float closestDepth = texture(depthMap, projectionCoordinates.xy).r;
	float bias = max(0.00005 * (1.0 - dot(Normal, lightDirForShadow)), 0.005);
	float directionalLightshadowFactor = 0.0f;

	//Test
	/*vec3 pos = FragPos + (Normal * 0.4f);
	vec2 pos2 = (0.5f * pos.xy) / (pos.w + vec2(0.5f, 0.5f));
	pos2.y = 1.0f - pos2.y;
	float b = 1.0f - texture(pos2, (1.0f - (pos.z / pos.w)) + 0.005f);*/
	//Test


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
float PointLightShadowMapCalculation(vec3 FragPos, vec3 Normal, vec3 lightPosition)
{
	//float cubeMapClosestDepth = texture(cubeMapdepthMap, fragPositionToLightPosition).r;
	//cubeMapClosestDepth *= 25.0f; //Far Plane
	//float cubeMapBias = max(0.005 * (1.0 - dot(Normal, lightDirForShadow)), 0.005);
	//float CubeMapShadowFactor = cubeMapCurrentDepth - cubeMapBias > cubeMapClosestDepth ? 0.5 : 0.0;
	float far_plane = 25.0;
	vec3 fragPositionToLightPosition = FragPos - lightPosition;
	float cubeMapCurrentDepth = length(fragPositionToLightPosition);
	float cubeMapBias = 0.15;
	float cubeMapShadowFactor = 0.0f;
	float cubeMapSamples = 20;

	float viewDistance = length(view_position - FragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	for (int i = 0; i < cubeMapSamples; ++i)
	{
		float closestDepth = texture(cubeMapdepthMap, fragPositionToLightPosition + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= far_plane;   // undo mapping [0;1]
		if (cubeMapCurrentDepth - cubeMapBias > closestDepth)
			cubeMapShadowFactor += 0.5;
	}
	cubeMapShadowFactor /= float(cubeMapSamples);

	return cubeMapShadowFactor;
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

	float shadowFactor = 0.0f;

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

	//Test Directional Light
	    vec3 lightDir = normalize(drPosition - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * drColor * AmbientOcclusion;

		vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = drColor * spec * Specular;
		vec3 metallic = drColor * spec * Metallic;
        // attenuation
        float distance = length(drPosition - FragPos);
        float attenuation = 1.0;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular + metallic;



	float density = 0.12;
	float gradient = 1.5;
	float distanceToPos = length(view_position - FragPos);
    float visibility = exp(-pow((distanceToPos * density), gradient));
	visibility  = clamp(visibility, 0.0, 1.0);

	FragColor = lighting;
	FragColor = mix(vec3(0.749, 0.843, 0.823), FragColor, visibility);

	
	if (lights[1].lightType == 0)
		shadowFactor = DirectionalShadowMapCalculation(FragPos, Normal, lights[1].Position);
	//if (lights[2].lightType == 1)
	//	shadowFactor = PointLightShadowMapCalculation(FragPos, Normal, lights[2].Position);
	

	FragColor = lighting * (1.0f - shadowFactor);
}