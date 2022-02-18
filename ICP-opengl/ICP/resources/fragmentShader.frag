#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Normal;
in vec3 FragPos;

// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

uniform vec3 lightDirection;

uniform vec3 lightPos;

uniform vec3 camPos;

uniform vec3 lightColor;

vec3 pointLight()
{	
	vec3 lightVec = lightPos - FragPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 0.3;
	float b = 0.2;

	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightVec);
	float diffuse = max(dot(normal, lightDir), 0.0f);

	// specular lighting
	float specularLight = 0.20f;
	vec3 viewDirection = normalize(camPos - FragPos);
	vec3 reflectionDirection = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return texture(myTextureSampler, UV).rgb * (diffuse * inten + ambient + specular * inten) * lightColor;
}

vec3 directionalLight()
{
	vec3 lightColor = vec3(1.0f,1.0f,1.0f);
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightDirection);
	float diffuse = max(dot(normal, lightDir), 0.0f);

	// specular lighting
	float specularLight = 0.20f;
	vec3 viewDirection = normalize(camPos - FragPos);
	vec3 reflectionDirection = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;
	return texture(myTextureSampler, UV).rgb * (diffuse + ambient + specular);
}

void main()
{
	color = vec4(pointLight() + directionalLight(),0.7);
}