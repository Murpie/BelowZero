#version 440 core
layout(location = 0) out vec3 FragColor;
  
in vec2 TexCoords;
uniform vec3 view_position;


struct Light {
    vec3 Position;
    vec3 Color;
	float Linear;
    float Quadratic;
};

	vec3 drColor = vec3(0.9f, 1.0f, 0.84f);
	vec3 drPosition = vec3(400.0f, 200.0f, 0.0f);


const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gSpecular;
uniform sampler2D gMetallic;
uniform sampler2D gAO;
uniform sampler2D ssao;

void main()
{ 
	// retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 Specular = texture(gSpecular, TexCoords).rgb;
	vec3 Metallic = texture(gMetallic, TexCoords).rgb;
	vec3 AO = texture(gAO, TexCoords).rgb;
	float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    // then calculate lighting as usual
    vec3 lighting = vec3(0.0 * Albedo * AmbientOcclusion * AO);
    vec3 viewDir = normalize(view_position + 1 - FragPos);

    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * lights[i].Color * AmbientOcclusion;

		vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
		vec3 metallic = lights[i].Color * spec * Metallic;
        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular + metallic;
    }

	//Test Directional Light
	    vec3 lightDir = normalize(drPosition - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.3) * Albedo * drColor * AmbientOcclusion;

		vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = drColor * spec * Specular;
		vec3 metallic = drColor * spec * Metallic;
        // attenuation
        float distance = length(drPosition - FragPos);
        float attenuation = 1.0;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular + metallic;



	float density = 0.12;
	float gradient = 1.5;
	float distanceToPos = length(view_position - FragPos);
    float visibility = exp(-pow((distanceToPos * density), gradient));
	visibility  = clamp(visibility, 0.0, 1.0);

	FragColor = lighting;
	FragColor = mix(vec3(0.749, 0.843, 0.823), FragColor, visibility);


}