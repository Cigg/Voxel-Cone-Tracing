#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal_cameraspace;

out vec4 color;

void main() {
	
	color = vec4(normal_cameraspace, 1.0);
}