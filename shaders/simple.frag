#version 430
#extension GL_ARB_shader_image_load_store : enable

// Data from geometry shader
in fData {
    vec2 UV;
    mat4 projectionMatrix;
    float depth;
} frag;

out vec4 color;
uniform layout(RGBA8) image3D voxelTexture;
uniform sampler2D DiffuseTexture;
uniform int voxelSize;

void main() {
    vec4 materialColor = texture(DiffuseTexture, frag.UV);
    color = vec4(1.0, frag.depth, 0.0, 1.0);

    ivec3 pos = ivec3(gl_FragCoord.xy, frag.depth * voxelSize);
    imageStore(voxelTexture, pos, color);

}