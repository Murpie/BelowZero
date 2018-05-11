#version 440 core
layout(location = 0) out vec3 FragColor;

in vec2 TexCoords;
uniform vec3 view_position;


uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D iceNormal;
uniform sampler2D SceneTexture;

uniform float hp;
uniform float cold;
uniform float water;
uniform float food;
uniform float fade;

uniform int ScreenX;
uniform int ScreenY;

vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);


void main()
{
	float Cold = cold;

	vec2 texCoords = vec2((gl_FragCoord.x / ScreenX), (gl_FragCoord.y / ScreenY));

	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Albedo = texture(SceneTexture, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;

	vec3 viewDir = normalize(view_position - FragPos);

	vec3 ice = texture(iceNormal, TexCoords).rgb;
	vec3 bump = normalize(ice) * 2.0 - 1.0;
	bump = ice;
	bump *= normalize(viewDir);


	float _Refraction = 1.3;
	float _Frensel = 2.0;
	float _Reflectance  = 1.0;
	
	vec3 n = normalize(ice);
	vec3 v = viewDir;
	
	float fr = pow(1.0f - dot(v, n), _Frensel) * _Reflectance;
	vec3 reflectDir = reflect(-v, n);
	vec3 refractDir = refract(-v, n, _Refraction);
	
	vec3 reflectColor = texture(SceneTexture, reflectDir.xy).rgb;
	vec3 refractColor = texture(SceneTexture, refractDir.xy).rgb;

	vec3 finalColor = reflectColor * fr + refractColor;
	vec3 Mix2 = mix(finalColor, Albedo, 0.8);


	//mat4 testMatrix = mat4(1) * projMatrix;

	//ice = vec4(projMatrix * vec4(ice, 1.0)).xyz;
	//vec3 normalDir = vec4(mat4(1) * normalize(vec4(viewDir, 1.0))).xyz;
	//ice = normalDir * ice;
	vec3 bumpTex = 2.0 * texture(iceNormal, texCoords).rgb - 1.0;
	vec2 newUV = vec2(texCoords.x + bumpTex.x, texCoords.y + bumpTex.y);

	vec3 posDir = normalize(view_position - FragPos);
	bump = normalize(-viewDir * ice);

	//FragColor = ice;//RefractionVariable * FragColor;

	vec3 Refract = normalize(refract(-viewDir, -bumpTex, 1.75));
	vec3 Reflect = normalize(reflect(-viewDir, -bumpTex));
	vec3 testAlbedo1 = texture(SceneTexture, Refract.xy).rgb;
	vec3 testAlbedo2 = texture(SceneTexture, Reflect.xy).rgb;

	vec3 testAlbedo = (testAlbedo2 * testAlbedo1);
	vec3 Mix = mix(testAlbedo, Albedo, 0.8);
	Mix *= vec3(0.33, 0.9, 1.0);
	
	vec3 itsagoodashow = texture(SceneTexture, newUV).rgb;


	float coldVariable = 1.0;
	if (Cold <= 50)
	{
		coldVariable = Cold / 100;
	}
	if (Cold <= 0)
		Cold = 0;

	vec3 coldTexture = mix(Mix, Albedo, coldVariable);

	FragColor = coldTexture;

}