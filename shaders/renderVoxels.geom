#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

uniform float VoxelSize;
uniform int Dimensions;
uniform int TotalNumVoxels; // Dimensions^3

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

in vec4 color[];

out vec4 fragColor;
out vec3 normal_world;

void main() {
	fragColor = color[0];

	vec4 v1 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(-0.5, 0.5, 0.5, 0));
	vec4 v2 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(0.5, 0.5, 0.5, 0));
	vec4 v3 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(-0.5, -0.5, 0.5, 0));
	vec4 v4 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(0.5, -0.5, 0.5, 0));
	vec4 v5 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(-0.5, 0.5, -0.5, 0));
	vec4 v6 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(0.5, 0.5, -0.5, 0));
	vec4 v7 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(-0.5, -0.5, -0.5, 0));
	vec4 v8 = ProjectionMatrix * ModelViewMatrix * (gl_in[0].gl_Position + vec4(0.5, -0.5, -0.5, 0));

	//
	//      v5 _____________ v6
	//        /|           /|
	//       / |          / |
	//      /  |         /  |
	//     /   |        /   |
	// v1 /____|_______/ v2 |
	//    |    |       |    |
	//    |    |_v7____|____| v8
	//    |   /        |   /
	//    |  /         |  /  
	//    | /          | /  
	// v3 |/___________|/ v4
	//

	// TODO: Optimize
	// +Z
    normal_world = vec3(0, 0, 1);
    gl_Position = v1;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();
    gl_Position = v1;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();

    // -Z
    normal_world = vec3(0, 0, -1);
    gl_Position = v6;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v7;
    EmitVertex();
    EndPrimitive();
    gl_Position = v6;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v5;
    EmitVertex();
    EndPrimitive();

    // +X
    normal_world = vec3(1, 0, 0);
    gl_Position = v2;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v2;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -X
    normal_world = vec3(-1, 0, 0);
    gl_Position = v5;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v3;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v1;
    EmitVertex();
    EndPrimitive();

    // +Y
    normal_world = vec3(0, 1, 0);
    gl_Position = v5;
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -Y
    normal_world = vec3(0, -1, 0);
    gl_Position = v3;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v3;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();
}