#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;
out vec3 normal_cameraspace;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main() {
	gl_Position =  projectionMatrix * modelViewMatrix * vec4(vertexPosition_modelspace,1);
	normal_cameraspace = normalize((modelViewMatrix * vec4(vertexNormal_modelspace,0)).xyz);
	UV = vertexUV;
}

