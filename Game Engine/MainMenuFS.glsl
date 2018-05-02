#version 440 core
out vec4 FragColor;

in vec2 TexCoords;
vec4 color;

uniform sampler2D textureToUse;
uniform float scaling1;
uniform float scaling2;
uniform float scaling3;
float newScale;

void main()
{
	if (scaling1 > 1.0)
		newScale = scaling1 / 10; // Test to see if it it able to change color
	if (scaling2 > 1.0)
		newScale = scaling2 / 10;
	if (scaling3 > 1.0)
		newScale = scaling3 / 10;
	
	color = texture(textureToUse, TexCoords);
	if (color.a < 0.1)
		discard;


	/*if (scaling1 > 1.0)
	{
		newScale = scaling1 / 10;
		FragColor = vec4(color.xyz + newScale, color.w);
	}*/
	else
		FragColor = color;
}