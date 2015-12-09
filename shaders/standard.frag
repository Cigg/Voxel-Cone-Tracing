#version 400 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 voxelTextureUV;
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

uniform vec2 DiffuseTextureSize;
uniform vec2 SpecularTextureSize;
uniform vec2 MaskTextureSize;
uniform vec2 HeightTextureSize;

void main() {
	vec4 materialColor = texture(DiffuseTexture, UV);
    // level 0 mipmap is full size, level 1 is half that size and so on
    vec4 voxelColor = textureLod(VoxelTexture, voxelTextureUV, 0.0);
	float alpha = materialColor.a;

	vec3 lightColor = vec3(1.0f);
	
	if(materialColor.a < 0.5f) {
		discard;
	}

	vec4 specularColor = texture(SpecularTexture, UV);
	specularColor = length(specularColor.gb) > 0.0f ? specularColor : specularColor.rrra;

	// Calculate gradients
	vec2 offset = vec2(1.0f) / HeightTextureSize;
	float curr = texture(HeightTexture, UV).r;
    float diffX = texture(HeightTexture, UV + vec2(offset.x, 0.0f)).r - curr;
    float diffY = texture(HeightTexture, UV + vec2(0.0f, offset.y)).r - curr;

    // Tangent space bump normal
    float bumpMult = -3.0f;
    vec3 bumpNormal_tangent = normalize(vec3(bumpMult*diffX, bumpMult*diffY, 1.0f));

    // Matrix to convert from tangent space to camera space
    mat3 tangentToCam = inverse(transpose(mat3(Tangent_cam, Bitangent_cam, Normal_cam)));
    // Camera space normal, light direction and eye direction
    vec3 N = normalize(tangentToCam * bumpNormal_tangent);
    vec3 L = normalize(LightDirection_cam);
    vec3 E = normalize(EyeDirection_cam);

    float cosTheta = max(0, dot(N, L));

    float visibility = texture(ShadowMap, vec3(Position_depth.xy, (Position_depth.z - 0.001)/Position_depth.w));
    
    vec3 ambientLighting = 0.2f * materialColor.xyz;
    vec3 diffuseReflection = visibility * cosTheta * lightColor * materialColor.xyz;
    vec3 specularReflection = visibility * lightColor * specularColor.xyz * pow(max(0.0, dot(reflect(-L, N), E)), Shininess);

	//color = vec4(ambientLighting + diffuseReflection + specularReflection, alpha);
    color = vec4(voxelColor.rgb, alpha);
    //color = vec4(voxelTextureUV, alpha);
    //color = vec4(vec3(visibility), 1.0);
}