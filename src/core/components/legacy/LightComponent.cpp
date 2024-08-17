#include "./LightComponent.h"

LightComponent::LightComponent(glm::vec3 position, glm::vec4 color)
{
	light.reset(new Light(position, color));
	shaderProgram_ptr.reset(new Shader("Shaders/light.vert", "Shaders/light.frag"));
	model_ptr.reset(new Model("Models/planet/planet.obj"));
	
	glm::vec3 scaleVector(0.25f);
	translate(position);
	scale(scaleVector);
}

void LightComponent::render(Camera& camera)
{
	shaderProgram_ptr->Activate();
	shaderProgram_ptr->setMat4("matrix", modelMatrix);
	shaderProgram_ptr->setMat4("mvp", camera.getMVP());
	std::cout << light->color.x << ", " << light->color.y << std::endl;
	shaderProgram_ptr->setVec3("lightColor", light->color);
	translate(light->position);
	model_ptr->Draw(*shaderProgram_ptr);
}
