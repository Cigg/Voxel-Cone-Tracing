#version 330 core

// Data from geometry shader
in fData {
    vec2 UV;
} frag;

out vec4 color;

uniform sampler2D DiffuseTexture;

void main() {
    vec4 materialColor = texture(DiffuseTexture, frag.UV);
    color = materialColor;
} 