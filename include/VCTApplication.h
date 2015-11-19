#ifndef VCTAPPLICATION_H
#define VCTAPPLICATION_H

#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include "Object.h"
#include "Material.h"
#include "Camera.h"
#include "Controls.h"
#include "Texture.h"
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
	GLuint create3DTexture();
	bool loadObjects();
	void drawTextureQuad(GLuint textureID);
	void drawVoxels();
	
	int width_, height_;
	Camera* camera_;
	Controls* controls_;
	GLFWwindow* window_;

	std::vector<Object*> objects_;
	std::map<int, Material*> materials_;

	GLuint standardShader_;

	// Stuff for shadow mapping
	GLuint depthFramebuffer_;
	Texture depthTexture_;
	GLuint shadowShader_;
	GLuint quadShader_;
	GLuint quadVertexArray_;
	GLuint quadVBO_;

	// Voxelization
	GLuint voxelizationFramebuffer_;
    GLuint voxelizationTexture_; // Used to render voxelization step
    GLuint voxelizationShader_;
    GLuint voxelTexture_;
    const int voxelDimensions_ = 64;

	// Render voxels
	GLuint texture3D_;
	GLuint renderVoxelsShader_;
	GLuint texture3DVertexArray_;
	unsigned int texture3DSize_;
	float texture3DWorldSize_;
};

#endif // VCTAPPLICATION_H