#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

struct Texture {
	GLuint textureID;
	int width, height, componentsPerPixel;
};

#endif // TEXTURE_H