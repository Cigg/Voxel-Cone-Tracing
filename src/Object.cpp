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
	glUseProgram(shader);
	
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_)), position_);
	glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjectionMatrix"), 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);

	mesh_->draw();
}

void Object::drawTo3DTexture(GLuint shader, GLuint texID) {
    material_->bindMaterial();
    glUseProgram(shader);
    
    // left, right, bottom, top, zNear, zFar
    glm::mat4 projectionMatrix = glm::ortho(-1.5, 1.5, -1.5, 1.5, 0.0, 1000.0);
    glm::mat4 projX = projectionMatrix * glm::lookAt(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projY = projectionMatrix * glm::lookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
    glm::mat4 projZ = projectionMatrix * glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    
    glUniformMatrix4fv(glGetUniformLocation(shader, "projX"), 1, GL_FALSE, &projX[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projY"), 1, GL_FALSE, &projY[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projZ"), 1, GL_FALSE, &projZ[0][0]);
    
    // Bind single level of texture to image unit so we can write to it from shaders
    glBindTexture(GL_TEXTURE_3D, texID);
    glBindImageTexture(0, texID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glUniform1i(glGetUniformLocation(shader, "voxelTexture"), 0);
    
    mesh_->draw();
}