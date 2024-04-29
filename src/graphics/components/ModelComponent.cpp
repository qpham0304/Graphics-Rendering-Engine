#include "headers/ModelComponent.h"

ModelComponent::ModelComponent(const char* path)
{
	model_ptr.reset(new Model(path));
	shaderProgram_ptr.reset(new Shader("Shaders/default.vert", "Shaders/default.frag"));
	const char* fileName = std::strrchr(path, '/');
	name = fileName + 1;
}


void ModelComponent::setUniform()
{

}

void ModelComponent::renderShadow(Shader& shader)
{
	Component::renderShadow(shader);
	if (hasAnimation) {
		auto aru_transforms_shadow = animator_ptr->GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms_shadow.size(); ++i)
			shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms_shadow[i]);
		model_ptr->Draw(shader);
	}
	else
		model_ptr->Draw(shader);
}

void ModelComponent::loadAnimation(const char* path)
{
	Component::loadAnimation(path);
	animation_ptr.reset(new Animation(path, model_ptr.get()));
	animator_ptr.reset(new Animator(animation_ptr.get()));
}

void ModelComponent::updateAnimation(float dt)
{
	Component::updateAnimation(dt);
	animator_ptr->UpdateAnimation(dt);
}

void ModelComponent::render(Camera& camera, const Light& light)
{
	Component::render(camera, light);
	countVertices = model_ptr->getNumVertices();
	if (hasAnimation) {
		auto aru_transforms = animator_ptr->GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms.size(); ++i)
			shaderProgram_ptr->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms[i]);
		model_ptr->Draw(*shaderProgram_ptr);
	}
	else
		model_ptr->Draw(*shaderProgram_ptr);
}