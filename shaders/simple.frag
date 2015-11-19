#version 430
#extension GL_ARB_shader_image_load_store : enable

// Data from geometry shader
in fData {
    vec2 UV;
    mat4 projectionMatrix;
} frag;

out vec4 color;
uniform layout(RGBA8) image3D voxelTexture;
uniform sampler2D DiffuseTexture;

//layout(r32f /* Format, must match parameter in glBindImageTexture() */, binding = 0) writeonly uniform highp image2D uImage;

void main() {
    vec4 materialColor = texture(DiffuseTexture, frag.UV);
    
//    float size = textureSize(voxelTexture, 0).x
//    vec4 temp = texture(voxelTexture, vec3(0,0,0));
    
    imageStore(voxelTexture, ivec3(gl_FragCoord), materialColor);
    color = vec4(1.0, 0.0, 0.0, 1.0);

}