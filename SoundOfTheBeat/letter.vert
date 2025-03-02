#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(inPos, 1.0);
    chTex = inTex;
}  