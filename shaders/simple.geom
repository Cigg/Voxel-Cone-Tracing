#version 330 core

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

void main() {
    vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = normalize(cross(p1,p2));

    float nDotX = abs(dot(normal, vec3(1.0,0.0,0.0)));
    float nDotY = abs(dot(normal, vec3(0.0,1.0,0.0)));
    float nDotZ = abs(dot(normal, vec3(0.0,0.0,1.0)));

    // 0 = x axis dominant, 1 = y axis dominant, 2 = z axis dominant
    int domAxis = (nDotX >= nDotY && nDotX >= nDotZ) ? 0 : (nDotY >= nDotX && nDotY >= nDotZ) ? 1 : 2;
    
    mat3 projectionMatrix = mat3(
                                 domAxis == 1 ? 1 : domAxis == 2 ? 1 : 0, 0.0, 0.0,
                                 0.0, domAxis == 0 ? 1 : domAxis == 2 ? 1 : 0, 0.0,
                                 0.0, 0.0, domAxis == 0 ? 1 : domAxis == 1 ? 1 : 0
                                 );
    
    // For every vertex sent in vertices
    // Pass through code
    for(int i = 0;i < gl_in.length(); i++) {
        frag.UV = vertices[i].UV;
        gl_Position = mat4(projectionMatrix) * gl_in[i].gl_Position;
        EmitVertex();
    }
    
    // Finished creating vertices
    EndPrimitive();
}