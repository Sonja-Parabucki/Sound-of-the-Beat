#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;
out vec4 chCol;

uniform vec2 uR;
uniform float uAspect;     // Aspect ratio
uniform float uInflation;

void main()
{
	gl_Position = vec4((inPos.x * uAspect * uInflation + uR[0]),
						(inPos.y * uInflation + uR[1]),
						inPos.z,
						1.0); 
	chCol = inCol;
}