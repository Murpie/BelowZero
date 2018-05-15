#version 440 core
layout(location = 0) out vec3 FragColor;

in vec2 TexCoords;
uniform vec3 view_position;
uniform vec3 player_position;

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
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform float cascadeEndClipSpace[];
uniform mat4 lightSpaceMatrix[];


vec4 lightSpacePosition[3];
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

void calculateLightSpacePositions(vec3 FragPos)
{
	for (int i = 0; i < 3; i++)
		lightSpacePosition[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0f);
}

float cascadedShadowMapCalculation(int cascadeIndex, vec4 lightSpacePos)
{
	float shadow = 0.0f;
	float depth = 0.0f;
	vec3 projectionCoordinates = lightSpacePos.xyz / lightSpacePos.w;

	vec2 UVCoords;
	UVCoords.x = 0.5 * projectionCoordinates.x + 0.5;
	UVCoords.y = 0.5 * projectionCoordinates.y + 0.5;
	float z = 0.5 * projectionCoordinates.z + 0.5;
	
	if (cascadeIndex == 0)
		depth = texture(shadowMap0, UVCoords.xy).x;
	if (cascadeIndex == 1)
		depth = texture(shadowMap1, UVCoords.xy).x;
	if (cascadeIndex == 2)
		depth = texture(shadowMap2, UVCoords.xy).x;

	if (depth < z + 0.0001)
		shadow = 0.4f;
	else
		shadow = 0.0f;

	return shadow;
}

void main()
{
	// retrieve data from G-buffer
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;

	// then calculate lighting as usual
	vec3 lighting = vec3(0.0, 0.0, 0.0);
	vec3 viewDir = normalize(view_position - FragPos);

	for (int i = 0; i < NR_LIGHTS; ++i)
	{
		// diffuse
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * lights[i].Color;
		//diffuse = Albedo;

		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);

		// attenuation
		float distance = length(lights[i].Position - FragPos);
		float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
		lighting += diffuse;
	}

	//Test Directional Light
	vec3 lightDir = normalize(drPosition - vec3(0.0, 0.0, 0.0));
	vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * drColor;

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
	// attenuation
	float distance = length(drPosition - vec3(0.0, 0.0, 0.0));
	float attenuation = 1.0;
	lighting += diffuse;

	float density = 0.02;
	float gradient = 3.0;
	float distanceToPos = length(view_position - FragPos);
	float visibility = exp(-pow((distanceToPos * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	
	// =========================== TESTING SHADOWS ==================================
	float shadowFactor = 0.0;

	calculateLightSpacePositions(FragPos);

	for (int i = 0; i < 3; i++)
	{
		if (FragPos.z <= cascadeEndClipSpace[i])
		{
			shadowFactor = cascadedShadowMapCalculation(i, lightSpacePosition[i]);
			break;
		}
	}


	FragColor = lighting * (1.0f - shadowFactor);
	FragColor = mix(vec3(0.749, 0.843, 0.823), FragColor / 1.5, visibility);

	//FragColor = lightSpacePosition[2].xyz;

	//FragColor = mix(vec3(0.749, 0.843, 0.823), FragColor / 1.5);
}