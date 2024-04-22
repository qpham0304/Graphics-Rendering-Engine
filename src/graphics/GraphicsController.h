#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> 
#include "components/headers/Component.h"
#include "components/headers/ModelComponent.h"

class OpenGLController {
private:
	OpenGLController() {};

	static std::unordered_map<std::string, std::unique_ptr<Component>> components; //take uuid 
	static std::unordered_map<std::string, Shader> shaders;

public:
	//static Light light;

	static void render(Camera& camera, Light& light);
	static void renderShadow(Shader& shadowMapShader, Camera& camera);
	static std::string addComponent(Component& component);
	static std::string addComponent(const char* path);
	static void removeComponent(std::string id);
	static std::string getActiveComponent();
	static void addShader();
	static void removeShader();
};

