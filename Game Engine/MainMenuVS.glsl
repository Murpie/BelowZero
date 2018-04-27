#version 440 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoords;
uniform int buttonTransformation;

void main()
{
	TexCoords = aTexCoord;

	if (buttonTransformation == 1)
		gl_Position = vec4(aPos.x, aPos.y + 0.2, 0.0, 1.0);
	else if (buttonTransformation == 2)
		gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	else if (buttonTransformation == 3)
		gl_Position = vec4(aPos.x, aPos.y - 0.2, 0.0, 1.0);
	else if (buttonTransformation == 0)
		gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
