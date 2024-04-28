#include "GraphicsController.h"

//std::unordered_map<std::string, std::unique_ptr<Component>> OpenGLController::components = {};
//std::unordered_map<std::string, Shader> OpenGLController::shaders = {};
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

Component* OpenGLController::getComponent(std::string id)
{
	if (components.find(id) != components.end()) {
		return components[id].get();
	}
	return nullptr;
}

Component* OpenGLController::getSelectedComponent()
{
	if (selectedID.empty()) {
		return nullptr;
	}
	return getComponent(selectedID);
}

std::string OpenGLController::addComponent(Component& component)
{
	std::string id = component.getID();
	//std::unique_ptr<ModelComponent> c = std::make_unique<ModelComponent>(std::move(component));
	//if (components.find(id) == components.end()) {
	//	components[id] = std::move(c);
	//}
	//std::cout << "component already exist: " << components[id].getID() << "\n";
	return id;
}

std::string OpenGLController::addComponent(const char* path)
{
	std::unique_ptr<ModelComponent> component = std::make_unique<ModelComponent>(path);
	std::string id = component->getID();
	
	// assume all the model loaded model works as expected
	// this would still add a component to the map if a model failed to load
	if (components.find(component->getID()) == components.end()) {
		components[component->getID()] = std::move(component);
		return id;
	}
	return "";
}

void OpenGLController::updatecomponent(std::string id)
{
	components[id]->select();
}


void OpenGLController::removeComponent(std::string id)
{
	if (components.find(id) != components.end()) {
		components[id].reset();
		components.erase(id);
	}
	else {
		std::cerr << "could not find the selected index to delete \n";
	}
}

void OpenGLController::addShader()
{

}

void OpenGLController::removeShader()
{

}

void OpenGLController::setSelectedID(std::string id)
{
	selectedID = id;
}

std::string OpenGLController::getSelectedID() {
	return selectedID;
}
