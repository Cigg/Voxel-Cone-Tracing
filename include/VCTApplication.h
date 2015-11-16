#ifndef VCTAPPLICATION_H
#define VCTAPPLICATION_H

#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include "Object.h"
#include "Material.h"
#include "Camera.h"
#include "Controls.h"
#include "VCTApplication.h"

class VCTApplication {
public:
	VCTApplication(const int width, const int height, GLFWwindow* window);
	~VCTApplication();

	int getWindowWidth();
	int getWindowHeight();
	GLFWwindow* getWindow();
	Camera* getCamera();

	bool initialize();
	void update(float deltaTime);
	void draw();

protected:
	int width_, height_;
	Camera* camera_;
	Controls* controls_;
	GLFWwindow* window_;

	std::vector<Object*> objects_;
	std::map<int, Material*> materials_;

	// Stuff for shadow mapping and voxelization
	GLuint depthFramebuffer_;
	GLuint depthTexture_;
	GLuint shadowShader_;
	GLuint quadShader_;
    GLuint geometryFramebuffer_;
    GLuint geometryTexture_; // Used to render voxelization step
    GLuint geometryShader_;
	GLuint quadVertexArray_;
	GLuint quadVBO_;
    GLuint voxelTexture_;
    
    const int voxelSize = 256;
};

#endif // VCTAPPLICATION_H