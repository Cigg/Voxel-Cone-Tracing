#version 330 core

layout(location = 0) in vec3 vertexPosition_model;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_model;
layout(location = 3) in vec3 vertexTangent_model;
layout(location = 4) in vec3 vertexBitangent_model;

out vData {
    vec2 UV;
} vert;

void main() {
    vert.UV = vertexUV;
    gl_Position = vec4(vertexPosition_model,1);
}