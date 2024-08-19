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
#include "../src/graphics/renderer/SkyboxRenderer.h"
#include "../components/legacy/Component.h"
#include "../components/legacy/LightComponent.h"
#include "Scene.h"

class SceneManager {
private:
	static std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
	static std::string selectedID;
	std::mutex animationsLock;
	std::mutex animatorsLock;
	std::mutex modelsLock;
	std::string activeScene;

	SceneManager();

	std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;


public:
	static std::mutex mtx;
	std::unordered_map<std::string, std::shared_ptr<Model>> models;
	std::unordered_map<std::string, std::shared_ptr<Animation>> animations;

	Texture defaultAlbedo;
	Texture defaultNormal;
	Texture defaultMetallic;
	Texture defaultRoughness;
	Texture defaultAO;

	~SceneManager();

	static SceneManager& getInstance();

	//static std::mutex mutex;
	static std::unordered_map<std::string, std::unique_ptr<Component>> components; //take uuid 
	static std::unordered_map<std::string, std::unique_ptr<LightComponent>> lights;
	static Camera* cameraController;
	static bool gammaCorrection;

	bool addScene(const std::string& name);
	bool addScene(std::unique_ptr<Scene> scene);
	Scene* getScene(const std::string& name);
	Scene* getActiveScene();
	bool removeScene(const std::string& name);
	void onUpdate(const float&& deltaTime);
	void onGuiUpdate(const float&& deltaTime);
	std::string addModel(const std::string& path);
	std::string addModelFromMeshes(std::vector<Mesh>& meshes);
	bool removeModel(const std::string& path);
	std::string addAnimation(const std::string& path, Model* model);
	bool removeAnimation(const std::string& path);

	
	//TODO: refactor these static functions into a new project or remove them completely
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

