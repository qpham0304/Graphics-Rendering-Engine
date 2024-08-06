#pragma once

#include "entt.hpp"
#include<glm/glm.hpp>

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

	

public:
	Scene();
	~Scene() = default;

	void onUpdate(const float& deltaTime);
};

