#version 330 core

// Data from geometry shader
in fData {
    vec2 UV;
} frag;

out vec4 color;

//uniform sampler3D voxelTexture;
uniform sampler2D DiffuseTexture;

//layout(r32f /* Format, must match parameter in glBindImageTexture() */, binding = 0) writeonly uniform highp image2D uImage;

void main() {
    vec4 materialColor = texture(DiffuseTexture, frag.UV);
    
//    float size = textureSize(voxelTexture, 0).x
//    vec4 temp = texture(voxelTexture, vec3(0,0,0));
    
    color = vec4(1.0, 0.0, 0.0, 1.0);
}