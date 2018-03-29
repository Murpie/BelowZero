#version 440 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform mat4 view_matrix;

vec3 getPosition(vec2 uv)
{
	vec3 pos = texture(gPosition, uv).xyz;
	pos = (view_matrix * vec4(pos, 1.0)).xyz;
	return pos;
}

vec3 getNormal(vec2 uv) // Get Normal in view space
{
	vec3 norm = normalize(texture(gNormal, uv).xyz);
	norm = normalize((view_matrix * vec4(norm, 0.0)).xyz); // Gets rid of translation
	return norm;
}

vec2 getRandom(vec2 uv, vec2 screenSize) // Get normal vector from noise texture
{
	vec2 randNormNew = normalize(texture(noiseTexture, screenSize * uv / 64).xy * 2.0 - 1.0);

	return randNormNew;
}

float doAO(vec2 uv, vec2 coord, vec3 fragPos, vec3 normal)
{
	float scale = 2.0;
	float falloff = 0;
	float intensity = 1.0;

	vec3 diff = getPosition(uv + coord) - fragPos;
	vec3 v = normalize(diff);
	float d = length(diff) * scale;
	return max(0.0, dot(normal, v) - falloff) * (1.0 / (1.0 + d)) * intensity;
}

void main() {

	vec2 screenSize = vec2(1280, 720);
	const vec2 uvSample[4] = { vec2(1 / screenSize.x, 0), vec2(-1 / screenSize.x, 0), vec2(0, 1 / screenSize.y), vec2(0, -1 / screenSize.y) }; // Normalize UV Samples

	vec3 p = getPosition(TexCoords);
	vec3 n = getNormal(TexCoords);
	vec2 randomVector = getRandom(TexCoords, screenSize);

	float sampleRadius = 25.0;
	float ao = 0.0;
	float rad = sampleRadius / p.z;

	int iterations = 4;
	for (int i = 0; i < iterations; ++i) //Iterate through all the UV samples
	{
		vec2 coord1 = uvSample[i] * rad;
		vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);

		//Use some magic numbers to make it look good, smoothed kinda
		ao += doAO(TexCoords, coord1*0.25, p, n);
		ao += doAO(TexCoords, coord2*0.5, p, n);
		ao += doAO(TexCoords, coord1*0.75, p, n);
		ao += doAO(TexCoords, coord2, p, n);
	}
	
	ao /= float(iterations) * 4; // Divides the ao with 16 for each frame. Works like a reset

	FragColor = 1.0 - ao;
}