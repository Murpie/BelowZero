#version 440 core
layout(location = 0) out vec4 color;

in vec2 texCoords;

uniform sampler2D theTexture;
uniform sampler2D SceneTexture;

void main()
{
	//color = vec4(1.0, 0.5, 0.5, 0.0);
	if (texture(theTexture, texCoords).w < 0.01)
	{
		color = texture(SceneTexture, texCoords);
	}
	else
		color = texture(theTexture, texCoords);
}