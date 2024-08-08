#pragma once

#include<glm/glm.hpp>
#include "entt.hpp"
#include "../entities/Entity.h"
#include "../layers/LayerManager.h"

struct TransformComponent {
public:
	glm::mat4 model;

	//TransformComponent(glm::mat4&& model) : model(model) {
	//
	//};
};

struct ShaderComponent {
	std::string path;

	ShaderComponent(const std::string&& path) : path(path) {
	
	};
};

class Scene
{
private:
	entt::registry registry;
	entt::entity entity;
	std::string sceneName;

public:
	LayerManager layerManager;
	bool isEnabled;

	Scene(const std::string name);
	~Scene() = default;


	bool addLayer(Layer* layer);
	bool removeLayer(int&& index);


	void addEntity(Entity entity);

	template<typename T>
	void addComponent(Entity& entity, T& component);
	void onStart();
	void onStop();
	void onUpdate(const float& deltaTime);
	void onGuiUpdate(const float& deltaTime);
	const std::string& getName() const;
};

