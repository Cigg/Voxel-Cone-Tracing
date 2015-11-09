#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D Texture;

void main(){
	color = vec4(texture(Texture, UV).rgb, 1.0);
}
