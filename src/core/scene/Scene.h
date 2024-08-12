#pragma once

#include<glm/glm.hpp>
#include "entt.hpp"
#include "../entities/Entity.h"
#include "../layers/LayerManager.h"

//class Entity;

class Scene
{
private:
	std::string sceneName;
	entt::registry registry;

	friend Entity;

public:
	std::unordered_map<std::string, Entity> entities;
	LayerManager layerManager;
	bool isEnabled;

	Scene(const std::string name);
	~Scene() = default;


	bool addLayer(Layer* layer);
	bool removeLayer(int&& index);


	std::string addEntity(const std::string& name = "Entity");
	bool removeEntity(const std::string& uuid);
	bool hasEntity(const std::string& id);
	Entity getEntity(const std::string& id);

	void onStart();
	void onStop();
	void onUpdate(const float& deltaTime);
	void onGuiUpdate(const float& deltaTime);
	const std::string& getName() const;
};

