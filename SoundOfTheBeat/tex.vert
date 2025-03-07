#version 330 core 

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform mat4 uM; //model (transformation) matrix
uniform mat4 uPV; //projection * view matrix

void main()
{
	gl_Position = uPV * uM *  vec4(inPos, 1.0);
	chTex = inTex;
}