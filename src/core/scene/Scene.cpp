#include "Scene.h"
#include "imgui.h"
#include "../../graphics/utils/Utils.h"
#include "../entities/Entity.h"
#include "../components/MComponent.h"

Scene::Scene(const std::string name) : sceneName(name), isEnabled(true)
{
}

bool Scene::addLayer(Layer* layer)
{
	return layerManager.AddLayer(layer);
}

bool Scene::removeLayer(int&& index)
{
	return layerManager.RemoveLayer(std::move(index));
}

uint32_t Scene::addEntity(const std::string& name)
{
	entt::entity e = registry.create();
	//std::string uuid = Utils::uuid::get_uuid();
	uint32_t uuid = (uint32_t)e;
	if (entities.find(uuid) != entities.end()) {
		return -1;	//MAX_VALUE of uint32_t
	}
	entities[uuid] = Entity(e, registry);
	entities[uuid].addComponent<TransformComponent>();
	entities[uuid].addComponent<NameComponent>(name);

	return uuid;
}

bool Scene::removeEntity(const uint32_t& uuid)
{
	if (entities.find(uuid) != entities.end()) {
		entities.erase(uuid);
		return true;
	}
	return false;
}

bool Scene::hasEntity(const uint32_t& uuid)
{
	return (entities.find(uuid) != entities.end());
}

Entity Scene::getEntity(const uint32_t& uuid)
{
	if (entities.find(uuid) != entities.end()) {
		return entities[uuid];
	}
	throw std::runtime_error("Entity does not exist");
}

void Scene::selectEntities(std::vector<Entity> entities)
{
	selectedEntities = entities;
}

const std::vector<Entity>& Scene::getSelectedEntities()
{
	return selectedEntities;
}

void Scene::onUpdate(const float& deltaTime)
{
	entt::basic_view view = registry.view<TransformComponent>();

	view.each([&deltaTime](auto& trans) {

	});

	for (const auto& layer : layerManager) {
		if (!layer->m_Enabled) {
			continue;
		}
		layer->OnUpdate();
	}
}

void Scene::onGuiUpdate(const float& deltaTime)
{
	for (auto& layer : layerManager) {
		if (!layer->m_Enabled) {
			continue;
		}
		layer->OnGuiUpdate();
	}
	for (auto& layer : layerManager) {
		if (ImGui::Begin("right side bar")) {
			ImGui::Checkbox(layer->GetName().c_str(), &layer->m_Enabled);
			ImGui::End();
		}
	}
}

const std::string& Scene::getName() const
{
	return sceneName;
}
