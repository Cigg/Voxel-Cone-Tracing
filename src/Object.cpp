#include <iostream>

#include "Camera.h"
#include "VCTApplication.h"

#include "Object.h"

Object::Object() {
	position_ = glm::vec3(0.0f);
	scale_ = 1.0f;
}

Object::~Object() {
	delete mesh_;
}

void Object::setMaterial(Material* mat) {
	material_ = mat;
}

void Object::setMesh(Mesh* mesh) {
	mesh_ = mesh;
}

void Object::setPosition(glm::vec3 pos) {
	position_ = pos;
}

void Object::setScale(float scale) {
	scale_ = scale;
}

void Object::draw(VCTApplication* app) {
	GLuint shader = material_->useMaterial();
	if(shader != 0) {
		Camera* cam = app->getCamera();

		glm::mat4 modelViewMatrix = glm::translate(glm::scale(cam->getViewMatrix(), glm::vec3(scale_)), position_);
		glm::mat4 projectionMatrix = cam->getProjectionMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
		mesh_->draw();
	}
}