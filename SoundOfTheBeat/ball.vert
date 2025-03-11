#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTex;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 uM; //model (transformation) matrix
uniform mat4 uPV; //projection * view matrix

void main()
{
	vec4 model = uM * vec4(inPos, 1.0);
	gl_Position = uPV * model;
	FragPos = vec3(model);
	Normal = inNormal;
}