#pragma once
#include "Component.h"
class LightComponent : public Component
{

private:

public:
	std::unique_ptr<Light> light;

	LightComponent(glm::vec3 position, glm::vec4 color);

	void render(Camera& camera);
};

