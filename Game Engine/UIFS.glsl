#version 440 core
layout(location = 0) out vec4 color;

in vec2 texCoords;

uniform sampler2D theTexture;
uniform sampler2D equipedTexture;
uniform sampler2D inventoryTexture1;
uniform sampler2D inventoryTexture2;
uniform sampler2D inventoryTexture3;
uniform sampler2D inventoryTexture4;
uniform sampler2D inventoryTexture5;
uniform sampler2D SceneTexture;

uniform float hp;
uniform float cold;
uniform float water;
uniform float food;

void main()
{
	if (texture(equipedTexture, texCoords).w >= 0.01)
	{
		color.xyz = texture(SceneTexture, texCoords).xyz * abs(texture(equipedTexture, texCoords).w - 1);
		color.w = texture(SceneTexture, texCoords).w;
	}
	else if (texture(theTexture, texCoords).w >= 0.01)
	{
		color.xyz = texture(SceneTexture, texCoords).xyz * abs(texture(theTexture, texCoords).w - 1);
		color.w = texture(SceneTexture, texCoords).w;
	}
	else if (texture(inventoryTexture1, texCoords).w >= 0.01)
	{
		color.xyz = texture(SceneTexture, texCoords).xyz * abs(texture(inventoryTexture1, texCoords).w - 1);
		color.w = texture(SceneTexture, texCoords).w;
	}
	else if (texture(inventoryTexture2, texCoords).w >= 0.01)
	{
		color.xyz = texture(SceneTexture, texCoords).xyz * abs(texture(inventoryTexture2, texCoords).w - 1);
		color.w = texture(SceneTexture, texCoords).w;
	}
	else if (texture(inventoryTexture3, texCoords).w >= 0.01)
	{
		color.xyz = texture(SceneTexture, texCoords).xyz * abs(texture(inventoryTexture3, texCoords).w - 1);
		color.w = texture(SceneTexture, texCoords).w;
	}
	else if (texture(inventoryTexture4, texCoords).w >= 0.01)
	{
		color.xyz = texture(SceneTexture, texCoords).xyz * abs(texture(inventoryTexture4, texCoords).w - 1);
		color.w = texture(SceneTexture, texCoords).w;
	}
	else if (texture(inventoryTexture5, texCoords).w >= 0.01)
	{
		color.xyz = texture(SceneTexture, texCoords).xyz * abs(texture(inventoryTexture5, texCoords).w - 1);
		color.w = texture(SceneTexture, texCoords).w;
	}
	else
	{
		color = texture(SceneTexture, texCoords);
	}



	if ( texCoords.x > 0.0439 && texCoords.x < 0.04 + (0.1585 * (hp / 100.0)) && texCoords.y > 0.028 && texCoords.y < 0.039)
    		color = vec4(0.0, 1.0, 0.0, 1.0);
	else if ( texCoords.x > 0.0433 && texCoords.x < 0.04 + (0.1585 * (food / 100.0)) && texCoords.y > 0.074 && texCoords.y < 0.085)
    		color = vec4(1.0, 0.0, 0.0, 1.0);
	else if ( texCoords.x > 0.0433 && texCoords.x < 0.04 + (0.1585 * (water / 100.0)) && texCoords.y > 0.12 && texCoords.y < 0.132)
    		color = vec4(0.0, 0.0, 1.0, 1.0);
	else if ( texCoords.x > 0.0433 && texCoords.x < 0.04 + (0.1585 * (cold / 100.0)) && texCoords.y > 0.167 && texCoords.y < 0.178)
    		color = vec4(0.0, (cold / 100), 0.0, 1.0);

	if (texture(theTexture, texCoords).w >= 0.01)
		color.xyz += texture(theTexture, texCoords).xyz;
	
	if (texture(equipedTexture, texCoords).w >= 0.01)
		color.xyz += texture(equipedTexture, texCoords).xyz;

	if (texture(inventoryTexture1, texCoords).w >= 0.01)
		color.xyz += texture(inventoryTexture1, texCoords).xyz;

	if (texture(inventoryTexture2, texCoords).w >= 0.01)
		color.xyz += texture(inventoryTexture2, texCoords).xyz;

	if (texture(inventoryTexture3, texCoords).w >= 0.01)
		color.xyz += texture(inventoryTexture3, texCoords).xyz;

	if (texture(inventoryTexture4, texCoords).w >= 0.01)
		color.xyz += texture(inventoryTexture4, texCoords).xyz;

	if (texture(inventoryTexture5, texCoords).w >= 0.01)
		color.xyz += texture(inventoryTexture5, texCoords).xyz;
	

}