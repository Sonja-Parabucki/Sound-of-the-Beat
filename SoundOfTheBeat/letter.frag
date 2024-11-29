#version 330 core

in vec2 chTex; //coordinates
out vec4 outCol;

uniform sampler2D uTex; //texture unit
//uniform vec3 textCol;

void main()
{
    outCol = vec4(1.0, 1.0, 1.0, texture(uTex, chTex).r);
}