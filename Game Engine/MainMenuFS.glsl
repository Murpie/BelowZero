#version 440 core
out vec4 FragColor;

in vec2 TexCoords;
vec4 color;

uniform sampler2D backGroundTexture;
uniform sampler2D startButtonTexture;
uniform sampler2D settingsButtonTexture;
uniform sampler2D ExitButtonTexture;

void main()
{
	
		color = texture(backGroundTexture, TexCoords);

	if (texture(startButtonTexture, TexCoords).w >= 0.01)
	{
		color.xyz = texture(backGroundTexture, TexCoords).xyz * abs(texture(startButtonTexture, TexCoords).w - 1);
		color.w = texture(backGroundTexture, TexCoords).w;
	}
	else if (texture(settingsButtonTexture, TexCoords).w >= 0.01)
	{
		color.xyz = texture(backGroundTexture, TexCoords).xyz * abs(texture(settingsButtonTexture, TexCoords).w - 1);
		color.w = texture(backGroundTexture, TexCoords).w;
	}
	else if (texture(ExitButtonTexture, TexCoords).w >= 0.01)
	{
		color.xyz = texture(backGroundTexture, TexCoords).xyz * abs(texture(ExitButtonTexture, TexCoords).w - 1);
		color.w = texture(backGroundTexture, TexCoords).w;
	}
	else
		color = texture(backGroundTexture, TexCoords);

	if (texture(startButtonTexture, TexCoords).w >= 0.01)
		color.xyz += texture(startButtonTexture, TexCoords).xyz;
	
	if (texture(settingsButtonTexture, TexCoords).w >= 0.01)
		color.xyz += texture(settingsButtonTexture, TexCoords).xyz;
	
	if (texture(ExitButtonTexture, TexCoords).w >= 0.01)
		color.xyz += texture(ExitButtonTexture, TexCoords).xyz;


	FragColor = color;
	

}