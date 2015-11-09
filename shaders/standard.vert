#version 330 core

layout(location = 0) in vec3 vertexPosition_model;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_model;
layout(location = 3) in vec3 vertexTangent_model;
layout(location = 4) in vec3 vertexBitangent_model;

out vec2 UV;
out vec3 Normal_cam;
out vec3 Tangent_cam;
out vec3 Bitangent_cam;
out vec3 Position_cam;
out vec3 LightDirection_tangent;
out vec3 EyeDirection_tangent;
out vec3 LightDirection_cam;
out vec3 EyeDirection_cam;
out vec4 Position_depth;

uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 DepthModelViewProjectionMatrix;

void main() {
	gl_Position =  ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition_model,1);
	Position_depth = DepthModelViewProjectionMatrix * vec4(vertexPosition_model, 1);
	Position_depth.xyz = Position_depth.xyz * 0.5f + 0.5f;
	vec3 position_world = (ModelMatrix * vec4(vertexPosition_model,1)).xyz;
	Position_cam = (ModelViewMatrix * vec4(vertexPosition_model, 1)).xyz;
	Normal_cam = normalize((ModelViewMatrix * vec4(vertexNormal_model,0)).xyz);
	Tangent_cam = normalize((ModelViewMatrix * vec4(vertexTangent_model,0)).xyz);
	//Bitangent_cam = -normalize(cross(Normal_cam, Tangent_cam));
	Bitangent_cam = normalize((ModelViewMatrix * vec4(vertexBitangent_model,0)).xyz);

	EyeDirection_cam = vec3(0,0,0) - Position_cam; // Normalize in fragment shader or else it will be interpolated wrong

	vec3 lightPosition = vec3(70.0f, 160.0f, -20.0f);
	vec3 lightPosition_cam = (ViewMatrix * vec4(lightPosition, 1)).xyz;
	//LightDirection_cam = normalize(lightPosition_cam - Position_cam);
	LightDirection_cam = mat3(ViewMatrix) * normalize(vec3(1, 1, 0)); // Directional light

	UV = vertexUV;
}

