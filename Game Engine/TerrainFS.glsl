#version 440 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSpecular;
layout (location = 4) out vec3 gMetallic;
layout (location = 5) out vec3 gAO;


in vec3 normal;
in vec2 uv;
in vec4 fragpos;

uniform sampler2D gAlbedoIn;

uniform int foundAlbedo;

void main () 
{
	//Normal
	gPosition = fs_in.FragPos.xyz;

	//Albedo
	//if (foundAlbedo == 1)
	//{
	//	gAlbedo = texture(gAlbedoIn, uv).xyz;
	//}
	//else
	//{
		gAlbedo = vec3(0.86, 0.94, 0.92);
	//}

	//Normal
		norm = vec3(0.5, 0.5, 1);
		norm = normal;

	norm = normalize(norm * 2.0 - 1.0);
	gNormal = fs_in.TBN*norm;

	//Specular
		gSpecular = vec3(0, 0, 0);


	//Metallic
		gMetallic = vec3(0, 0, 0);
	
	//AO
		gAO = vec3(1, 1, 1);
}