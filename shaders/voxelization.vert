#version 330 core

layout(location = 0) in vec3 vertexPosition_model;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_model;
layout(location = 3) in vec3 vertexTangent_model;
layout(location = 4) in vec3 vertexBitangent_model;

uniform mat4 DepthModelViewProjectionMatrix;
uniform mat4 ModelMatrix;

out vData {
    vec2 UV;
    vec4 position_depth;
} vert;

void main() {
    vert.UV = vertexUV;
    vert.position_depth = DepthModelViewProjectionMatrix * vec4(vertexPosition_model, 1);
	vert.position_depth.xyz = vert.position_depth.xyz * 0.5f + 0.5f;

    gl_Position = ModelMatrix * vec4(vertexPosition_model,1);
}