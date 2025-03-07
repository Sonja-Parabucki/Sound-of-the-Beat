#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;
out vec4 chCol;

uniform float uInflation;

uniform mat4 uM; //model (transformation) matrix
uniform mat4 uV; //view matrix
uniform mat4 uP; //projection matrix

void main()
{
	gl_Position = uP * uV * uM * vec4((inPos.x * uInflation),
								(inPos.y * uInflation),
								inPos.z,
								1.0); 
	chCol = inCol;
}