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

// bool showDiffuse = false, showIndirectDiffuse = false, showIndirectSpecular = false, showAmbientOcculision = false;
uniform float showDiffuse;
uniform float showIndirectDiffuse;
uniform float showIndirectSpecular;
uniform float showAmbientOcculision;

const int NUM_CONES = 6;
const float MAX_DIST = 200.0;
const float ALPHA_THRESH = 0.95;

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
vec3 worldNormal;

vec4 sampleVoxels(vec3 worldPosition, float lod) {
    vec3 offset = vec3(1.0 / VoxelDimensions, 1.0 / VoxelDimensions, 0); // Why??
    vec3 voxelTextureUV = worldPosition / (VoxelGridWorldSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5 + offset;
    return textureLod(VoxelTexture, voxelTextureUV, lod);
}

// Third argument to say how long between steps?
vec4 coneTrace(vec3 direction, float angle, out float occlusion) {
    // Returns the lod level that will be used if the texture is sampled automatically (with texture(..) )
    // float lodLevel = textureQueryLod(VoxelTexture, voxelTextureUV).x;
    // level 0 mipmap is full size, level 1 is half that size and so on

    float lod = 0.0;
    vec3 color = vec3(0);
    float alpha = 0.0;
    occlusion = 0.0;

    float voxelWorldSize = VoxelGridWorldSize / VoxelDimensions;
    float dist = voxelWorldSize; // Start one voxel away to avoid self occlusion

    while(dist < MAX_DIST && alpha < ALPHA_THRESH) {
        float radius = max(voxelWorldSize, angle * dist);
        float lodLevel = log2(radius / voxelWorldSize);
        vec4 voxelColor = sampleVoxels(Position_world + dist * direction, lodLevel);
        float a = (1.0 - alpha);
        color += a * voxelColor.rgb;
        alpha += a * voxelColor.a;
        occlusion += (a * voxelColor.a) / (1.0 + 0.05 * radius);
        dist += radius * 2.0;
    }

    return vec4(color, alpha);
}

vec4 indirectLight(out float occlusion_out) {
    vec4 color = vec4(0);
    occlusion_out = 0.0;

    for(int i = 0; i < NUM_CONES; i++) {
        int idx = i;
        float occlusion = 0.0;
        color += coneWeights[idx] * coneTrace(tangentToWorld * coneDirections[idx], 1.5, occlusion); // tan(60) = sqrt(3) = 1.732
        occlusion_out += coneWeights[idx] * occlusion;
    }

    occlusion_out = 1.0 - occlusion_out;

    return color;
}

void main() {
    vec4 materialColor = texture(DiffuseTexture, UV);
    if(materialColor.a < 0.5) {
        discard;
    }
    
    // Calculate gradients
    vec2 offset = vec2(1.0) / HeightTextureSize;
    float curr = texture(HeightTexture, UV).r;
    float diffX = texture(HeightTexture, UV + vec2(offset.x, 0.0)).r - curr;
    float diffY = texture(HeightTexture, UV + vec2(0.0, offset.y)).r - curr;

    // Tangent space bump normal
    float bumpMult = -3.0;
    vec3 bumpNormal_tangent = normalize(vec3(bumpMult*diffX, 1.0, bumpMult*diffY));

    // Matrix to convert from tangent space to camera space
    mat3 camToTangent = transpose(mat3(Tangent_cam, Normal_cam, Bitangent_cam));
    mat3 tangentToCam = inverse(camToTangent);
    tangentToWorld = inverse(transpose(mat3(Tangent_world, Normal_world, Bitangent_world)));

    // Camera space normal, light direction and eye direction
    vec3 N = normalize(tangentToCam * bumpNormal_tangent);
    vec3 L = normalize(LightDirection_cam);
    vec3 E = normalize(EyeDirection_cam);

    vec3 N_world = normalize(tangentToWorld * bumpNormal_tangent);
    vec3 E_world = normalize(tangentToWorld * camToTangent * EyeDirection_cam);
    worldNormal = N_world;
    

    // // bool showDiffuse = false, showIndirectDiffuse = false, showIndirectSpecular = false, showAmbientOcculision = false;

    // Shadow map
    float visibility = texture(ShadowMap, vec3(Position_depth.xy, (Position_depth.z - 0.0005)/Position_depth.w));
    //visibility = 1.0; // Don't use visibility ATM

    // Variables for direct light
    vec4 specularColor = texture(SpecularTexture, UV);
    specularColor = length(specularColor.gb) > 0.0 ? specularColor : specularColor.rrra;
    float shininess = pow(max(0.0, dot(reflect(-L, N), E)), 0.5);
    float cosTheta = max(0, dot(N, L));
    vec3 lightColor = vec3(1.0);
    float alpha = materialColor.a; // Is the alpha variable used?

    // Calculate direct light
    //vec3 ambientLighting = 0.1 * materialColor.xyz;
    vec3 diffuseLight = showDiffuse > 0.5 ? 1.0 * visibility * cosTheta * lightColor : vec3(0.0);
    //vec3 specularReflection = visibility * lightColor * specularColor.xyz * shininess;

    // Variables for indirect light
    vec3 reflectDir = normalize(-E_world - 2.0 * dot(-E_world, N_world) * N_world);
    cosTheta = max(0, dot(N_world, reflectDir));

    // Calculate indirect light
    float occlusion = 0.0;
    vec4 indirectReflection =  indirectLight(occlusion);
    vec3 indirectDiffuseLight = showIndirectDiffuse > 0.5 ? 6.0 * indirectReflection.rgb : vec3(0.0);
    float occlusionThresh = 0.25;
    //occlusion = showAmbientOcculision > 0.5 ? smoothstep(0.0, 1.0 - occlusionThresh, occlusion) : 1.0;
    vec3 diffuseReflection = occlusion * (diffuseLight + indirectDiffuseLight) * materialColor.rgb;
    
    float specularOcclusion;
    vec4 tracedSpecular = coneTrace(reflectDir, 0.2, specularOcclusion);
    vec3 specularReflection = showIndirectSpecular > 0.5 ? 6.0 * specularOcclusion * specularColor.rgb * tracedSpecular.rgb * shininess: vec3(0.0); // tan(30) = 1/sqrt(3)

    //color = vec4(occlusion*(ambientLighting + diffuseReflection + specularReflection), alpha);
    //color = vec4(vec3(occlusion), alpha);
    //color = vec4(indirectReflection.rgb, alpha);
    //color = vec4(voxelColor.rgb, alpha);
    //color = vec4(voxelTextureUV, alpha);
    //color = vec4(vec3(visibility), 1.0);
    color = vec4(diffuseReflection + specularReflection, alpha);
}