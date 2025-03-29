#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 outCol;

uniform vec3 uLightCol;
uniform vec3 uLightPos;
uniform vec3 uCameraAt;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

vec3 calcOne(vec3 lightPos) 
{
	vec3 ambient = material.ambient;

	vec3 norm = normalize(Normal);
	vec3 distanceVec = lightPos - vec3(FragPos.xy, 0.0);
	float distance = length(distanceVec);
	vec3 lightDir = normalize(distanceVec);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * material.diffuse;

	vec3 viewDir = normalize(uCameraAt - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * material.specular;

	float attenuation = 1.0 / (1.0 + distance * 0.045  + distance * distance * 0.0075);

	return attenuation * (ambient + diffuse + specular) * uLightCol;
}

void main()
{
	vec3 final = calcOne(vec3(uLightPos.xy, 0.0));
	final += calcOne(vec3(-uLightPos.xy, 0.0));
	final += calcOne(vec3(uLightPos.x, -uLightPos.y, 0.0));
	final += calcOne(vec3(-uLightPos.x, uLightPos.y, 0.0));

	outCol = vec4(final, 1.0);
}