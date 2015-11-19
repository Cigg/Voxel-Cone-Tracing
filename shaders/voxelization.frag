#version 430
#extension GL_ARB_shader_image_load_store : enable

// Data from geometry shader
in fData {
    vec2 UV;
    mat4 projectionMatrix;
    flat int axis;
} frag;

out vec4 color;
uniform layout(RGBA8) image3D voxelTexture;
uniform sampler2D DiffuseTexture;
uniform int voxelDimensions;

void main() {
    vec4 materialColor = texture(DiffuseTexture, frag.UV);
	ivec3 camPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, voxelDimensions * gl_FragCoord.z);
	ivec3 texPos;
	if(frag.axis == 1) {
	    texPos.x = voxelDimensions - camPos.z;
		texPos.z = camPos.x;
		texPos.y = camPos.y;
	}
	else if(frag.axis == 2) {
	    texPos.z = camPos.y;
		texPos.y = voxelDimensions - camPos.z;
		texPos.x = camPos.x;
	} else {
	    texPos = camPos;
	}

	color = vec4(texPos / float(voxelDimensions), 1.0);
    imageStore(voxelTexture, texPos, color);
}