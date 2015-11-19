#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

#include "Texture.h"

class Material {
public:
	enum TEXTURES_TYPES {
		DIFFUSE_TEXTURE,
		SPECULAR_TEXTURE,
		MASK_TEXTURE,
		HEIGHT_TEXTURE,
		NUM_TEXTURES
	};

	Material();
	~Material();

	void loadAssimpMaterial(const aiMaterial* material, std::string path);
	Texture loadTexture(std::string filenameString);
	void bindMaterial(GLuint shader);

	bool hasAlpha_; // Has an alpha channel in the diffuseTexture_ 
	std::string name_;

protected:
	// Material properties
	glm::vec3 ambientColor_;
	glm::vec3 diffuseColor_;
	glm::vec3 specularColor_;
	glm::vec3 emissiveColor_;
	float shininess_;
	float opacity_;

	Texture diffuseTexture_;
	Texture specularTexture_;
	Texture maskTexture_;
	Texture heightTexture_;

	// Not used
	// int illuminationModel_;
	//float refractionIndex_;
	//glm::vec3 transmissionFilter_;
};

#endif // MATERIAL_H