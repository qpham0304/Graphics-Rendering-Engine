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

bool Scene::addEntity(const std::string& name)
{
	std::string uuid = Utils::uuid::get_uuid();
	//std::string uuid = "123456";
	if (entities.find(uuid) != entities.end()) {
		return false;
	}
	entt::entity e = registry.create();
	entities[uuid] = Entity(e, registry);
	entities[uuid].addComponent<TransformComponent>();
	entities[uuid].addComponent<NameComponent>(name);

	return true;
}

bool Scene::removeEntity(const std::string& name)
{
	if (entities.find(name) != entities.end()) {
		entities.erase(name);
		return true;
	}
	return false;
}

bool Scene::hasEntity(const std::string& uuid)
{
	return (entities.find(uuid) != entities.end());
}

Entity Scene::getEntity(const std::string& uuid)
{
	if (entities.find(uuid) != entities.end()) {
		return entities[uuid];
	}
	throw std::runtime_error("Entity does not exist");
}

void Scene::onUpdate(const float& deltaTime)
{
	entt::basic_view view = registry.view<TransformComponent, ShaderComponent>();

	view.each([&deltaTime](auto& trans, auto& shaders) {

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
