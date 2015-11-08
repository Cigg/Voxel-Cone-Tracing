#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>
#include <iostream>

class VCTApplication; // Forward declaration

#include "Mesh.h"
#include "Material.h"

class Object {
public:
	Object();
	~Object();

	bool loadMeshFromFile(const std::string &path);
	void setPosition(glm::vec3 pos);
	void setScale(float scale);
	void draw(VCTApplication* app);

	Mesh* mesh_;
	Material* material_;

protected:
	glm::vec3 position_;
	float scale_;
};

inline bool compareObjects(Object* obj1, Object* obj2) {
	return (!obj1->material_->hasAlpha_ && obj2->material_->hasAlpha_);
}

#endif // OBJECT_H