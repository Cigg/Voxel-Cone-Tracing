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

void Object::draw(VCTApplication* app) {
	GLuint shader = material_->bindMaterial();

	if(shader != 0) {
		Camera* cam = app->getCamera();

		glm::mat4 viewMatrix = cam->getViewMatrix();
		glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_)), position_);
		glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
		glm::mat4 projectionMatrix = cam->getProjectionMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);

		glm::vec3 lightPosition = app->lightPosition_;
		glm::vec3 lightColor = app->lightColor_;
		glUniform3f(glGetUniformLocation(shader, "LightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(shader, "LightColor"), lightColor.x, lightColor.y, lightColor.z);

		mesh_->draw();
	}
}