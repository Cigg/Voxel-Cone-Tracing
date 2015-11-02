#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>

#include <string>

class Material {
public:
	Material(const std::string &vert, const std::string &frag, const std::string &name);
	~Material();

	GLuint useMaterial();	


protected:
	const std::string shadersLocation_ = "../shaders/";

	std::string name_;
	GLuint shader_;
};

#endif // MATERIAL_H