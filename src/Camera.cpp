#include <iostream>

#include "Camera.h"

Camera::Camera(glm::vec3 pos, float yaw, float pitch, glm::vec3 up, float fieldOfView, float aspectRatio, float near, float far) {
	position_ = pos;
	worldUpVector_ = up;

	yaw_ = yaw;
	pitch_ = pitch;
	roll_ = 0.0f;

	fieldOfView_ = fieldOfView;
	aspectRatio_ = aspectRatio;
	near_ = near;
	far_ = far;
}

Camera::~Camera() {
	
}

void Camera::update() {
	// Update camera coordinate system vectors
	front_.x = cos(yaw_) * cos(pitch_);
	front_.y = sin(pitch_);
	front_.z = sin(yaw_) * cos(pitch_);
	front_ = glm::normalize(front_);
	right_ = glm::normalize(glm::cross(front_, worldUpVector_));
	up_ = glm::normalize(glm::cross(right_, front_));

	// Update view matrix
	viewMatrix_ = glm::lookAt(position_, position_ + front_, up_);

	// Update projection matrix
	projectionMatrix_ = glm::perspective(fieldOfView_, aspectRatio_, near_, far_);
}

glm::mat4 Camera::getViewMatrix() {
	return viewMatrix_;
}

glm::mat4 Camera::getProjectionMatrix() {
	return projectionMatrix_;	
}

glm::vec3 Camera::getPosition() {
	return position_;	
}

glm::vec3 Camera::getDirection() {
	return front_;
}

void Camera::setPosition(glm::vec3 pos) {
	position_ = pos;
}

void Camera::setDirection(glm::vec3 dir) {
	front_ = dir;
}

void Camera::moveForward(float delta) {
	position_ += front_ * delta;
}

void Camera::moveBackward(float delta) {
	position_ -= front_ * delta;
}

void Camera::moveRight(float delta) {
	position_ += right_ * delta;
}

void Camera::moveLeft(float delta) {
	position_ -= right_ * delta;
}

void Camera::moveUp(float delta) {
	position_ += worldUpVector_ * delta;
}

void Camera::moveDown(float delta) {
	position_ -= worldUpVector_ * delta;
}

void Camera::addYaw(float yaw) {
	yaw_ += yaw;
}

void Camera::addPitch(float pitch) {
	pitch_ += pitch;
	if(pitch_ > 3.14f / 2.0f)
		pitch_ = 3.14f / 2.0f;
	if(pitch_ < -3.14f / 2.0f)
		pitch_ = -3.14f / 2.0f;
}