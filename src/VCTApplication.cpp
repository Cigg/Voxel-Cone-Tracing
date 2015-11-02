#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "VCTApplication.h"

VCTApplication::VCTApplication(const int width, const int height, GLFWwindow* window) {
	width_ = width;
	height_ = height;
	window_ = window;
	camera_ = NULL;
	controls_ = NULL;
}

VCTApplication::~VCTApplication() {
	if(camera_)
		delete camera_;
	if(controls_)
		delete controls_;

	for (std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
		delete (*obj);
   	} 
	objects_.clear();
}

int VCTApplication::getWindowWidth() {
	return width_;
}

int VCTApplication::getWindowHeight() {
	return height_;
}

GLFWwindow* VCTApplication::getWindow() {
	return window_;
}

Camera* VCTApplication::getCamera() {
	return camera_;
}

bool VCTApplication::initialize() {
	std::cout << "Initializing TestApplication" << std::endl;

	// Init camera parameters
	glm::vec3 pos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	float yaw = -3.1415f / 2.0f;
	float pitch = 0.0f;

	// Camera position, camera direction, world up vector, aspect ratio, field of view, min distance, max distance
	camera_ = new Camera(pos, yaw, pitch, up, 45.0f, 4.0f/3.0f, 0.1f, 1000.0f);

	// Speed, Mouse sensitivity
	controls_ = new Controls(8.0f, 0.15f);

	Material* mat;
	Object* obj;
	Mesh* mesh;

	mat = new Material( "standard.vert", "standard.frag", "standard");
	materials_.push_back(mat);

	// Load Crytek Sponza
	std::cout << "Loading objects..." << std::endl;
	Assimp::Importer importer;
	// Read file and store as a "scene"
	const aiScene* scene = importer.ReadFile("../data/models/crytek-sponza/sponza.obj", aiProcess_Triangulate);
	if(scene) {
		//std::cout << "mNumMeshes: " << scene->mNumMeshes << std::endl;
		for(unsigned int i = 0; i < scene->mNumMeshes; i++) {
			mesh = new Mesh(scene->mMeshes[i]);
			
			obj = new Object();
			obj->setMaterial(mat);
			obj->setMesh(mesh);
			obj->setScale(0.05f);
			objects_.push_back(obj);
		}
	}
	else {
		std::cerr << "Mesh: " << importer.GetErrorString() << std::endl;
		return false;	
	}

	std::cout << "Loading done! " << objects_.size() << " number of objects loaded" << std::endl;

	return true;
}

void VCTApplication::update(float deltaTime) {
	controls_->updateFromInputs(this, deltaTime);
	camera_->update();
}

void VCTApplication::draw() {
	for(std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
		(*obj)->draw(this);
	}	
}