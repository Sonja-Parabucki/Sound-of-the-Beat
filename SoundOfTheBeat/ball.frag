#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 outCol;

uniform vec3 uCol;
uniform vec3 uLightCol;
uniform vec3 uLightPos;

void main()
{
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * uLightCol;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(uLightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * uLightCol;

	vec3 final = (ambient + diffuse) * uCol;

	outCol = vec4(final, 1.0);
}