#version 330 core 

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform mat4 uM; //model (transformation) matrix
uniform mat4 uV; //view matrix
uniform mat4 uP; //projection matrix

void main()
{
	gl_Position = uP * uV * uM *  vec4(inPos, 1.0);
	chTex = inTex;
}