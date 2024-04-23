#pragma once

#include "Component.h"
#include <model.h>
#include "component.h"

class ModelComponent : public Component
{
private:
	std::unique_ptr<Model> model_ptr;

public:
	ModelComponent(const char* path);
	~ModelComponent() = default;


	void setUniform();
	void render(Camera& camera, const Light& light) override;
	void renderShadow(Shader& shader, Camera& camera) override;
};


