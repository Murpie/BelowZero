#version 440 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSpecular;
layout (location = 4) out vec3 gMetallic;
layout (location = 5) out vec3 gAO;

in GS_OUT {
    vec4 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D gAlbedoIn;
uniform sampler2D gNormalIn;
uniform sampler2D gSpecularIn;
uniform sampler2D gMetallicIn;
uniform sampler2D gAOIn;

uniform mat3 mtlInfo;

uniform int foundAlbedo;
uniform int foundNormal;
uniform int foundSpecular;
uniform int foundMetallic;
uniform int foundAO;

vec3 norm;

void main () 
{
	//Normal
	gPosition = fs_in.FragPos.xyz;

	//Albedo
	if (foundAlbedo == 1)
	{
		gAlbedo = texture(gAlbedoIn, fs_in.TexCoords).xyz;
	}
	else if (foundAlbedo == 2)
	{
		gAlbedo = vec3(mtlInfo[0].x, mtlInfo[0].y, mtlInfo[0].z);
	}
	else
	{
		gAlbedo = vec3(1.0, 0.0, 0.0);
	}

	//Normal
	if (foundNormal == 1)
	{
		norm = texture(gNormalIn, fs_in.TexCoords).rgb;
	}
	else
	{
		norm = vec3(0.5, 0.5, 1);
	}

	norm = normalize(norm * 2.0 - 1.0);
	gNormal = fs_in.TBN*norm;

	//Specular
	if (foundSpecular == 1)
	{
		gSpecular = texture(gSpecularIn, fs_in.TexCoords).xyz;
	}
	else if (foundSpecular == 2)
	{
		gSpecular = vec3(mtlInfo[1].x, mtlInfo[1].y, mtlInfo[1].z);
	}
	else
	{
		gSpecular = vec3(0, 0, 0);
	}

	//Metallic
	if (foundMetallic == 1)
	{
		gMetallic = texture(gMetallicIn, fs_in.TexCoords).rgb;
	}
	else
	{
		gMetallic = vec3(0, 0, 0);
	}
	
	//AO
	if (foundAO == 1)
	{
		gAO = texture(gAOIn, fs_in.TexCoords).rgb;
	}
	else
	{
		gAO = vec3(1, 1, 1);
	}
}