#version 330 core

#define M_PI 3.1415926535897932384626433832795

layout (triangles) in; // glDrawArrays is set to triangles so that's what we're working with
layout (triangle_strip, max_vertices = 3) out;

// Input from vertex shader, stored in an array
in vData {
    vec2 UV;
} vertices[];

// Data that will be sent to fragment shader
out fData {
    vec2 UV;
} frag;

uniform mat4 projX;
uniform mat4 projY;
uniform mat4 projZ;

void main() {
    vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = normalize(cross(p1,p2));

    float nDotX = abs(normal.x);
    float nDotY = abs(normal.y);
    float nDotZ = abs(normal.z);

    // 0 = x axis dominant, 1 = y axis dominant, 2 = z axis dominant
    int domAxis = (nDotX >= nDotY && nDotX >= nDotZ) ? 0 : (nDotY >= nDotX && nDotY >= nDotZ) ? 1 : 2;
    mat4 projectionMatrix = domAxis == 0 ? projX : domAxis == 1 ? projY : projZ;
    
    // For every vertex sent in vertices
    for(int i = 0;i < gl_in.length(); i++) {
        
        frag.UV = vertices[i].UV;
        gl_Position =  projectionMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }
    
    // Finished creating vertices
    EndPrimitive();
}