#version 440 core
out vec4 FragColor;

in vec2 TexCoords;
vec4 color;

uniform sampler2D textureToUse;

void main()
{

	color = texture(textureToUse, TexCoords);
	if (color.a < 0.1)
		discard;

	FragColor = color;
}