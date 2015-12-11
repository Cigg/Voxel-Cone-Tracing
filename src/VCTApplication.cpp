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
    
	// Load shaders
	standardShader_ = loadShaders("../shaders/standard.vert", "../shaders/standard.frag");
    voxelizationShader_ = loadShaders("../shaders/voxelization.vert", "../shaders/voxelization.frag", "../shaders/voxelization.geom");
    shadowShader_ = loadShaders("../shaders/shadow.vert", "../shaders/shadow.frag");
    quadShader_ = loadShaders("../shaders/quad.vert", "../shaders/quad.frag");
    renderVoxelsShader_ = loadShaders("../shaders/renderVoxels.vert", "../shaders/renderVoxels.frag", "../shaders/renderVoxels.geom");
    //clearVoxelsShader_ = loadShaders("../shaders/clearVoxels.vert", "../shaders/clearVoxels.frag");

    // Load objects
    std::cout << "Loading objects... " << std::endl;
    loadObject("../data/models/crytek-sponza/", "sponza.obj", glm::vec3(0.0f), sponzaScale_);
	//loadObject("../data/models/", "suzanne.obj");
    std::cout << "Loading done! " << objects_.size() << " objects loaded" << std::endl;

	// Sort object so opaque objects are rendered first
	std::sort(objects_.begin(), objects_.end(), compareObjects);
 
    // Create VAO for 3D texture. Won't really store any information but it's still needed.
	glGenVertexArrays(1, &texture3DVertexArray_);
    
    // ------------------------------------------------------------------- //
    // --------------------- Shadow map initialization ------------------- //
    // ------------------------------------------------------------------- //
	glm::mat4 viewMatrix = glm::lookAt(lightDirection_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projectionMatrix = glm::ortho	<float>(-120, 120, -120, 120, -500, 500);
	depthViewProjectionMatrix_ = projectionMatrix * viewMatrix;

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
    voxelTexture_.size = voxelDimensions_;

    //glGenFramebuffers(1, &voxelizationFramebuffer_);
    //glBindFramebuffer(GL_FRAMEBUFFER, voxelizationFramebuffer_);
    
    //glGenTextures(1, &voxelizationTexture_);
    //glBindTexture(GL_TEXTURE_2D, voxelizationTexture_);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, voxelTexture_.size, voxelTexture_.size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, voxelizationTexture_, 0);
    //
    //// The depth buffer
    //// Use this?
    //GLuint depthrenderbuffer;
    //glGenRenderbuffers(1, &depthrenderbuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, voxelTexture_.size, voxelTexture_.size);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    //if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //    std::cout << "Error creating framebuffer" << std::endl;
    //    return false;
    //}
    
    
    // ------------------------------------------------------------------- //
    // --------------------- 3D texture initialization ------------------- //
    // ------------------------------------------------------------------- //
    glEnable(GL_TEXTURE_3D);
    
    glGenTextures(1, &voxelTexture_.textureID);
    glBindTexture(GL_TEXTURE_3D, voxelTexture_.textureID);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 3); // Mipmap levels. Goes all the way to a 1 x 1 texture if not set

	//Fill 3D texture with empty values (not needed for glTexImage3D)
	int numVoxels = voxelTexture_.size * voxelTexture_.size * voxelTexture_.size;
	GLubyte* data = new GLubyte[numVoxels*4];
	for(int i = 0; i < voxelTexture_.size ; i++) {
		for(int j = 0; j < voxelTexture_.size ; j++) {
			for(int k = 0; k < voxelTexture_.size ; k++) {
				data[4*(i + j * voxelTexture_.size + k * voxelTexture_.size * voxelTexture_.size)] = 0;
				data[4*(i + j * voxelTexture_.size + k * voxelTexture_.size * voxelTexture_.size) + 1] = 0;
				data[4*(i + j * voxelTexture_.size + k * voxelTexture_.size * voxelTexture_.size) + 2] = 0;
				data[4*(i + j * voxelTexture_.size + k * voxelTexture_.size * voxelTexture_.size) + 3] = 0;
			}
		}
	}

	// Option 1: Texture storage
	//glTexStorage3D(GL_TEXTURE_3D, 3, GL_RGBA8, voxelTexture_.size, voxelTexture_.size, voxelTexture_.size);
	//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, voxelTexture_.size, voxelTexture_.size, voxelTexture_.size, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Option 2: Texture image
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, voxelTexture_.size, voxelTexture_.size, voxelTexture_.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	delete[] data;
	
	//glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST); // GL_FASTEST, GL_NICEST, GL_DONT_CARE
	glGenerateMipmap(GL_TEXTURE_3D); // Allocate mipmaps

	// Voxelize once here instead of for every frame for now.
	voxelizeScene();
    
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

	// Draw depth to texture
	drawDepthTexture();
    
	// Voxelize scene
	//voxelizeScene();

	// Draw the scene normally
	drawScene();

	// clearVoxels();

	// Draw voxels for debugging
	//drawVoxels();

	//drawTextureQuad(depthTexture_.textureID);
	//drawTextureQuad(voxelizationTexture_);
}

void VCTApplication::drawDepthTexture() {
	glUseProgram(shadowShader_);
	
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	// Draw to depth frame buffer instead of screen
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);
	// Set viewport of framebuffer size
	glViewport(0, 0, 1024, 1024);
	// Set clear color and clear
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
		(*obj)->drawSimple(depthViewProjectionMatrix_, shadowShader_);
	}

	glUseProgram(0);
}

void VCTApplication::voxelizeScene() {
	glUseProgram(voxelizationShader_);
	
	glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, voxelizationFramebuffer_);
	glViewport(0, 0, voxelTexture_.size, voxelTexture_.size);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
		// Voxelizes scene and injects lighting in the same step. Lighting can be deferred to avoid unnecesarry shading.
		(*obj)->drawTo3DTexture(voxelizationShader_, voxelTexture_, depthTexture_, voxelGridWorldSize_, depthViewProjectionMatrix_);
	}

	// glGenerateMipmap is too slow
	// Improvements:
	//   1: Use mipmap compute shader instead. Can handle directions too.
	//      See anisotropic voxels in the original article.
	//   2: Separate static objects and dynamic objects somehow. Static objects only have to be
	//      voxelized and mipmapped once.
	glBindTexture(GL_TEXTURE_3D, voxelTexture_.textureID);
	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);

	glEnable(GL_CULL_FACE);

	glUseProgram(0);
}

void VCTApplication::drawScene() {
	glUseProgram(standardShader_);

	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	// Draw to the screen  
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width_, height_);

	// Set clear color and clear
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform3f(glGetUniformLocation(standardShader_, "LightDirection"), lightDirection_.x, lightDirection_.y, lightDirection_.z);
	glUniform1f(glGetUniformLocation(standardShader_, "VoxelGridWorldSize"), voxelGridWorldSize_);
	glUniform1i(glGetUniformLocation(standardShader_, "VoxelDimensions"), voxelDimensions_);

	// Bind depth texture
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, depthTexture_.textureID);
	glUniform1i(glGetUniformLocation(standardShader_, "ShadowMap"), 10);

	// Bind scene 3D texture
	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_3D, voxelTexture_.textureID);
	glUniform1i(glGetUniformLocation(standardShader_, "VoxelTexture"), 11);

	glm::mat4 viewMatrix = camera_->getViewMatrix();
	glm::mat4 projectionMatrix = camera_->getProjectionMatrix();

	for (std::vector<Object*>::iterator obj = objects_.begin(); obj != objects_.end(); ++obj) {
		(*obj)->draw(viewMatrix, projectionMatrix, depthViewProjectionMatrix_, depthTexture_, voxelTexture_, standardShader_);
	}
	
	glUseProgram(0);
}

void VCTApplication::clearVoxels() {
	glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, voxelTexture_.size, voxelTexture_.size);
	
	glUseProgram(clearVoxelsShader_);

	glBindTexture(GL_TEXTURE_3D, voxelTexture_.textureID);
    glBindImageTexture(0, voxelTexture_.textureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glUniform1i(glGetUniformLocation(clearVoxelsShader_, "voxelTexture"), 0);
    glUniform1i(glGetUniformLocation(clearVoxelsShader_, "voxelDimensions"), voxelTexture_.size);

	glBindVertexArray(quadVertexArray_);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
	glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
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

	int numVoxels = voxelTexture_.size * voxelTexture_.size * voxelTexture_.size;
	float voxelSize = voxelGridWorldSize_ / voxelTexture_.size;
	glUniform1i(glGetUniformLocation(renderVoxelsShader_, "Dimensions"), voxelTexture_.size);
	glUniform1i(glGetUniformLocation(renderVoxelsShader_, "TotalNumVoxels"), numVoxels);
	glUniform1f(glGetUniformLocation(renderVoxelsShader_, "VoxelSize"), voxelSize);
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(voxelSize)), glm::vec3(0, 0, 0));
	//glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(voxelSize)), glm::vec3(0, 0, 0));
	glm::mat4 viewMatrix = camera_->getViewMatrix();
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 projectionMatrix = camera_->getProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(renderVoxelsShader_, "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(renderVoxelsShader_, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxelTexture_.textureID);
	glUniform1i(glGetUniformLocation(renderVoxelsShader_, "VoxelsTexture"), 0);

	glBindVertexArray(texture3DVertexArray_);
	glDrawArrays(GL_POINTS, 0, numVoxels);
	
	glBindVertexArray(0);
	glUseProgram(0);
}