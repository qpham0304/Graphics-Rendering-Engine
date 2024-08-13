#pragma once

#include<glm/glm.hpp>
#include "entt.hpp"
#include "../entities/Entity.h"
#include "../layers/LayerManager.h"

class Scene
{
private:
	std::string sceneName;
	entt::registry registry;
	std::vector<Entity> selectedEntities;


public:
	std::unordered_map<uint32_t, Entity> entities;
	LayerManager layerManager;
	bool isEnabled;

	Scene(const std::string name);
	~Scene() = default;


	bool addLayer(Layer* layer);
	bool removeLayer(int&& index);


	uint32_t addEntity(const std::string& name = "Entity");
	bool removeEntity(const uint32_t& uuid);
	bool hasEntity(const uint32_t& id);
	Entity getEntity(const uint32_t& id);
	void selectEntities(std::vector<Entity> entities);
	const std::vector<Entity>& getSelectedEntities();

	void onStart();
	void onStop();
	void onUpdate(const float& deltaTime);
	void onGuiUpdate(const float& deltaTime);
	const std::string& getName() const;
};

