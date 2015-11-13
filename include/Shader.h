#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

//GLuint loadShaders(const char* vert, const char* frag);
GLuint loadShaders(const char* vert, const char* frag, const char* geom = NULL);

#endif
