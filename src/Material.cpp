#include <iostream>
#include <assimp/scene.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Material.h"

Material::Material() {
	Texture2D tex;
	tex.width = tex.height = tex.textureID = 0;
	
	diffuseTexture_ = tex;
	specularTexture_ = tex;
	maskTexture_ = tex;
	heightTexture_ = tex;
	hasAlpha_ = true;
}

Material::~Material() {
	
}

void Material::loadAssimpMaterial(const aiMaterial* mat, std::string path) {
	aiString name;
	mat->Get(AI_MATKEY_NAME, name);
	name_ = name.data;

	std::cout << "\tLoading material with name: " << name_ << std::endl;

	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	ambientColor_ = glm::vec3(color.r, color.g, color.b);

	mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	specularColor_ = glm::vec3(color.r, color.g, color.b);

	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	diffuseColor_ = glm::vec3(color.r, color.g, color.b);

	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	emissiveColor_ = glm::vec3(color.r, color.g, color.b);

	mat->Get(AI_MATKEY_OPACITY, opacity_);

	mat->Get(AI_MATKEY_SHININESS, shininess_);

	if(mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		std::cout << "\t\tdiffuseTexture_ loaded" << std::endl;
		aiString texturePath;
		if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = path + texturePath.data;
			std::replace( fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' with '/'
			diffuseTexture_ = loadTexture(fullPath);
		}
	}

	if(mat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
		std::cout << "\t\tspecularTexture_ loaded" << std::endl;
		aiString texturePath;
		if(mat->GetTexture(aiTextureType_AMBIENT, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = path + texturePath.data;
			std::replace( fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' with '/'
			specularTexture_ = loadTexture(fullPath);
		}
	}

	if(mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
		std::cout << "\t\theightTexture_ loaded" << std::endl;
		aiString texturePath;
		if(mat->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = path + texturePath.data;
			std::replace( fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' with '/'
			heightTexture_ = loadTexture(fullPath);
		}
	}

	if(mat->GetTextureCount(aiTextureType_OPACITY) > 0) {
		std::cout << "\t\tmaskTexture_ loaded" << std::endl;
		aiString texturePath;
		if(mat->GetTexture(aiTextureType_OPACITY, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = path + texturePath.data;
			std::replace( fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' with '/'
			maskTexture_ = loadTexture(fullPath);
		}
	}
}

Texture2D Material::loadTexture(std::string filenameString) {
	Texture2D tex;

    const char* filename = filenameString.c_str();
    GLubyte* textureData = stbi_load(filename, &tex.width, &tex.height, &tex.componentsPerPixel, 0);

    if(!textureData) {
    	std::cout << "Couldn't load image: " << filename << std::endl;
    	return tex;
    }

    // Generate a texture ID and bind to it
    glGenTextures(1, &tex.textureID);
    glBindTexture(GL_TEXTURE_2D, tex.textureID);

    if(tex.componentsPerPixel == 4) {
	    glTexImage2D(GL_TEXTURE_2D,    // Type of texture
	                 0,                // Mipmap level (0 being the top level i.e. full size)
	                 GL_RGBA,          // Internal format
	                 tex.width,       // Width of the texture
	                 tex.height,      // Height of the texture,
	                 0,                // Border in pixels
	                 GL_RGBA,          // Data format
	                 GL_UNSIGNED_BYTE, // Type of texture data
	                 textureData);     // The image data to use for this texture
	} 
	else if(tex.componentsPerPixel == 3) {
		glTexImage2D(GL_TEXTURE_2D,    // Type of texture
	                 0,                // Mipmap level (0 being the top level i.e. full size)
	                 GL_RGB,          // Internal format
	                 tex.width,       // Width of the texture
	                 tex.height,      // Height of the texture,
	                 0,                // Border in pixels
	                 GL_RGB,          // Data format
	                 GL_UNSIGNED_BYTE, // Type of texture data
	                 textureData);     // The image data to use for this texture

	}else if(tex.componentsPerPixel == 1) {
		glTexImage2D(GL_TEXTURE_2D,    // Type of texture
	                 0,                // Mipmap level (0 being the top level i.e. full size)
	                 GL_RED,          // Internal format
	                 tex.width,       // Width of the texture
	                 tex.height,      // Height of the texture,
	                 0,                // Border in pixels
	                 GL_RED,          // Data format
	                 GL_UNSIGNED_BYTE, // Type of texture data
	                 textureData);     // The image data to use for this texture
	}

	if(tex.componentsPerPixel == 4 || tex.componentsPerPixel == 3 || tex.componentsPerPixel == 1) {
	    // Specify our minification and magnification filters
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glGenerateMipmap(GL_TEXTURE_2D);
	}

    // Check for OpenGL texture creation errors
    GLenum glError = glGetError();
    if(glError) {
        std::cout << "There was an error loading the texture: "<< filenameString << std::endl;
        switch (glError) {
            case GL_INVALID_ENUM:
                std::cout << "Invalid enum." << std::endl;
                break;

            case GL_INVALID_VALUE:
                std::cout << "Invalid value." << std::endl;
                break;

            case GL_INVALID_OPERATION:
                std::cout << "Invalid operation." << std::endl;

            default:
                std::cout << "Unrecognised GLenum." << std::endl;
                break;
        }
    }

    stbi_image_free(textureData);

    return tex;
}

void Material::bindMaterial(GLuint shader) {
	glUseProgram(shader);

	// glUniform3f(glGetUniformLocation(shader, "AmbientColor"), ambientColor_.r, ambientColor_.g, ambientColor_.b);
	// glUniform3f(glGetUniformLocation(shader, "DiffuseColor"), diffuseColor_.r, diffuseColor_.g, diffuseColor_.b);
	// glUniform3f(glGetUniformLocation(shader, "SpecularColor"), specularColor_.r, specularColor_.g, specularColor_.b);
	
	glUniform1f(glGetUniformLocation(shader, "Shininess"), shininess_);
	glUniform1f(glGetUniformLocation(shader, "Opacity"), opacity_);

	glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture_.textureID);
	glUniform1i(glGetUniformLocation(shader, "DiffuseTexture"), DIFFUSE_TEXTURE);
	glUniform2f(glGetUniformLocation(shader, "DiffuseTextureSize"), diffuseTexture_.width, diffuseTexture_.height);

	glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, specularTexture_.textureID);
	glUniform1i(glGetUniformLocation(shader, "SpecularTexture"), SPECULAR_TEXTURE);
	glUniform2f(glGetUniformLocation(shader, "SpecularTextureSize"), specularTexture_.width, specularTexture_.height);

	glActiveTexture(GL_TEXTURE0 + MASK_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, maskTexture_.textureID);
	glUniform1i(glGetUniformLocation(shader, "MaskTexture"), MASK_TEXTURE);
	glUniform2f(glGetUniformLocation(shader, "MaskTextureSize"), maskTexture_.width, maskTexture_.height);

	glActiveTexture(GL_TEXTURE0 + HEIGHT_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, heightTexture_.textureID);
	glUniform1i(glGetUniformLocation(shader, "HeightTexture"), HEIGHT_TEXTURE);
	glUniform2f(glGetUniformLocation(shader, "HeightTextureSize"), heightTexture_.width, heightTexture_.height);
}