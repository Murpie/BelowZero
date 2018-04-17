#version 440 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSpecular;
layout (location = 4) out vec3 gMetallic;
layout (location = 5) out vec3 gAO;


in vec3 normal;
in vec2 uv;
in vec3 fragpos;

uniform sampler2D gAlbedoIn;
uniform sampler2D gNormalIn;
uniform sampler2D gSpecularIn;
uniform sampler2D gMetallicIn;
uniform sampler2D gAOIn;

uniform int foundAlbedo;
uniform int foundNormal;
uniform int foundSpecular;
uniform int foundMetallic;
uniform int foundAO;

vec3 norm;
void main () 
{
	//Normal
	gPosition = fragpos.xyz;

	//Albedo
	if (foundAlbedo == 1)
	{
		gAlbedo = texture(gAlbedoIn, uv).xyz;
	}
	else
	{
		gAlbedo = vec3(1.0, 0.0, 0.0);
	}

	//Normal
	if (foundNormal == 1)
	{
		norm = texture(gNormalIn, uv).rgb;
	}
	else
	{
		norm = vec3(0.5, 0.5, 1);
		norm = normal;
	}

	norm = normalize(norm * 2.0 - 1.0);
	gNormal = norm;

	//Specular
	if (foundSpecular == 1)
	{
		gSpecular = texture(gSpecularIn, uv).xyz;
	}
	else
	{
		gSpecular = vec3(0, 0, 0);
	}

	//Metallic
	if (foundMetallic == 1)
	{
		gMetallic = texture(gMetallicIn, uv).rgb;
	}
	else
	{
		gMetallic = vec3(0, 0, 0);
	}
	
	//AO
	if (foundAO == 1)
	{
		gAO = texture(gAOIn, uv).rgb;
	}
	else
	{
		gAO = vec3(1, 1, 1);
	}
}