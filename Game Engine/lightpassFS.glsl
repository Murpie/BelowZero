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
uniform int nrOfLights;

uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D depthMap;
uniform mat4 LightSpaceMatrix;
uniform int ScreenX;
uniform int ScreenY;
uniform float water;

vec3 getBlur()
{
	const float offset = 1.0 / 50.0;

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

// ----------========== DIRECTIONAL LIGHT SHADOW CALCULATION ==========----------
float DirectionalShadowMapCalculation(vec3 FragPos, vec3 Normal, vec3 lightPos)
{
	vec3 lightDirForShadow = normalize(vec3(lightPos) - FragPos);
	vec4 shadowCoordinates = LightSpaceMatrix * vec4(FragPos, 1.0);
	vec3 projectionCoordinates = shadowCoordinates.xyz / shadowCoordinates.w;
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	float closestDepth = texture(depthMap, projectionCoordinates.xy).r;
	float directionalLightshadowFactor = 0.0f;
	float bias = 0.005;
	directionalLightshadowFactor += projectionCoordinates.z - bias > closestDepth ? 0.35 : 0.0;

	//float bias = max(0.0005 * (1.0 - dot(Normal, lightDirForShadow)), 0.005);
	//vec4 shadowCoordinates = LightSpaceMatrix * (vec4(FragPos, 1.0) + (vec4(Normal, 1.0) * 0.03));
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
	vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;

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

	float shadowFactor = 0.0f;

	for (int i = 0; i < nrOfLights; i++)
	{
		////dir
		//if (lights[i].lightType == 0) {
		//	// diffuse
		//	vec3 lightDir = normalize(lights[i].Position - FragPos);
		//	vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * lights[i].Color;
		//	//diffuse = Albedo;
		//
		//	vec3 halfwayDir = normalize(lightDir + viewDir);
		//	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
		//
		//	// attenuation
		//	float distance = length(lights[i].Position - FragPos);
		//	float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
		//	lighting += diffuse;
		//}

		//point
		if (lights[i].lightType == 1) {
			// diffuse
			vec3 lightDir = normalize(lights[i].Position - FragPos);
			float diffuse = max(dot(Normal, lightDir), 0.0);
			// attenuation
			float distance = length(lights[i].Position - FragPos);
			float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * (distance * distance));
			//float attenuation = 1.0 / ((distance * distance) / (lights[i].Linear * lights[i].Linear));

			vec3 diff = lights[i].Color * diffuse * Albedo;

			diff *= attenuation;

			lighting += diff * lights[i].intensity;
		}
	}

	//Directional Light
	// diffuse
	vec3 lightDir = normalize(vec3(7, 9, -5) - vec3(0.0, 0.0, 0.0));
	vec3 diffuse = max(dot(Normal, lightDir), 0.5) * Albedo * vec3(1, 1, 1);
	lighting += (diffuse * daylight);


	float density = 0.02;
	float gradient = 3.0;
	float distanceToPos = length(view_position - FragPos);
	float visibility = exp(-pow((distanceToPos * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);




	//FragColor = lighting;
	FragColor = mix(vec3(0.749, 0.843, 0.823) * daylight, lighting, visibility);
	//FragColor = mix(vec3(0.749, 0.843, 0.823), FragColor / 1.5);
}