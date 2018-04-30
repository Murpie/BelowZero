#version 440 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoords;
uniform int buttonTransformation;
uniform float scaling1;
uniform float scaling2;
uniform float scaling3;

vec4 finalPosition;

void main()
{
	TexCoords = aTexCoord;

	if (buttonTransformation == 1)
		finalPosition = vec4(aPos.x * scaling1, (aPos.y + 0.2) * scaling1, 0.0, 1.0);
	else if (buttonTransformation == 2)
		finalPosition = vec4(aPos.x * scaling2, (aPos.y - 0.1) * scaling2, 0.0, 1.0);
	else if (buttonTransformation == 3)
		finalPosition = vec4(aPos.x * scaling3, (aPos.y - 0.4) * scaling3, 0.0, 1.0);
	else if (buttonTransformation == 0)
		finalPosition = vec4(aPos.x, aPos.y, 0.0, 1.0);

	
	gl_Position = finalPosition;
}
