#version 440 core
// Ouput data
out vec4 color;

// Interpolated values from the vertex shaders
in vec2 uv;

uniform sampler2D myTextureSampler;

uniform float lifeLevel;

void main(){
	vec3 bbTexture = texture(myTextureSampler, uv).rgb;

	// Output color = color of the texture at the specified UV
	//color = texture( myTextureSampler, uv );
	//color = vec4(0.0, lifeLevel, 0.0, 1.0);
	color = vec4(bbTexture, 1.0);
	
	// Hardcoded life level, should be in a separate texture.
	//if (uv.x < lifeLevel && uv.y > 0.3 && uv.y < 0.7 && uv.x > 0.04 )
		//color = vec4(0.2, 0.8, 0.2, 1.0); // Opaque green
}