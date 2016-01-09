#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>
#include <iostream>

class VCTApplication; // Forward declaration

#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

class Object {
public:
	Object();
	~Object();

	bool loadMeshFromFile(const std::string &path);
	void setPosition(glm::vec3 pos);
	void setScale(float scale);
	void draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, glm::mat4 &depthModelViewProjectionMatrix, GLuint shader);
	void drawToDepth(glm::mat4 &depthViewProjectionMatrix, GLuint shader);
    void drawTo3DTexture(GLuint shader, glm::mat4 &depthViewProjectionMatrix);

	Mesh* mesh_;
	Material* material_;

protected:
	glm::vec3 position_;
	float scale_;
};

inline bool compareObjects(Object* obj1, Object* obj2) {
	if(!obj1->material_ || obj2->material_)
		return false;
	return (!obj1->material_->hasAlpha_ && obj2->material_->hasAlpha_);
}

#endif // OBJECT_H