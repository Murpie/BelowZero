#version 440 core
out vec4 FragColor;

in vec2 TexCoords;
vec4 color;

uniform sampler2D textureToUse;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform float Timer;
uniform float scaling1;
uniform float scaling2;
uniform float scaling3;
float newScale;
uniform int buttonTransformation;
void main()
{
	if (scaling1 > 1.0)
		newScale = scaling1 / 10; // Test to see if it it able to change color
	if (scaling2 > 1.0)
		newScale = scaling2 / 10;
	if (scaling3 > 1.0)
		newScale = scaling3 / 10;
	
	color = texture(textureToUse, TexCoords);
	vec4 DarkTexture = texture(Texture2, TexCoords);
	vec4 LightTexture = texture(Texture1, TexCoords);

	//vec4 settingsW = texture(SettingsW, TexCoords);
	//vec4 startW = texture(StartW, TexCoords);
	//vec4 exitW = texture(ExitW, TexCoords);

	vec4 finalColor = vec4(1);

	float InterVariable = 0.0f;
	
	if (Timer <=  4.0f)
	{
		InterVariable = Timer / 4.0f;
		finalColor = mix(DarkTexture, LightTexture, InterVariable);
	}
	else if (Timer <= 5.0f && Timer >= 4.0f)
		finalColor = LightTexture;
	else if (Timer >= 5.0f && Timer <= 9.0f)
	{
		InterVariable = (Timer - 5.0f) / 4.0f;
		finalColor = mix(LightTexture, DarkTexture, InterVariable);
	}
	else if (Timer <= 10.0f && Timer >= 9.0f)
		finalColor = DarkTexture;

	//Buttons
	if (Timer <= 4.0f)
	{
		InterVariable = Timer / 4.0f;
		color = mix(vec4(0.8, 0.8, 0.8, color.a), vec4(0.1, 0.1, 0.1, color.a), InterVariable);
	}
	else if (Timer <= 5.0f && Timer >= 4.0f)
	{
		color = vec4(0.1, 0.1, 0.1, color.a);
	}
	else if (Timer >= 5.0f && Timer <= 9.0f)
	{
		InterVariable = (Timer - 5.0f) / 4.0f;
		color = mix(vec4(0.1, 0.1, 0.1, color.a), vec4(0.8, 0.8, 0.8, color.a), InterVariable);
	}
	else if (Timer <= 10.0f && Timer >= 9.0f)
	{
		color = vec4(0.8, 0.8, 0.8, color.a);
	}
		

	

	



	if (color.a < 0.35)
		discard;
	else if (buttonTransformation == 0)
	{

		FragColor = finalColor;

	}
	else
		FragColor = color;
	//if(buttonTransformation == 0)
	//	FragColor = vec4(LightTexture, 1.0);
	//else

}