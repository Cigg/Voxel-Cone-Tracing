#include <GLFW/glfw3.h>

#include "VCTApplication.h"
#include "Controls.h"

Controls::Controls(float speed, float mouseSensitivity) {
	speed_ = speed;
	mouseSensitivity_ = mouseSensitivity;
	frameCount_ = 0;
	oldMousePos_ = glm::dvec2(0.0);
}

Controls::~Controls() {
	
}

void Controls::updateFromInputs(VCTApplication* app, float deltaTime) {
	GLFWwindow* window = app->getWindow();
	Camera* camera = app->getCamera();

	glm::dvec2 mousePos;
	glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
	glm::dvec2 mouseDelta = mousePos - oldMousePos_;
	oldMousePos_ = mousePos;

	// Mouse cursor position isn't updated the first four frames.
	// This is an ugly hack to wait until it is positioned in the middle
	// of the screen
	if(frameCount_ > 4) {
		camera->addYaw(mouseDelta.x * mouseSensitivity_);
		camera->addPitch(-1.0 * mouseDelta.y * mouseSensitivity_);
	}
	else {
		frameCount_++;
	}

	float translationDelta = deltaTime * speed_;

	// Move forward
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->moveForward(translationDelta);
	}
	// Move backward
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->moveBackward(translationDelta);
	}
	// Strafe right
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->moveRight(translationDelta);
	}
	// Strafe left
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->moveLeft(translationDelta);
	}
	// Move up
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera->moveUp(translationDelta);
	}
	// Move down
	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera->moveDown(translationDelta);
	}
}
