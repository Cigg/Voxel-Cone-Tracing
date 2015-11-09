#version 330 core

// Data from geometry shader
in fData {
    vec2 UV;
} frag;

out vec4 color;

uniform sampler2D DiffuseTexture;

void main() {
    vec4 materialColor = texture(DiffuseTexture, frag.UV);
    
    color = length(materialColor) < 0.001 ? vec4(1.0,0.0,0.0,1.0) : materialColor;
} 