#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

struct Texture2D {
	GLuint textureID;
	int width, height, componentsPerPixel;
};

struct Texture3D {
	GLuint textureID;
	int size, componentsPerPixel;
};

#endif // TEXTURE_H