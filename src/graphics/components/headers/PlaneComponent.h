#pragma once

#include <mesh.h>
#include <model.h>
#include "component.h"

class PlaneComponent : public Component
{
private:
	std::unique_ptr<Model> plane_ptr;

	std::vector<Texture> planeTextures;

	void setup();

public:
	PlaneComponent();
	PlaneComponent(glm::vec3 scale);

	void setUniform();
	void render(Camera& camera, Light& light);
	void renderShadow(Shader& shader, Camera &camera);
};

