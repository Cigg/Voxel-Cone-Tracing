#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class VCTApplication; // Forward declaration

class Controls {
public:
	Controls(float speed, float mouseSensitivity);
	~Controls();

	void updateFromInputs(VCTApplication* app, float deltaTime);

protected:
	// Settings
	float speed_;
	float mouseSensitivity_;

	glm::dvec2 oldMousePos_;
	int frameCount_;
};

#endif // CONTROLS_H