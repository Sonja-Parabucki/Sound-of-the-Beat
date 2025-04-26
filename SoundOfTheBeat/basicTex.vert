#version 330 core

layout(location = 0) in vec3 inPos;

uniform float uAspectRatio;

void main()
{
	gl_Position = vec4(inPos.x, inPos.y * uAspectRatio, inPos.z, 1.0);
}