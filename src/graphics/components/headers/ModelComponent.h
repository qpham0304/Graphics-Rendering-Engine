#pragma once

#include <Model.h>
#include <Animation.h>
#include <Animator.h>
#include "Component.h"

class ModelComponent : public Component
{
private:
	std::unique_ptr<Model> model_ptr;
	std::unique_ptr<Animation> animation_ptr;
	std::unique_ptr<Animator> animator_ptr;

public:
	ModelComponent(const char* path);
	~ModelComponent() = default;


	void setUniform();
	void render(Camera& camera, const Light& light) override;
	void renderShadow(Shader& shader) override;
	void loadAnimation(const char* path) override;
	void updateAnimation(float currentFrame) override;
};


