#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Shader.h"
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
	controls_ = new Controls(10.0f, 0.15f);

    GLuint standardShader = loadShaders("../shaders/simple.vert", "../shaders/simple.frag", "../shaders/simple.geom");
//	GLuint standardShader = loadShaders("../shaders/standard.vert", "../shaders/standard.frag");
	shadowShader_ = loadShaders("../shaders/shadow.vert", "../shaders/shadow.frag");
	quadShader_ = loadShaders("../shaders/quad.vert", "../shaders/quad.frag");

	// Load Crytek Sponza
	std::cout << "Loading objects..." << std::endl;
	Assimp::Importer importer;
	// Read file and store as a "scene"
	std::string crytekPath = "../data/models/crytek-sponza/";
	const aiScene* scene = importer.ReadFile(crytekPath + "sponza.obj", aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices);
	if(scene) {
		Material* mat;
		Object* obj;
		Mesh* mesh;

		// Create a materials from the loaded assimp materials
		for(unsigned int m = 0; m < scene->mNumMaterials; m++) {
			mat = new Material();
			mat->loadAssimpMaterial(scene->mMaterials[m], crytekPath);
			// Use standard shader for now
			mat->setShader(standardShader);
			materials_[m] = mat;
		}

		// Create objects and add to objects_ vector. An object has a mesh, a material and some other properties.
		for(unsigned int m = 0; m < scene->mNumMeshes; m++) {
			// Create new object
			obj = new Object();

			// Create a mesh from the loaded assimp mesh
			mesh = new Mesh();
			mesh->loadAssimpMesh(scene->mMeshes[m]);
			// Asign the object this mesh.
			obj->mesh_ = mesh;

			// Store pointer to material used
			obj->material_ = materials_[scene->mMeshes[m]->mMaterialIndex];

			obj->setScale(0.05f);
			objects_.push_back(obj);

		}
	}
	else {
		std::cerr << "Mesh: " << importer.GetErrorString() << std::endl;
		return false;	
	}

	std::cout << "Loading done! " << objects_.size() << " objects loaded" << std::endl;

	// Sort object so opaque objects are rendered first
	std::sort(objects_.begin(), objects_.end(), compareObjects);

	// Create framebuffer for shadow map
	glGenFramebuffers(1, &depthFramebuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);

	// Depth texture
	glGenTextures(1, &depthTexture_);
	glBindTexture(GL_TEXTURE_2D, depthTexture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_, 0);
	glDrawBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error creating framebuffer" << std::endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Quad FBO
	static const GLfloat quad[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &quadVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	return true;
}

void VCTApplication::update(float deltaTime) {
	controls_->updateFromInputs(this, deltaTime);
	camera_->update();
}

void VCTApplication::draw() {
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glUseProgram(shadowShader_);
	// glViewport(0,0,1024,1024);

	// glm::mat4 viewMatrix = camera_->getViewMatrix();//glm::lookAt(glm::vec3(0.5f,2,2), glm::vec3(0,0,0), glm::vec3(0,1,0));
	// glm::mat4 projectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -50, 2000);

	// for(std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
	// 	(*obj)->drawSimple(viewMatrix, projectionMatrix, shadowShader_);
	// }

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// // Draw framebuffer to screen
	// glUseProgram(quadShader_);
//	glViewport(0,0,1024,768);

	// // Bind our texture in Texture Unit 0
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, depthTexture_);
	// glUniform1i(glGetUniformLocation(quadShader_, "Texture"), 0);

	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
	// glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE, 0, (void*)0);

	// glDrawArrays(GL_TRIANGLES, 0, 6);
	// glDisableVertexAttribArray(0);


	glm::mat4 viewMatrix = camera_->getViewMatrix();
	glm::mat4 projectionMatrix = camera_->getProjectionMatrix();

	for(std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
		(*obj)->draw(viewMatrix, projectionMatrix);
	}
}