#version 440
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform int onOrOff;

float weight[6] = float[](1.0, 0.227027027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{

	vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
	vec3 result = texture(image, TexCoords).rgb * weight[onOrOff]; // current fragment's contribution
	if (onOrOff == 1) 
	{
		for (int i = 2; i < 6; ++i)
		{
			result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * (weight[i] / 2);
			result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * (weight[i] / 2);
			result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * (weight[i] / 2);
			result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * (weight[i] / 2);
		}
	}


	FragColor = vec4(result, 1.0);
}