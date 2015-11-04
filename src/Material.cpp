#include <iostream>
#include <assimp/scene.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Material.h"

Material::Material() {
	shader_ = 0;
}

Material::~Material() {
	
}

void Material::loadAssimpMaterial(const aiMaterial* material, std::string path) {
	if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString texturePath;
		if(material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = path + texturePath.data;
			std::replace( fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' with '/'
			diffuseTexture_ = loadTexture(fullPath);
		}
	}
}

void Material::setShader(GLuint shader) {
	shader_ = shader;
}

GLuint Material::bindMaterial() {
	if(shader_ != 0) {
		glUseProgram(shader_);

		int diffuseTextureIndex = 0;
		glActiveTexture(GL_TEXTURE0 + diffuseTextureIndex);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture_);
		glUniform1i(glGetUniformLocation(shader_, "diffuseTexture"), diffuseTextureIndex);
	}

	return shader_;
}

GLuint Material::loadTexture(std::string filenameString) {
    const char* filename = filenameString.c_str();

    int width, height, componentsPerPixel;
    GLubyte* textureData = stbi_load(filename, &width, &height, &componentsPerPixel, 0);

    // Generate a texture ID and bind to it
    GLuint tempTextureID;
    glGenTextures(1, &tempTextureID);
    glBindTexture(GL_TEXTURE_2D, tempTextureID);

    if(componentsPerPixel == 4) {
	    glTexImage2D(GL_TEXTURE_2D,    // Type of texture
	                 0,                // Mipmap level (0 being the top level i.e. full size)
	                 GL_RGBA,          // Internal format
	                 width,       // Width of the texture
	                 height,      // Height of the texture,
	                 0,                // Border in pixels
	                 GL_RGBA,          // Data format
	                 GL_UNSIGNED_BYTE, // Type of texture data
	                 textureData);     // The image data to use for this texture
	} 
	else if(componentsPerPixel == 3) {
		glTexImage2D(GL_TEXTURE_2D,    // Type of texture
	                 0,                // Mipmap level (0 being the top level i.e. full size)
	                 GL_RGB,          // Internal format
	                 width,       // Width of the texture
	                 height,      // Height of the texture,
	                 0,                // Border in pixels
	                 GL_RGB,          // Data format
	                 GL_UNSIGNED_BYTE, // Type of texture data
	                 textureData);     // The image data to use for this texture

	}else if(componentsPerPixel == 1) {
		glTexImage2D(GL_TEXTURE_2D,    // Type of texture
	                 0,                // Mipmap level (0 being the top level i.e. full size)
	                 GL_RED,          // Internal format
	                 width,       // Width of the texture
	                 height,      // Height of the texture,
	                 0,                // Border in pixels
	                 GL_RED,          // Data format
	                 GL_UNSIGNED_BYTE, // Type of texture data
	                 textureData);     // The image data to use for this texture
	}

	if(componentsPerPixel == 4 || componentsPerPixel == 3 || componentsPerPixel == 1) {
	    // Specify our minification and magnification filters
	    glGenerateMipmap(GL_TEXTURE_2D);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

    return tempTextureID;
}