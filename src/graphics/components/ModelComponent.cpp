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

void ModelComponent::renderShadow(Shader& shader, Camera& camera)
{
	Component::renderShadow(shader, camera);
	model_ptr->Draw(shader, camera);
}

void ModelComponent::loadAnimation()
{

}

void ModelComponent::render(Camera& camera, const Light& light)
{
	Component::render(camera, light);
	model_ptr->Draw(*shaderProgram_ptr, camera);
}