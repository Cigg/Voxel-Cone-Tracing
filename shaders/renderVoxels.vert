#version 330 core

out vec4 color;

uniform float VoxelSize;
uniform int Dimensions;
uniform int TotalNumVoxels; // Dimensions^3

uniform sampler3D VoxelsTexture;

void main() {
	vec3 pos; // Center of voxel
	pos.x = gl_VertexID % Dimensions;
	pos.z = (gl_VertexID / Dimensions) % Dimensions;
	pos.y = gl_VertexID / (Dimensions*Dimensions);

	color = texture(VoxelsTexture, pos/Dimensions);
	gl_Position = vec4(pos - Dimensions*0.5, 1);
}