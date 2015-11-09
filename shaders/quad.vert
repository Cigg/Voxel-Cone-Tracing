#version 330 core

layout(location = 0) in vec3 VertexPosition_model;

out vec2 UV;

void main(){
	gl_Position =  vec4(VertexPosition_model, 1);
	UV = (VertexPosition_model.xy + vec2(1, 1))/2.0f;
}

