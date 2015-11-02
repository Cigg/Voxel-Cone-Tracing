#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	Camera(glm::vec3 pos, float yaw, float pitch, glm::vec3 up, float fieldOfView, float aspectRatio, float near, float far);
	~Camera();

	void update();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::vec3 getPosition();
	glm::vec3 getDirection();

	void setPosition(glm::vec3 pos);
	void setDirection(glm::vec3 dir);
	void moveForward(float delta);
	void moveBackward(float delta);
	void moveRight(float delta);
	void moveLeft(float delta);
	void moveUp(float delta);
	void moveDown(float delta);
	void addYaw(float yaw);
	void addPitch(float pitch);

protected:
	glm::mat4 viewMatrix_;
	glm::mat4 projectionMatrix_;

	// Camera position and rotation
	glm::vec3 position_;
	float yaw_;
	float pitch_;
	float roll_;

	// Camera coordinate system vectors
	glm::vec3 front_;
	glm::vec3 right_;
	glm::vec3 up_;

	// Other camera properties
	glm::vec3 worldUpVector_;
	float fieldOfView_;
	float aspectRatio_;
	float near_;
	float far_;
};

#endif // CAMERA_H