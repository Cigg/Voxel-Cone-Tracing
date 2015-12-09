#version 430
#extension GL_ARB_shader_image_load_store : enable

uniform layout(RGBA8) image3D voxelTexture;
uniform int voxelDimensions;

void main() {
	for ( int i = 0; i < voxelDimensions; i++)
    	imageStore(voxelTexture, ivec3(gl_FragCoord.x, gl_FragCoord.y, i), vec3(0.0,0.0,0.0));
}