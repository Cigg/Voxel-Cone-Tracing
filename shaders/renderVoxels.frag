#version 330 core

in vec3 normal_world;
in vec4 fragColor;

out vec4 color;

void main() {
	if(fragColor.a < 0.5f)
		discard;

	color = vec4(fragColor);
}