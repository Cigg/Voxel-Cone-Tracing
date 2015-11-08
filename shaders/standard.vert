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

uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec3 LightPosition;

void main() {
	gl_Position =  ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition_model,1);
	Position_cam = (ModelViewMatrix * vec4(vertexPosition_model, 1)).xyz;
	Normal_cam = normalize((ModelViewMatrix * vec4(vertexNormal_model,0)).xyz);
	Tangent_cam = normalize((ModelViewMatrix * vec4(vertexTangent_model,0)).xyz);
	//Bitangent_cam = -normalize(cross(Normal_cam, Tangent_cam));
	Bitangent_cam = normalize((ModelViewMatrix * vec4(vertexBitangent_model,0)).xyz);

	// Transform from view/camera space to tangent space
	mat3 camToTangent = transpose(mat3(Tangent_cam, Bitangent_cam, Normal_cam));

	vec3 eyeDirection_cam = normalize(vec3(0,0,0) - Position_cam);
	EyeDirection_tangent = camToTangent * eyeDirection_cam; // Direction to eye from current vertex

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	//vec3 lightPosition = vec3(70.0, 160.0, -20.0);
	vec3 lightPosition_cam = (ViewMatrix * vec4(LightPosition, 1)).xyz;
	vec3 lightDirection_cam = normalize(lightPosition_cam - Position_cam);
	LightDirection_tangent = normalize(camToTangent * lightDirection_cam);

	UV = vertexUV;
}

