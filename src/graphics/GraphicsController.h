#pragma once

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm> 
#include <memory>
#include <future>
#include <mutex>
#include <thread>
#include "components/headers/Component.h"
#include "components/headers/SkyboxComponent.h"

class OpenGLController {
private:
	static std::unordered_map<std::string, Shader> shaders;
	static std::string selectedID;
	OpenGLController();

public:
	static std::unordered_map<std::string, std::unique_ptr<Component>> components; //take uuid 
	static std::unordered_map<std::string, Light> lights;
	static Camera* cameraController;

	static void renderTest(Light& light, UniformProperties& uniforms, const std::vector<Light> lights);
	static void render(Light& light, UniformProperties& uniforms);
	static void renderShadow(Shader& shadowMapShader, Light& light);
	static Component* getComponent(std::string id);
	static Component* getSelectedComponent();
	static std::string getSelectedID();
	static int getNumVertices();
	static void setSelectedID(std::string id);
	static std::string addComponent(Component& component);
	static std::string addComponent(const char* path);
	static void updatecomponent(std::string id);
	static void removeComponent(std::string id);
	static void addShader();
	static void removeShader();
};

