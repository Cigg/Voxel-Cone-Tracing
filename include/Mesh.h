#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <assimp/scene.h>

class Mesh {
public:
	Mesh(const aiMesh* mesh);
	~Mesh();

	void draw();

protected:
	GLuint vertexArray_;
	GLuint vboIndices_;
	GLuint numIndices_;
	GLuint vboVertices_;
	GLuint vboNormals_;
	GLuint vboTexCoords_;
	bool hasNormals_;
	bool hasTexCoords_;
	unsigned int materialIndex_;
};

#endif // MESH_H