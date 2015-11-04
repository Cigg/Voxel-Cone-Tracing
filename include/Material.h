#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>

#include <string>

class Material {
public:
	Material();
	~Material();

	void loadAssimpMaterial(const aiMaterial* material, std::string path);
	void setShader(GLuint shaderID);
	GLuint loadTexture(std::string filenameString);
	GLuint bindMaterial();


protected:
	std::string name_;
	GLuint shader_;

	GLuint diffuseTexture_;
	// more textures
};

#endif // MATERIAL_H