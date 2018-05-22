#version 440 core
layout(location = 0) out vec3 FragColor;

in vec2 TexCoords;

uniform vec3 view_position;

struct Light {
	vec3 Position;
	vec3 Color;
	int lightType;
	float intensity;
	float Linear;
	float Quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform float daylight;

uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform vec3 shadowMapLightPosition;
uniform float cascadeEndClipSpace[3];
uniform mat4 lightSpaceMatrix[3];
uniform mat4 viewMatrix;
uniform mat4 ProjectionMatrix;

uniform int ScreenX;
uniform int ScreenY;
uniform float water;

int selected = 0;
vec4 lightSpacePosition[3];
float clipSpacePosZ;

vec3 drColor = vec3(0.9f, 1.0f, 0.84f) * daylight;
vec3 drPosition = vec3(lights[0].Position);

vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);

vec3 getBlur()
{
	const float offset = 1.0 / 300.0;

	vec2 blurOffsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset, 0.0f),
		vec2(-offset, -offset),
		vec2(0.0f, -offset),
		vec2(offset, -offset)
		);

	float blurKernel[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
		);

	vec3 blurSampleTex[9];
	vec3 blurColor = vec3(0.0);

	for (int i = 0; i < 9; i++)
		blurSampleTex[i] = vec3(texture(gAlbedo, TexCoords + blurOffsets[i]));

	for (int i = 0; i < 9; i++)
		blurColor += blurSampleTex[i] * blurKernel[i];

	return blurColor;
}


void calculateLightSpacePositions(vec3 FragPos)
{
	for (int i = 0; i < 3; i++)
		lightSpacePosition[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0f);
}

float cascadedShadowMapCalculation(int cascadeIndex, vec4 lightSpacePos, vec3 normals, vec3 FragPos)
{
	float shadow = 0.0f;
	float depth = 0.0f;
	float z;
	vec3 lightDirForShadow = normalize(shadowMapLightPosition - FragPos);
	float bias = max(0.05 * (1.0 - dot(normals, lightDirForShadow)), 0.005);
	vec3 projectionCoordinates = lightSpacePos.xyz; // / lightSpacePos.w;

	vec2 UVCoords;
	UVCoords.x = 0.5 * projectionCoordinates.x + 0.5;
	UVCoords.y = 0.5 * projectionCoordinates.y + 0.5;
	z = 0.5 * projectionCoordinates.z + 0.5;
	
	if (cascadeIndex == 0)							// Check Which ShadowMap To Use
		depth = texture(shadowMap0, UVCoords).x;
	if (cascadeIndex == 1)
		depth = texture(shadowMap1, UVCoords).x;
	if (cascadeIndex == 2)
		depth = texture(shadowMap2, UVCoords).x;

	if (z - bias > depth) // Determine If There Shall Be Shadow
		shadow = 0.35f;
	else
		shadow = 0.0;
	
	if (depth > 1.0f) // If Shadow Is Outside of range 0-1 = No Shadow
		shadow = 0.0f;

	return shadow;
}

void main()
{
	// retrieve data from G-buffer
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec4 FragPosition = texture(gPosition, TexCoords);
	vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	clipSpacePosZ = (ProjectionMatrix * viewMatrix * FragPosition).z;


	float Thirst = water;
	float thirstVariable = 1.0;
	if (Thirst <= 50)
	{
		thirstVariable = Thirst / 100;
	}
	Albedo = mix(getBlur(), Albedo, thirstVariable);




	// then calculate lighting as usual
	vec3 lighting = vec3(0.0, 0.0, 0.0);
	vec3 viewDir = normalize(view_position - FragPos);

	for (int i = 0; i < NR_LIGHTS; ++i)
	{
		//dir
		if (lights[i].lightType == 0) {
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

		//point
		if (lights[i].lightType == 1) {
			// diffuse
			vec3 lightDir = normalize(lights[i].Position - FragPos);
			vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].Color;
			// attenuation
			float distance = length(lights[i].Position - FragPos);
			float attenuation = 1.0 / ((distance * distance) / (lights[i].Linear * lights[i].Linear));
			//diffuse *= attenuation;

			lighting += (diffuse * attenuation) * lights[i].intensity;
		}
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
		if (clipSpacePosZ <= cascadeEndClipSpace[i])//if (FragPos.z <= cascadeEndClipSpace[i]) // Check Which Cascade To Sample from
		{
			shadowFactor = cascadedShadowMapCalculation(i, lightSpacePosition[i], Normal, FragPos);
			selected = i;
			break;
		}
		else
		{
			selected = -1;
		}
	}

	FragColor = lighting * (1.0f - shadowFactor);
	//FragColor = mix(vec3(0.749, 0.843, 0.823) * daylight, FragColor / 1.5, visibility);

	//FragColor = mix(vec3(0.749, 0.843, 0.823), FragColor / 1.5);
	/*if (selected == 0)
		FragColor.xyz = FragColor.xyz + vec3(0.5, 0.0, 0.0);
	if (selected == 1)
		FragColor.xyz = FragColor.xyz + vec3(0.0, 0.5, 0.0);
	if (selected == 2)
		FragColor.xyz = FragColor.xyz + vec3(0.0, 0.0, 0.5);
	if (selected == -1)
		FragColor = FragColor;*/
}

