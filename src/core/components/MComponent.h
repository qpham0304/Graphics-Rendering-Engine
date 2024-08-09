#pragma once

#include<glm/glm.hpp>
#include<string>
#include "../../graphics/utils/Utils.h"
//#include "model.h"

struct TransformComponent {
public:
	glm::mat4 model;

	TransformComponent() = default;
	TransformComponent(glm::mat4&& model) : model(model) {
		
	};
};

struct ShaderComponent {
	std::string path;

	ShaderComponent(const std::string&& path) : path(path) {

	};
};

struct NameComponent {
public:
	std::string name = "Entity";

	NameComponent() = default;
	NameComponent(const std::string& name) : name(name) {
		
	};
};

struct ModelComponent {
public:
	std::string path;
	ModelComponent() = default;
	ModelComponent(const std::string&& path) : path(path) {};
};

struct AnimationComponent {
public:
	std::string path;
	//std::weak_ptr<Model> model;
	AnimationComponent() = default;
	AnimationComponent(const std::string&& path) : path(path) {};
};

struct MeshComponent {
public:
	MeshComponent() = default;

};

