#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>

class VCTApplication; // Forward declaration

#include "Mesh.h"
#include "Material.h"

class Object {
public:
	Object();
	~Object();

	bool loadMeshFromFile(const std::string &path);
	void setMaterial(Material* mat);
	void setMesh(Mesh* mesh);
	void setPosition(glm::vec3 pos);
	void setScale(float scale);

	void draw(VCTApplication* app);

protected:
	Mesh* mesh_;
	Material* material_;

	glm::vec3 position_;
	float scale_;

};

#endif // OBJECT_H