#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

class Mesh {
public:
	Mesh();
	~Mesh();

	void draw();
	void loadAssimpMesh(const aiMesh* mesh);

protected:
	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec2> uvs_;
	std::vector<glm::vec3> normals_;
	std::vector<glm::vec3> tangents_;
	std::vector<glm::vec3> bitangents_;
	std::vector<unsigned int> indices_;

	GLuint vertexArray_;
	GLuint vboIndices_;
	GLuint numIndices_;
	GLuint vboVertices_;
	GLuint vboNormals_;
	GLuint vboTangents_;
	GLuint vboBitangents_;
	GLuint vboTexCoords_;
	bool hasNormals_;
	bool hasTangentsAndBitangents_;
	bool hasTexCoords_;
	unsigned int materialIndex_;
};

#endif // MESH_H