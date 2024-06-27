#include "GraphicsController.h"

std::unordered_map<std::string, std::unique_ptr<Component>> OpenGLController::components = {};
std::unordered_map<std::string, Shader> OpenGLController::shaders = {};
Camera* OpenGLController::cameraController = nullptr;
std::string OpenGLController::selectedID = "";
std::unordered_map<std::string, std::unique_ptr<LightComponent>> OpenGLController::lights = {};


bool OpenGLController::gammaCorrection = true;

void OpenGLController::renderPBR(Light& light, UniformProperties& uniforms)
{
	//TODO: horrendous solution for now, find a better one
	std::vector<Light*> ls;
	for (auto& pair : lights) {
		std::string id = pair.first;
		LightComponent* lightComponent = lights[id].get();
		if (lightComponent != nullptr && lightComponent->light.get() != nullptr) {
			ls.push_back(lightComponent->light.get());
			lightComponent->render(*cameraController);
		}
	}

	uniforms.gammaCorrection = gammaCorrection;
	for (auto& pair : components) {
		pair.second->renderPBR(*cameraController, light, uniforms, ls);
	}
}

void OpenGLController::render(std::vector<Light> lights, UniformProperties& uniforms)
{
	//for (auto& pair : lights) {
	//	std::string id = pair.first;
	//	LightComponent* lightComponent = lights[id].get();
	//	if (lightComponent != nullptr && lightComponent->light.get() != nullptr) {
	//		ls.push_back(lightComponent->light.get());
	//		lightComponent->render(*cameraController);
	//	}
	//}

	auto& light = lights[1];
	for (auto& pair : components) {
		pair.second->render(*cameraController, light, uniforms);
	}
}

void OpenGLController::renderShadow(Shader& shadowMapShader, Light& light)
{
	shadowMapShader.Activate();
	//shadowMapShader.setMat4("mvp", light.mvp);
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
	//components[id].reset(new Component(component));

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
		std::cerr << "error loading model " << e.what();
		return "";
	}
}

std::string OpenGLController::addPointLight(glm::vec3 position, glm::vec4 color)
{
	try {
		std::string id = Utils::uuid::get_uuid();
		OpenGLController::lights[id].reset(new LightComponent(position, color));
		return id;
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
