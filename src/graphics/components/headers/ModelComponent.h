#pragma once

#include <Model.h>
#include <Animation.h>
#include <Animator.h>
#include "Component.h"

class ModelComponent : public Component
{
private:
	std::unique_ptr<Model> model_ptr;
	std::unique_ptr<Animation> animation;
	std::unique_ptr<Animator> animator;
	bool hasAnimation = false;

public:
	ModelComponent(const char* path);
	~ModelComponent() = default;


	void setUniform();
	void render(Camera& camera, const Light& light) override;
	void renderShadow(Shader& shader, Camera& camera) override;
	void loadAnimation();

};


