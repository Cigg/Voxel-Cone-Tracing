#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal_cameraspace;

out vec4 color;

//Uniforms
uniform sampler2D diffuseTexture;

void main() {
	vec3 diffuseColor = texture(diffuseTexture, UV).rgb;
	color = vec4(diffuseColor, 1.0);
}