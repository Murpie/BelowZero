#version 440 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform samplerCube skybox;

uniform vec3 cameraPos;

void main()
{
	vec3 I = normalize(Position - cameraPos); //  Approach vector
	vec3 R = reflect(I, normalize(Normal)); // Reflection vector
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}