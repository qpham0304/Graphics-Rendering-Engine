#include "GraphicsController.h"

std::unordered_map<std::string, std::unique_ptr<Component>> OpenGLController::components = {};
std::unordered_map<std::string, Shader> OpenGLController::shaders = {};
Camera* OpenGLController::cameraController = nullptr;
std::string OpenGLController::selectedID = "";
std::unordered_map<std::string, Light> OpenGLController::lights = {};

void OpenGLController::renderTest(Light& light, UniformProperties& uniforms, const std::vector<Light> lights)
{
	for (auto& pair : components) {
		pair.second->renderPBR(*cameraController, light, uniforms, lights);
	}
}

void OpenGLController::render(Light& light, UniformProperties& uniforms)
{
	for (auto& pair : components) {
		pair.second->render(*cameraController, light, uniforms);
	}
}

void OpenGLController::renderShadow(Shader& shadowMapShader, Light& light)
{
	shadowMapShader.Activate();
	shadowMapShader.setMat4("mvp", light.mvp);
	for (auto& pair : components) {
		pair.second->renderShadow(shadowMapShader);
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
	//std::unique_ptr<Component> c = std::make_unique<Component>(std::move(component));
	//if (components.find(id) == components.end()) {
	//	components[id] = std::move(c);
	//}
	//std::cout << "component already exist: " << components[id].getID() << "\n";
	//components[id] = std::move(c);

	return id;
}

std::string OpenGLController::addComponent(const char* path)
{
	try {
		std::unique_ptr<Component> component = std::make_unique<Component>(path);
		std::string id = component->getID();

		if (components.find(component->getID()) == components.end()) {
			components[component->getID()] = std::move(component);
			return id;
		}
	}
	catch (const std::runtime_error& e) {
		return "";
	}
}

void OpenGLController::updatecomponent(std::string id)
{
	components[id]->select();
}

void OpenGLController::removeComponent(std::string id)
{
	if (components.find(id) != components.end() && components[id]->isSelected()) {
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

int OpenGLController::getNumVertices()
{
	int countVertices = 0;
	for (auto& pair : components) {
		countVertices += pair.second->getNumVertices();
	}
	return countVertices;
}

void OpenGLController::setSelectedID(std::string id)
{
	selectedID = id;
}

std::string OpenGLController::getSelectedID() {
	return selectedID;
}
