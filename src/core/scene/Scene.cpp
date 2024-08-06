#include "Scene.h"
#include "../../graphics/utils/Utils.h"

Scene::Scene()
{
	TransformComponent transform;
	ShaderComponent shaderPath("Shaders/light");
	entity = registry.create();
	registry.emplace<TransformComponent>(entity, transform);
	registry.emplace<ShaderComponent>(entity, shaderPath);
}

void Scene::onUpdate(const float& deltaTime)
{
	entt::basic_view view = registry.view<TransformComponent, ShaderComponent>();

	view.each([&deltaTime](auto& trans, auto& shaders) {

	});
}
