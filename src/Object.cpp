#include <iostream>

#include "Camera.h"
#include "VCTApplication.h"

#include "Object.h"

Object::Object() {
	material_ = NULL;
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

void Object::draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, glm::mat4 &depthViewProjectionMatrix, GLuint shader) {
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_)), position_);
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 depthModelViewProjectionMatrix = depthViewProjectionMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "DepthModelViewProjectionMatrix"), 1, GL_FALSE, &depthModelViewProjectionMatrix[0][0]);

	// Load material specific stuff
	// If there is no material textures will still be bound from previously drawn object.
	// All objects should probably have a material and glBindTexture(GL_TEXTURE_2D, 0)
	// if it doesnt have any textures.
	if(material_) {
		material_->bindMaterial(shader);
	}

	mesh_->draw();
}

void Object::drawToDepth(glm::mat4 &depthViewProjectionMatrix, GLuint shader) {
	glUseProgram(shader);

	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_)), position_);
	glm::mat4 modelViewProjectionMatrix = depthViewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjectionMatrix"), 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);

	mesh_->draw();
}

void Object::drawTo3DTexture(GLuint shader, glm::mat4 &depthViewProjectionMatrix) {
    material_->bindMaterial(shader);

    // Matrix to transform to light position
    glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_)), position_);
    glm::mat4 depthModelViewProjectionMatrix = depthViewProjectionMatrix * modelMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shader, "DepthModelViewProjectionMatrix"), 1, GL_FALSE, &depthModelViewProjectionMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
    
    mesh_->draw();
}