#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 outCol;

uniform vec3 uCol;
uniform vec3 uLightCol;
uniform vec3 uLightPos;
uniform vec3 uCameraAt;

void main()
{
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * uLightCol;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(uLightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * uLightCol;

	float specularStrength = 1;
	vec3 viewDir = normalize(uCameraAt - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * uLightCol;

	vec3 final = (ambient + diffuse + specular) * uCol;

	outCol = vec4(final, 1.0);
}