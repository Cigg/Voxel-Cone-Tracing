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

void Object::setPosition(glm::vec3 pos) {
	position_ = pos;
}

void Object::setScale(float scale) {
	scale_ = scale;
}

void Object::draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix) {
	GLuint shader = material_->bindMaterial();

	if(shader != 0) {
		glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_)), position_);
		glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);


		mesh_->draw();
	}
}

// This is a bit stupid. Make drawing of objects more general somehow
void Object::drawSimple(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, GLuint shader) {
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_)), position_);
	glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjectionMatrix"), 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);

	mesh_->draw();
}