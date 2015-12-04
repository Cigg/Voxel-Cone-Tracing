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

bool VCTApplication::loadObject(std::string path, std::string name, glm::vec3 pos, float scale) {
	Assimp::Importer importer;

	// Read file and store as a "scene"
	const aiScene* scene = importer.ReadFile(path + name, aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices);
    
	if(scene) {
		Material* mat;
		Object* obj;
		Mesh* mesh;

		// Create a materials from the loaded assimp materials
		for(unsigned int m = 0; m < scene->mNumMaterials; m++) {
			mat = new Material();
			mat->loadAssimpMaterial(scene->mMaterials[m], path);
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

			obj->setScale(scale);
			obj->setPosition(pos);
			objects_.push_back(obj);
		}
	}
	else {
		std::cerr << "Mesh: " << importer.GetErrorString() << std::endl;
		return false;	
	}

	return true;
}

bool VCTApplication::initialize() {
	std::cout << "Initializing VCT" << std::endl;

	// Init camera parameters
	glm::vec3 pos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	float yaw = -3.1415f / 2.0f;
	float pitch = 0.0f;

	// Camera position, camera direction, world up vector, aspect ratio, field of view, min distance, max distance
	camera_ = new Camera(pos, yaw, pitch, up, 45.0f, 4.0f/3.0f, 0.1f, 1000.0f);

	// Speed, Mouse sensitivity
	controls_ = new Controls(10.0f, 0.0015f);
    
	standardShader_ = loadShaders("../shaders/standard.vert", "../shaders/standard.frag");
    voxelizationShader_ = loadShaders("../shaders/voxelization.vert", "../shaders/voxelization.frag", "../shaders/voxelization.geom");
    shadowShader_ = loadShaders("../shaders/shadow.vert", "../shaders/shadow.frag");
    quadShader_ = loadShaders("../shaders/quad.vert", "../shaders/quad.frag");
    renderVoxelsShader_ = loadShaders("../shaders/renderVoxels.vert", "../shaders/renderVoxels.frag", "../shaders/renderVoxels.geom");

    // Load objects
    std::cout << "Loading objects... " << std::endl;
    loadObject("../data/models/crytek-sponza/", "sponza.obj", glm::vec3(0.0f), sponzaScale_);
	//loadObject("../data/models/", "suzanne.obj");
    std::cout << "Loading done! " << objects_.size() << " objects loaded" << std::endl;

	// Sort object so opaque objects are rendered first
	std::sort(objects_.begin(), objects_.end(), compareObjects);
 
    // Create 3D texture to test that rendering of voxels work
    texture3DSize_ = voxelDimensions_;
    //texture3D_ = create3DTexture();
    // Create VAO for 3D texture. Won't really store any information but it's still needed.
	glGenVertexArrays(1, &texture3DVertexArray_);
    
    // ------------------------------------------------------------------- //
    // --------------------- Shadow map initialization ------------------- //
    // ------------------------------------------------------------------- //
	// Create framebuffer for shadow map
	glGenFramebuffers(1, &depthFramebuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);

	// Depth texture
	depthTexture_.width = depthTexture_.height = 1024;
	glGenTextures(1, &depthTexture_.textureID);
	glBindTexture(GL_TEXTURE_2D, depthTexture_.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, depthTexture_.width, depthTexture_.height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_.textureID, 0);
	// No color output
	glDrawBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error creating framebuffer" << std::endl;
		return false;
	}

    
    // ------------------------------------------------------------------- //
    // ---------------- Voxelization shader initialization ------------------- //
    // ------------------------------------------------------------------- //
    glGenFramebuffers(1, &voxelizationFramebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, voxelizationFramebuffer_);
    
    glGenTextures(1, &voxelizationTexture_);
    glBindTexture(GL_TEXTURE_2D, voxelizationTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, voxelDimensions_, voxelDimensions_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, voxelizationTexture_, 0);
    
    // The depth buffer
    // Use this?
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, voxelDimensions_, voxelDimensions_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating framebuffer" << std::endl;
        return false;
    }
    
    
    // ------------------------------------------------------------------- //
    // --------------------- 3D texture initialization ------------------- //
    // ------------------------------------------------------------------- //
    glEnable(GL_TEXTURE_3D);
    
    glGenTextures(1, &voxelTexture_);
    glBindTexture(GL_TEXTURE_3D, voxelTexture_);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Fill 3D texture with empty values
	int numVoxels = voxelDimensions_ * voxelDimensions_ * voxelDimensions_;
	GLubyte* data = new GLubyte[numVoxels*4];
	for(int i = 0; i < voxelDimensions_ ; i++) {
		for(int j = 0; j < voxelDimensions_ ; j++) {
			for(int k = 0; k < voxelDimensions_ ; k++) {
				data[4*(i + j * voxelDimensions_ + k * voxelDimensions_ * voxelDimensions_)] = 0;
				data[4*(i + j * voxelDimensions_ + k * voxelDimensions_ * voxelDimensions_) + 1] = 0;
				data[4*(i + j * voxelDimensions_ + k * voxelDimensions_ * voxelDimensions_) + 2] = 0;
				data[4*(i + j * voxelDimensions_ + k * voxelDimensions_ * voxelDimensions_) + 3] = 0;
			}
		}
	}

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, texture3DSize_, texture3DSize_, texture3DSize_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete[] data;
    
    // ------------------------------------------------------------------- //
    // -------------------------------- Misc ----------------------------- //
    // ------------------------------------------------------------------- //
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
	glGenVertexArrays(1, &quadVertexArray_);

	return true;
}

void VCTApplication::update(float deltaTime) {
	controls_->updateFromInputs(this, deltaTime);
	camera_->update();
}

void VCTApplication::draw() {
	// ------------------------------------------------------------------- // 
	// --------------------- Draw depth to texture ----------------------- //
	// ------------------------------------------------------------------- // 
	// Draw to depth frame buffer instead of screen
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);
	// Set viewport of framebuffer size
	glViewport(0,0,1024,1024);
    // Set clear color and clear
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewMatrix = glm::lookAt(-glm::vec3(0.3, -0.9, 0.1), glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 projectionMatrix = glm::ortho	<float>(-120, 120, -120, 120, -500, 500);
	glm::mat4 depthViewProjectionMatrix = projectionMatrix * viewMatrix;

	for(std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
		(*obj)->drawSimple(viewMatrix, projectionMatrix, shadowShader_);
	}
    
    // ------------------------------------------------------------------- //
    // --------------------- Use voxelization shader ------------------------- //
    // ------------------------------------------------------------------- //
    glDisable(GL_CULL_FACE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, voxelizationFramebuffer_);
    glViewport(0, 0, voxelDimensions_, voxelDimensions_);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
        (*obj)->drawTo3DTexture(voxelizationShader_, voxelTexture_, depthTexture_, voxelDimensions_, texture3DWorldSize_, depthViewProjectionMatrix);
    }

	// ------------------------------------------------------------------- // 
	// --------------------- Draw the scene normally --------------------- //
	// ------------------------------------------------------------------- // 
    glEnable(GL_CULL_FACE);

	// Draw to the screen  
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width_, height_);
	// Set clear color and clear
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewMatrix = camera_->getViewMatrix();
    projectionMatrix = camera_->getProjectionMatrix();
    
	// for(std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
	// 	//(*obj)->drawSimple(viewMatrix, projectionMatrix, shadowShader_);
	// 	(*obj)->draw(viewMatrix, projectionMatrix, depthViewProjectionMatrix, depthTexture_, standardShader_);
	// }

	// // Voxelize
	// for(std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
	// 	//(*obj)->drawSimple(viewMatrix, projectionMatrix, geometryShader_);
	// 	(*obj)->draw(viewMatrix, projectionMatrix, depthViewProjectionMatrix, depthTexture_, standardShader_);
	// }

	// Draw voxels
	drawVoxels();

	//drawTextureQuad(depthTexture_.textureID);
	drawTextureQuad(voxelizationTexture_);
}

void VCTApplication::drawTextureQuad(GLuint textureID) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0,300,300);
	
	glUseProgram(quadShader_);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(quadShader_, "Texture"), 0);

	glBindVertexArray(quadVertexArray_);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
	glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
}

void VCTApplication::drawVoxels() {
	glUseProgram(renderVoxelsShader_);

	int numVoxels = texture3DSize_ * texture3DSize_ * texture3DSize_;
	float voxelSize = texture3DWorldSize_ / texture3DSize_;
	glUniform1i(glGetUniformLocation(renderVoxelsShader_, "Dimensions"), texture3DSize_);
	glUniform1i(glGetUniformLocation(renderVoxelsShader_, "TotalNumVoxels"), numVoxels);
	glUniform1f(glGetUniformLocation(renderVoxelsShader_, "VoxelSize"), voxelSize);
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(voxelSize * sponzaScale_)), glm::vec3(0, 0, 0));
	//glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(voxelSize)), glm::vec3(0, 0, 0));
	glm::mat4 viewMatrix = camera_->getViewMatrix();
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 projectionMatrix = camera_->getProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(renderVoxelsShader_, "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(renderVoxelsShader_, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D, texture3D_);
	glBindTexture(GL_TEXTURE_3D, voxelTexture_);
	glUniform1i(glGetUniformLocation(renderVoxelsShader_, "VoxelsTexture"), 0);

	glBindVertexArray(texture3DVertexArray_);
	glDrawArrays(GL_POINTS, 0, numVoxels);
	
	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint VCTApplication::create3DTexture() {
	srand (time(NULL));

	int size = texture3DSize_;
	int numVoxels = size * size * size;
	GLubyte* data = new GLubyte[numVoxels*4];
	for(int i = 0; i < size ; i++) {
		for(int j = 0; j < size ; j++) {
			for(int k = 0; k < size ; k++) {
				unsigned char r = rand() % 256;
				data[4*(i + j * size + k * size * size)] = r;
				r = rand() % 256;
				data[4*(i + j * size + k * size * size) + 1] = r;
				r = rand() % 256;
				data[4*(i + j * size + k * size * size) + 2] = r;
				r = rand() % 256;
				data[4*(i + j * size + k * size * size) + 3] = r;
			}
		}
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texture3DSize_, texture3DSize_, texture3DSize_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_3D, 0);

	delete[] data;

	return textureID;
}