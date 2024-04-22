#include "GraphicsController.h"

std::unordered_map<std::string, std::unique_ptr<Component>> OpenGLController::components = {};
std::unordered_map<std::string, Shader> OpenGLController::shaders = {};
//Light OpenGLController::light;


void OpenGLController::render(Camera& camera, Light& light)
{
	for (auto& pair: components) {
		pair.second->render(camera, light);
	}
}

void OpenGLController::renderShadow(Shader& shadowMapShader, Camera& camera)
{
	for (auto& pair : components) {
		pair.second->renderShadow(shadowMapShader, camera);
	}
}

std::string OpenGLController::addComponent(Component& component)
{
	std::string id = component.getID();
	if (components.find(id) == components.end()) {
		//components[component.getID()] = std::move(component);
	}
	//std::cout << "component already exist: " << components[id].getID() << "\n";
	return id;

}

std::string OpenGLController::addComponent(const char* path)
{
	std::unique_ptr<ModelComponent> component = std::make_unique<ModelComponent>(path);
	std::string id = component->getID();
	glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f);
	component->scale(scale);
	
	if (components.find(component->getID()) == components.end()) {
		components[component->getID()] = std::move(component);
	}
	return id;
}


void OpenGLController::removeComponent(std::string id)
{

}

std::string OpenGLController::getActiveComponent()
{

	return std::string();
}

void OpenGLController::addShader()
{

}

void OpenGLController::removeShader()
{

}
