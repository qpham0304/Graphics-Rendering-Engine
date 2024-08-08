#include "Scene.h"
#include "imgui.h"

Scene::Scene(const std::string name) : sceneName(name), isEnabled(true)
{
	TransformComponent transform;
	ShaderComponent shaderPath("Shaders/light");
	entity = registry.create();
	registry.emplace<TransformComponent>(entity, transform);
	registry.emplace<ShaderComponent>(entity, shaderPath);
}

bool Scene::addLayer(Layer* layer)
{
	return layerManager.AddLayer(layer);
}

bool Scene::removeLayer(int&& index)
{
	return layerManager.RemoveLayer(std::move(index));
}

void Scene::addEntity()
{
	auto a = registry.create();
	Entity ent();
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
