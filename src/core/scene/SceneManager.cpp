#include "SceneManager.h"

std::unordered_map<std::string, std::unique_ptr<Shader>> SceneManager::shaders = {};
std::unordered_map<std::string, std::unique_ptr<Component>> SceneManager::components = {};
Camera* SceneManager::cameraController = nullptr;
std::string SceneManager::selectedID = "";
std::unordered_map<std::string, std::unique_ptr<LightComponent>> SceneManager::lights = {};


bool SceneManager::gammaCorrection = true;

SceneManager::SceneManager() {
	
}

SceneManager::~SceneManager()
{

}

SceneManager& SceneManager::getInstance()
{
	static SceneManager instance;
	return instance;
}

bool SceneManager::addScene(const std::string& name)
{
	if (scenes.find(name) == scenes.end()) {
		scenes[name].reset(new Scene(name));
		return true;
	}
	else {
		return false;
	}
}

bool SceneManager::addScene(std::unique_ptr<Scene> scene)
{
	if (scenes.find(scene->getName()) == scenes.end()) {
		scenes[scene->getName()] = std::move(scene);
		return true;
	}
	else {
		return false;
	}
}

bool SceneManager::removeScene(const std::string& name)
{
	if (scenes.find(name) != scenes.end()) {
		scenes[name].reset();
		scenes.erase(name);
		return true;
	}
	else {
		return false;
	}
}

void SceneManager::onUpdate(const float&& deltaTime)
{
	for (auto& [name, scene] : scenes) {
		if(!scene->isEnabled) {
			continue;
		}
		scene->onUpdate(deltaTime);
	}
}

void SceneManager::onGuiUpdate(const float&& deltaTime)
{
	for (auto& [name, scene] : scenes) {
		scene->onGuiUpdate(deltaTime);
	}
}

void SceneManager::renderPBR(Light& light, UniformProperties& uniforms)
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

void SceneManager::render(std::vector<Light> lights, UniformProperties& uniforms)
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

void SceneManager::renderShadow(Shader& shadowMapShader, Light& light)
{
	shadowMapShader.Activate();
	//shadowMapShader.setMat4("mvp", light.mvp);
	for (auto& pair : components) {
		pair.second->renderShadow(shadowMapShader);
	}
}

Component* SceneManager::getComponent(std::string id)
{
	if (components.find(id) != components.end()) {
		return components[id].get();
	}
	return nullptr;
}

Component* SceneManager::getSelectedComponent()
{
	if (selectedID.empty()) {
		return nullptr;
	}
	return getComponent(selectedID);
}

std::string SceneManager::addComponent(Component& component)
{
	std::string id = component.getID();
	if (components.find(id) == components.end()) {
		components[id] = std::make_unique<Component>(std::move(component));
		return id;
	}
	std::cout << "component already exist: " << components[id]->getID() << "\n";
	return "";
}

std::string SceneManager::addComponent(const char* path)
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

std::string SceneManager::addPointLight(glm::vec3 position, glm::vec4 color)
{
	try {
		std::string id = Utils::uuid::get_uuid();
		SceneManager::lights[id].reset(new LightComponent(position, color));
		return id;
	}
	catch (const std::runtime_error& e) {
		return "";
	}
}

void SceneManager::updatecomponent(std::string id)
{
	components[id]->select();
}

void SceneManager::removeComponent(std::string id)
{
	if (components.find(id) != components.end() && components[id]->isSelected()) {
		components[id].reset();
		components.erase(id);
	}
	else {
		std::cerr << "could not find the selected index to delete \n";
	}
}

void SceneManager::addShader()
{

}

void SceneManager::removeShader()
{

}

int SceneManager::getNumVertices()
{
	int countVertices = 0;
	for (auto& pair : components) {
		countVertices += pair.second->getNumVertices();
	}
	return countVertices;
}

void SceneManager::setSelectedID(std::string id)
{
	selectedID = id;
}

std::string SceneManager::getSelectedID() {
	return selectedID;
}
