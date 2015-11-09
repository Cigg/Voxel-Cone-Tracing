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
    // For every vertex sent in vertices
    for(int i = 0;i < gl_in.length(); i++) {
        frag.UV = vertices[i].UV;
        
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    
    // Finished creating vertices
    EndPrimitive();
}