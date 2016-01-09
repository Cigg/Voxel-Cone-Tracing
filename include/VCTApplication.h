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
	void updateInput();
	void draw();

protected:
	bool loadObject(std::string path, std::string name, glm::vec3 pos = glm::vec3(0.0f), float scale = 1.0f);
	void drawTextureQuad(GLuint textureID);
	void drawVoxels();
	void drawDepthTexture();
	void voxelizeScene();
	
	int width_, height_;
	Camera* camera_;
	Controls* controls_;
	GLFWwindow* window_;

	std::vector<Object*> objects_;
	std::map<int, Material*> materials_;

	GLuint standardShader_;

	const float sponzaScale_ = 0.05f;
	glm::vec3 lightDirection_ = glm::vec3(-0.3, 0.9, -0.25);

	// Stuff for shadow mapping
	GLuint depthFramebuffer_;
	Texture2D depthTexture_;
	GLuint shadowShader_;
	glm::mat4 depthViewProjectionMatrix_;

	// Voxelization
    GLuint voxelizationShader_;
    Texture3D voxelTexture_;
    const int voxelDimensions_ = 512;
    const float voxelGridWorldSize_ = 150.0f;
    glm::mat4 projX_, projY_, projZ_;

	// Render voxels
	GLuint renderVoxelsShader_;
	GLuint texture3DVertexArray_;

	// Render texture debug
	GLuint quadShader_;
	GLuint quadVertexArray_;
	GLuint quadVBO_;

	// Inputs
	bool press1_ = false, press2_ = false, press3_ = false, press4_ = false;
	bool showDiffuse_ = true, showIndirectDiffuse_ = true, showIndirectSpecular_ = true, showAmbientOcculision_ = true;
};

#endif // VCTAPPLICATION_H