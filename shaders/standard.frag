#version 400 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_world;
in vec3 Normal_world;
in vec3 Tangent_world;
in vec3 Bitangent_world;
in vec3 EyeDirection_world;
in vec4 Position_depth; // Position from the shadow map point of view

out vec4 color;


// Textures
uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D MaskTexture;
uniform sampler2D HeightTexture;
uniform vec2 HeightTextureSize;

// Material properties
uniform float Shininess;
uniform float Opacity;

// Shadow map
uniform sampler2DShadow ShadowMap;

// Voxel stuff
uniform sampler3D VoxelTexture;
uniform float VoxelGridWorldSize;
uniform int VoxelDimensions;

// Toggle "booleans"
uniform float ShowDiffuse;
uniform float ShowIndirectDiffuse;
uniform float ShowIndirectSpecular;
uniform float ShowAmbientOcculision;

uniform vec3 LightDirection;

const float MAX_DIST = 100.0;
const float ALPHA_THRESH = 0.95;

// 6 60 degree cone
const int NUM_CONES = 6;
vec3 coneDirections[6] = vec3[]
(                            vec3(0, 1, 0),
                            vec3(0, 0.5, 0.866025),
                            vec3(0.823639, 0.5, 0.267617),
                            vec3(0.509037, 0.5, -0.700629),
                            vec3(-0.509037, 0.5, -0.700629),
                            vec3(-0.823639, 0.5, 0.267617)
                            );
float coneWeights[6] = float[](0.25, 0.15, 0.15, 0.15, 0.15, 0.15);

// // 5 90 degree cones
// const int NUM_CONES = 5;
// vec3 coneDirections[5] = vec3[]
// (                            vec3(0, 1, 0),
//                             vec3(0, 0.707, 0.707),
//                             vec3(0, 0.707, -0.707),
//                             vec3(0.707, 0.707, 0),
//                             vec3(-0.707, 0.707, 0)
//                             );
// float coneWeights[5] = float[](0.28, 0.18, 0.18, 0.18, 0.18);

mat3 tangentToWorld;

vec4 sampleVoxels(vec3 worldPosition, float lod) {
    vec3 offset = vec3(1.0 / VoxelDimensions, 1.0 / VoxelDimensions, 0); // Why??
    vec3 voxelTextureUV = worldPosition / (VoxelGridWorldSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5 + offset;
    return textureLod(VoxelTexture, voxelTextureUV, lod);
}

// Third argument to say how long between steps?
vec4 coneTrace(vec3 direction, float tanHalfAngle, out float occlusion) {
    
    // lod level 0 mipmap is full size, level 1 is half that size and so on
    float lod = 0.0;
    vec3 color = vec3(0);
    float alpha = 0.0;
    occlusion = 0.0;

    float voxelWorldSize = VoxelGridWorldSize / VoxelDimensions;
    float dist = voxelWorldSize; // Start one voxel away to avoid self occlusion
    vec3 startPos = Position_world + Normal_world * voxelWorldSize; // Plus move away slightly in the normal direction to avoid
                                                                    // self occlusion in flat surfaces

    while(dist < MAX_DIST && alpha < ALPHA_THRESH) {
        // smallest sample diameter possible is the voxel size
        float diameter = max(voxelWorldSize, 2.0 * tanHalfAngle * dist);
        float lodLevel = log2(diameter / voxelWorldSize);
        vec4 voxelColor = sampleVoxels(startPos + dist * direction, lodLevel);

        // front-to-back compositing
        float a = (1.0 - alpha);
        color += a * voxelColor.rgb;
        alpha += a * voxelColor.a;
        //occlusion += a * voxelColor.a;
        occlusion += (a * voxelColor.a) / (1.0 + 0.03 * diameter);
        dist += diameter * 0.5; // smoother
        //dist += diameter; // faster but misses more voxels
    }

    return vec4(color, alpha);
}

vec4 indirectLight(out float occlusion_out) {
    vec4 color = vec4(0);
    occlusion_out = 0.0;

    for(int i = 0; i < NUM_CONES; i++) {
        float occlusion = 0.0;
        // 60 degree cones -> tan(30) = 0.577
        // 90 degree cones -> tan(45) = 1.0
        color += coneWeights[i] * coneTrace(tangentToWorld * coneDirections[i], 0.577, occlusion);
        occlusion_out += coneWeights[i] * occlusion;
    }

    occlusion_out = 1.0 - occlusion_out;

    return color;
}

vec3 calcBumpNormal() {
    // Calculate gradients
    vec2 offset = vec2(1.0) / HeightTextureSize;
    float curr = texture(HeightTexture, UV).r;
    float diffX = texture(HeightTexture, UV + vec2(offset.x, 0.0)).r - curr;
    float diffY = texture(HeightTexture, UV + vec2(0.0, offset.y)).r - curr;

    // Tangent space bump normal
    float bumpMult = -3.0;
    vec3 bumpNormal_tangent = normalize(vec3(bumpMult*diffX, 1.0, bumpMult*diffY));

    return normalize(tangentToWorld * bumpNormal_tangent);
}

void main() {
    vec4 materialColor = texture(DiffuseTexture, UV);
    float alpha = materialColor.a;

    if(alpha < 0.5) {
        discard;
    }
    
    tangentToWorld = inverse(transpose(mat3(Tangent_world, Normal_world, Bitangent_world)));

    // Normal, light direction and eye direction in world coordinates
    vec3 N = calcBumpNormal();
    vec3 L = LightDirection;
    vec3 E = normalize(EyeDirection_world);
    
    // Calculate diffuse light
    vec3 diffuseReflection;
    {
        // Shadow map
        float visibility = texture(ShadowMap, vec3(Position_depth.xy, (Position_depth.z - 0.0005)/Position_depth.w));

        // Direct diffuse light
        float cosTheta = max(0, dot(N, L));
        vec3 directDiffuseLight = ShowDiffuse > 0.5 ? vec3(visibility * cosTheta) : vec3(0.0);

        // Indirect diffuse light
		float occlusion = 0.0;
        vec3 indirectDiffuseLight = indirectLight(occlusion).rgb;
        indirectDiffuseLight = ShowIndirectDiffuse > 0.5 ? 4.0 * indirectDiffuseLight : vec3(0.0);

        // Sum direct and indirect diffuse light and tweak a little bit
        occlusion = min(1.0, 1.5 * occlusion); // Make occlusion brighter
        diffuseReflection = 2.0 * occlusion * (directDiffuseLight + indirectDiffuseLight) * materialColor.rgb;
    }
    
    // Calculate specular light
    vec3 specularReflection;
    {
        vec4 specularColor = texture(SpecularTexture, UV);
        // Some specular textures are grayscale:
        specularColor = length(specularColor.gb) > 0.0 ? specularColor : specularColor.rrra;
        vec3 reflectDir = normalize(-E - 2.0 * dot(-E, N) * N);

        // Maybe fix so that the cone doesnt trace below the plane defined by the surface normal.
        // For example so that the floor doesnt reflect itself when looking at it with a small angle
        float specularOcclusion;
        vec4 tracedSpecular = coneTrace(reflectDir, 0.07, specularOcclusion); // 0.2 = 22.6 degrees, 0.1 = 11.4 degrees, 0.07 = 8 degrees angle
        specularReflection = ShowIndirectSpecular > 0.5 ? 2.0 * specularColor.rgb * tracedSpecular.rgb : vec3(0.0);
    }

    color = vec4(diffuseReflection + specularReflection, alpha);
}