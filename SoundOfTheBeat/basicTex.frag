#version 330 core

out vec4 outCol;

uniform vec3 uCol;


void main()
{
	outCol = vec4(uCol, 1.0);
}