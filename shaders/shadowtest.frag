#version 330 core

out vec4 color;
 
void main(){
    // Not really needed, OpenGL does it anyway
    color = vec4(vec3((gl_FragCoord.z-0.98f)/0.02f), 1);
}