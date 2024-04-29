#pragma once

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
	~PlaneComponent() = default;

	void setUniform();
	void render(Camera& camera, const Light& light) override;
	void renderShadow(Shader& shader) override;
};

