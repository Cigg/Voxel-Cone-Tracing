#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D Texture;

void main(){
	color = vec4(vec3((texture(Texture, UV).r)), 1.0);
}
