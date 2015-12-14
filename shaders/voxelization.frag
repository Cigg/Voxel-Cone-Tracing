#version 430
#extension GL_ARB_shader_image_load_store : enable

// Data from geometry shader
in fData {
    vec2 UV;
    mat4 projectionMatrix;
    flat int axis;
    vec4 position_depth; // Position from the shadow map point of view
} frag;

out vec4 color;
uniform layout(RGBA8) image3D voxelTexture;
uniform sampler2D DiffuseTexture;
uniform sampler2DShadow ShadowMap;
uniform int voxelDimensions;

void main() {
	// Material color texture lookup and check if position voxel is shadowed widh the shadow map
    vec4 materialColor = texture(DiffuseTexture, frag.UV);
    float visibility = texture(ShadowMap, vec3(frag.position_depth.xy, (frag.position_depth.z - 0.001)/frag.position_depth.w));

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

	texPos.z = voxelDimensions - texPos.z - 1;

	color = vec4(texPos / float(voxelDimensions), 1.0);
    imageStore(voxelTexture, texPos, vec4(materialColor.rgb * visibility, 1.0));
    //imageStore(voxelTexture, texPos, materialColor);
}