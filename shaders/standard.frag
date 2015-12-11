#version 400 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_world;
in vec3 Normal_world;
in vec3 Tangent_world;
in vec3 Bitangent_world;
in vec3 Normal_cam;
in vec3 Tangent_cam;
in vec3 Bitangent_cam;
in vec3 Position_cam;
in vec3 EyeDirection_cam;
in vec3 LightDirection_cam;
in vec4 Position_depth; // Position from the shadow map point of view

out vec4 color;

//Uniforms
// uniform vec3 AmbientColor;
// uniform vec3 DiffuseColor;
// uniform vec3 SpecularColor;

uniform float Shininess;
uniform float Opacity;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D MaskTexture;
uniform sampler2D HeightTexture;
uniform sampler2DShadow ShadowMap;

uniform sampler3D VoxelTexture;
uniform float VoxelGridWorldSize;
uniform int VoxelDimensions;

uniform vec2 DiffuseTextureSize;
uniform vec2 SpecularTextureSize;
uniform vec2 MaskTextureSize;
uniform vec2 HeightTextureSize;

const int NUM_CONES = 6;
const float MAX_DIST = 600.0;
const float OCCLUSION_THRESH = 0.95;

vec3 coneDirections[6] = vec3[](
                            vec3(0, 1, 0),
                            vec3(0, 0.5, 0.866025),
                            vec3(0.823639, 0.5, 0.267617),
                            vec3(0.509037, 0.5, -0.700629),
                            vec3(-0.509037, 0.5, -0.700629),
                            vec3(-0.823639, 0.5, 0.267617)
                            );

float coneWeights[6] = float[](0.25, 0.15, 0.15, 0.15, 0.15, 0.15);

mat3 tangentToWorld;

vec4 sampleVoxels(vec3 worldPosition, float lod) {
    //vec3 offset = vec3(0.0, 1 / VoxelDimensions, 0.0);
    vec3 voxelTextureUV = worldPosition / (VoxelGridWorldSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5;

    return textureLod(VoxelTexture, voxelTextureUV, lod);
}

vec4 coneTrace(vec3 direction) {

    // Returns the lod level that will be used if the texture is sampled automatically (with texture(..) )
    //float lodLevel = textureQueryLod(VoxelTexture, voxelTextureUV).x;
    // level 0 mipmap is full size, level 1 is half that size and so on

    float lod = 0.0;
    vec3 color = vec3(0);
    float alpha = 0.0;

    float voxelWorldSize = VoxelGridWorldSize / VoxelDimensions;
    float dist = 1.0 * voxelWorldSize; // Start one voxel away to avoid self occlusion
    while(dist < MAX_DIST && alpha < OCCLUSION_THRESH) {
        // radius = tan(60) * dist
        float radius = sqrt(3) * dist;
        float lodLevel = log2(radius / voxelWorldSize);
        vec4 voxelColor = sampleVoxels(Position_world + dist * direction, lodLevel);
        color += (1.0 - alpha) * voxelColor.rgb;
        alpha += (1.0 - alpha) * voxelColor.a;
        dist += radius * 2.0;
    }

    return vec4(color, alpha);
}

vec4 indirectLight() {
    vec4 color = vec4(0);

    for(int i = 0; i < NUM_CONES; i++) {
        color += coneWeights[i] * coneTrace(tangentToWorld * coneDirections[i]);
    }

    return color;
}

void main() {
	vec4 materialColor = texture(DiffuseTexture, UV);
	if(materialColor.a < 0.5) {
		discard;
	}

	float alpha = materialColor.a;

	vec3 lightColor = vec3(1.0);
	
	vec4 specularColor = texture(SpecularTexture, UV);
	specularColor = length(specularColor.gb) > 0.0 ? specularColor : specularColor.rrra;

	// Calculate gradients
	vec2 offset = vec2(1.0) / HeightTextureSize;
	float curr = texture(HeightTexture, UV).r;
    float diffX = texture(HeightTexture, UV + vec2(offset.x, 0.0)).r - curr;
    float diffY = texture(HeightTexture, UV + vec2(0.0, offset.y)).r - curr;

    // Tangent space bump normal
    float bumpMult = -10.0;
    vec3 bumpNormal_tangent = normalize(vec3(bumpMult*diffX, 1.0, bumpMult*diffY));

    // Matrix to convert from tangent space to camera space
    mat3 tangentToCam = inverse(transpose(mat3(Tangent_cam, Normal_cam, Bitangent_cam)));

    // Camera space normal, light direction and eye direction
    vec3 N = normalize(tangentToCam * bumpNormal_tangent);
    vec3 L = normalize(LightDirection_cam);
    vec3 E = normalize(EyeDirection_cam);

    float cosTheta = max(0, dot(N, L));

    float visibility = texture(ShadowMap, vec3(Position_depth.xy, (Position_depth.z - 0.001)/Position_depth.w));
    visibility = 1.0;

    vec3 ambientLighting = 0.1 * materialColor.xyz;
    vec3 diffuseReflection = visibility * cosTheta * lightColor * materialColor.xyz;
    vec3 specularReflection = visibility * lightColor * specularColor.xyz * pow(max(0.0, dot(reflect(-L, N), E)), Shininess);

    tangentToWorld = inverse(transpose(mat3(Tangent_world, Normal_world, Bitangent_world)));
    
    vec4 indirectReflection = indirectLight();

	//color = vec4(occlusion*(ambientLighting + diffuseReflection + specularReflection), alpha);
    //color = vec4(vec3(occlusion), alpha);
    color = vec4(indirectReflection.rgb, alpha);
    //color = vec4(voxelColor.rgb, alpha);
    //color = vec4(voxelTextureUV, alpha);
    //color = vec4(vec3(visibility), 1.0);
}