#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> 
#include <memory>
#include "components/headers/Component.h"
#include "components/headers/ModelComponent.h"
#include "components/headers/PlaneComponent.h"
#include "components/headers/SkyboxComponent.h"

class OpenGLController {
private:
	static std::unordered_map<std::string, Shader> shaders;
	static std::vector<std::string> lightsID;
	static std::string selectedID;
	OpenGLController();

public:
	static std::unordered_map<std::string, std::unique_ptr<Component>> components; //take uuid 
	static Camera* cameraController;

	static void render(Camera& camera, Light& light);
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

