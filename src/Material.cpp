#include "Shader.h"

#include "Material.h"

Material::Material(const std::string &vert, const std::string &frag, const std::string &name) {
	name_ = name;
	std::string vertFilePath = shadersLocation_ + vert;
	std::string fragFilePath = shadersLocation_ + frag;
	shader_ = loadShaders(vertFilePath.c_str(), fragFilePath.c_str());
}

Material::~Material() {
	
}

GLuint Material::useMaterial() {
	if(shader_ != NULL) {
		glUseProgram(shader_);
		return shader_;
	}
	else {
		return 0;
	}
}