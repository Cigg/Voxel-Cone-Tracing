#version 330 core

layout(location = 0) in vec3 vertexPosition_model;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_model;
layout(location = 3) in vec3 vertexTangent_model;
layout(location = 4) in vec3 vertexBitangent_model;

out vec2 UV;
out vec3 Position_world;
out vec3 Normal_world;
out vec3 Tangent_world;
out vec3 Bitangent_world;
out vec3 Normal_cam;
out vec3 Tangent_cam;
out vec3 Bitangent_cam;
out vec3 Position_cam;
out vec3 LightDirection_tangent;
out vec3 EyeDirection_tangent;
out vec3 LightDirection_cam;
out vec3 EyeDirection_cam;
out vec4 Position_depth;

uniform vec3 LightDirection;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 DepthModelViewProjectionMatrix;

void main() {
	gl_Position =  ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition_model,1);

	Position_world = (ModelMatrix * vec4(vertexPosition_model,1)).xyz;

	Position_depth = DepthModelViewProjectionMatrix * vec4(vertexPosition_model, 1);
	Position_depth.xyz = Position_depth.xyz * 0.5 + 0.5;

	Position_cam = (ModelViewMatrix * vec4(vertexPosition_model, 1)).xyz;

	Normal_cam = normalize((ModelViewMatrix * vec4(vertexNormal_model,0)).xyz);
	Tangent_cam = normalize((ModelViewMatrix * vec4(vertexTangent_model,0)).xyz);
	Bitangent_cam = normalize((ModelViewMatrix * vec4(vertexBitangent_model,0)).xyz);

	Normal_world = normalize((ModelMatrix * vec4(vertexNormal_model,0)).xyz);
	Tangent_world = normalize((ModelMatrix * vec4(vertexTangent_model,0)).xyz);
	Bitangent_world = normalize((ModelMatrix * vec4(vertexBitangent_model,0)).xyz);

	EyeDirection_cam = vec3(0,0,0) - Position_cam; // Normalize in fragment shader or else it will be interpolated wrong

	LightDirection_cam = mat3(ViewMatrix) * normalize(LightDirection); // Directional light

	UV = vertexUV;
}