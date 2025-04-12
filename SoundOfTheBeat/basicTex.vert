#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform float uAspectRatio;

void main()
{
	gl_Position = vec4(inPos.x, inPos.y * uAspectRatio, inPos.z, 1.0);
	chTex = inTex;
}