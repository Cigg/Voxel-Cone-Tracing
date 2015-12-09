#version 330 core

layout(location = 0)in vec4 vert;

void main()
{
    gl_Position = vert;
}