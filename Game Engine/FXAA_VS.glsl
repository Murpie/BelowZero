#version 440 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 inverseScreenSize;

uniform int width;
uniform int height;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0, 1);
	inverseScreenSize = vec2(1.0 / width, 1.0 / height);
	TexCoords = aTexCoords;
}