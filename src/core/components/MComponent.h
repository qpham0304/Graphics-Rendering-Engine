#pragma once

#include<glm/glm.hpp>
#include<string>
#include "../../graphics/utils/Utils.h"
#include "model.h"
#include "Animation.h"
#include "Animator.h"

struct TransformComponent {
public:
	glm::mat4 modelMatrix;
	glm::vec3 translate;
	glm::vec3 rotation;
	glm::vec3 scale;

	TransformComponent() = default;
	TransformComponent(glm::mat4&& modelMatrix) : modelMatrix(modelMatrix) {
		
	};
};

struct ShaderComponent {
	std::string path;

	ShaderComponent(const std::string&& path) : path(path) {

	};
};

struct NameComponent {
public:
	std::string name = "";

	NameComponent() = default;
	NameComponent(const std::string& name) : name(name) {
		
	};
};

struct ModelComponent {
public:
	std::string path = "None";
	std::weak_ptr<Model> model;
	ModelComponent() = default;
	ModelComponent(const std::string&& path, std::shared_ptr<Model> model) : path(path), model(model) {};
	ModelComponent(const std::string&& path) : path(path) {};

	void reset() {
		path = "None";
		model.reset();
	}
};

struct AnimationComponent {
public:
	std::string path;
	std::weak_ptr<Animation> animation;
	AnimationComponent() = default;
	AnimationComponent(const std::string&& path) : path(path) {};
};

struct AnimatorComponent {
public:
	std::string path;
	std::weak_ptr<Animator> animation;
	AnimatorComponent() = default;
	AnimatorComponent(const std::string&& path) : path(path) {};
};

struct MeshComponent {
public:
	MeshComponent() = default;
};

