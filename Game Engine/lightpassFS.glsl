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
uniform sampler2D depthMap;
uniform mat4 LightSpaceMatrix;

vec3 drColor = vec3(0.9f, 1.0f, 0.84f);
vec3 drPosition = vec3(lights[0].Position);

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
	vec3 lightDirForShadow = normalize(vec3(lightPos) - FragPos);
	vec4 shadowCoordinates = LightSpaceMatrix * vec4(FragPos, 1.0);
	//vec4 shadowCoordinates = LightSpaceMatrix * (vec4(FragPos, 1.0) + (vec4(Normal, 1.0) * 0.03));
	vec3 projectionCoordinates = shadowCoordinates.xyz / shadowCoordinates.w;
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	float closestDepth = texture(depthMap, projectionCoordinates.xy).r;
	float directionalLightshadowFactor = 0.0f;
	//float bias = max(0.0005 * (1.0 - dot(Normal, lightDirForShadow)), 0.05);
	float bias = 0.005;
	directionalLightshadowFactor += projectionCoordinates.z - bias > closestDepth ? 0.35 : 0.0;
	
	/*vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r;
			directionalLightshadowFactor += projectionCoordinates.z - bias > pcfDepth ? 0.7 : 0.0;
		}
	}
	directionalLightshadowFactor /= 9.0;*/

	if (projectionCoordinates.z > 1.0)
		directionalLightshadowFactor = 0.0;

	return directionalLightshadowFactor;
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
    
    // then calculate lighting as usual
    vec3 lighting = vec3(0.0 * Albedo * AO);
    vec3 viewDir = normalize(view_position - FragPos);

	float shadowFactor = 0.0f;

    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * lights[i].Color;

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
	    vec3 lightDir = normalize(drPosition - vec3(0.0, 0.0, 0.0));
        vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * drColor;

		vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = drColor * spec * Specular;
		vec3 metallic = drColor * spec * Metallic;
        // attenuation
        float distance = length(drPosition - vec3(0.0, 0.0, 0.0));
        float attenuation = 1.0;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular + metallic;

	float density = 0.05;
	float gradient = 3.0;
	float distanceToPos = length(view_position - FragPos);
    float visibility = exp(-pow((distanceToPos * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	if (lights[1].lightType == 0)
		shadowFactor = DirectionalShadowMapCalculation(FragPos, Normal, lights[1].Position);

	FragColor = lighting * (1.0f - shadowFactor);
	FragColor = mix(vec3(0.749, 0.843, 0.823), FragColor, visibility);
}