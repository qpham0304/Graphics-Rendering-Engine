#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> 
#include <memory>
#include "components/headers/Component.h"
#include "components/headers/ModelComponent.h"

namespace OpenGLController {
	static std::unordered_map<std::string, std::unique_ptr<Component>> components; //take uuid 
	static std::unordered_map<std::string, Shader> shaders;
	static Camera* cameraController;
	static std::string selectedID = "";

	void render(Camera& camera, Light& light);
	void renderShadow(Shader& shadowMapShader, Camera& camera);
	Component* getComponent(std::string id);
	Component* getSelectedComponent();
	std::string getSelectedID();
	void setSelectedID(std::string id);
	std::string addComponent(Component& component);
	std::string addComponent(const char* path);
	void updatecomponent(std::string id);
	void removeComponent(std::string id);
	void addShader();
	void removeShader();
};

