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
#include "components/headers/LightComponent.h"

class OpenGLController {
private:
	static std::unordered_map<std::string, Shader> shaders;
	static std::string selectedID;
	OpenGLController();

public:
	static Texture defaultAlbedo;
	static Texture defaultNormal;
	static Texture defaultMetallic;
	static Texture defaultRoughness;
	static Texture defaultAO;

	static std::unordered_map<std::string, std::unique_ptr<Component>> components; //take uuid 
	static std::unordered_map<std::string, std::unique_ptr<LightComponent>> lights;
	static Camera* cameraController;
	static bool gammaCorrection;

	static void renderPBR(Light& light, UniformProperties& uniforms);
	static void render(std::vector<Light> light, UniformProperties& uniforms);
	static void renderShadow(Shader& shadowMapShader, Light& light);
	static Component* getComponent(std::string id);
	static Component* getSelectedComponent();
	static std::string getSelectedID();
	static int getNumVertices();
	static void setSelectedID(std::string id);
	static std::string addComponent(Component& component);
	static std::string addComponent(const char* path);
	static std::string addPointLight(glm::vec3 position, glm::vec4 color);
	static void updatecomponent(std::string id);
	static void removeComponent(std::string id);
	static void addShader();
	static void removeShader();
};

