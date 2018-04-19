#version 440 core
layout (location = 0) out vec3 gPosition;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gNormal;

in GS_OUT {
	vec4 vertex_normal;
    vec4 FragPos;
    vec2 TexCoords;
} fs_in;

uniform sampler2D gAlbedoIn;

void main () 
{
	//Normal
	gPosition = fs_in.FragPos.xyz;

	gAlbedo = vec3(1.0, 0.0, 0.0);
	/*gAlbedo = texture(gAlbedoIn, fs_in.TexCoords).xyz;*/

	gNormal = normalize(fs_in.vertex_normal.xyz);
}