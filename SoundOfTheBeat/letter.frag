#version 330 core

in vec2 chTex; //coordinates
out vec4 outCol;

uniform sampler2D uTex; //texture unit
uniform vec3 uTextCol;

void main()
{
    outCol = vec4(uTextCol, 1.0) * vec4(1.0, 1.0, 1.0, texture(uTex, chTex).r);
}