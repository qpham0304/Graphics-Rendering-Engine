#pragma once

#include<glm/glm.hpp>
#include<string>
#include "../../graphics/utils/Utils.h"
#include "model.h"
#include "Animation.h"
#include "Animator.h"

class TransformComponent {
private:
	glm::mat4 modelMatrix = glm::mat4(1.0f);

public:
	glm::vec3 translateVec = glm::vec3(0.0f);
	glm::vec3 rotateVec = glm::vec3(0.0f);
	glm::vec3 scaleVec = glm::vec3(1.0f);

	TransformComponent() = default;
	TransformComponent(glm::mat4&& modelMatrix) : modelMatrix(modelMatrix) {
		
	};

	void updateTransform() {
		//glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), rotateVec.x, glm::vec3(1.0, 0.0, 0.0))
		//						* glm::rotate(glm::mat4(1.0f), rotateVec.y, glm::vec3(0.0, 1.0, 0.0))
		//						* glm::rotate(glm::mat4(1.0f), rotateVec.z, glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rotationMat = glm::toMat4(glm::quat(rotateVec));
		glm::mat4 translateMat = glm::translate(glm::mat4(1.0), translateVec);
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), scaleVec);
		modelMatrix = translateMat * rotationMat * scaleMat;
	}

	void translate(const glm::vec3& translate) {
		translateVec = translate;
		updateTransform();
	}

	void rotate(const glm::vec3& rotate) {
		rotateVec = rotate;
		updateTransform();
	}

	void scale(const glm::vec3& scale) {
		scaleVec = scale;
		updateTransform();
	}

	void translate(glm::vec3&& translate) {
		translateVec = translate;
		updateTransform();
	}

	void rotate(glm::vec3&& rotate) {
		rotateVec = rotate;
		updateTransform();
	}

	void scale(glm::vec3&& scale) {
		scaleVec = scale;
		updateTransform();
	}

	glm::mat4& getModelMatrix() {
		return modelMatrix;
	}
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
	ModelComponent(std::string&& path, std::shared_ptr<Model> model) : path(path), model(model) {};
	ModelComponent(std::string&& path) : path(path) {};

	void reset() {
		path = "None";
		model.reset();
	}
};

struct AnimationComponent {
public:
	std::string path = "None";
	std::weak_ptr<Animation> animation;
	Animator animator;

	AnimationComponent() = default;
	AnimationComponent(std::string&& path, std::shared_ptr<Animation> animation) : path(path), animation(animation) {
		if (animation) {
			animator.Init(animation.get());
		}
	};
	AnimationComponent(std::string&& path) : path(path) {};

	void reset() {
		path = "None";
		animation.reset();
	}
};

//struct AnimatorComponent {
//public:
//	std::string path;
//	std::weak_ptr<Animator> animation;
//	AnimatorComponent() = default;
//	AnimatorComponent(std::string&& path) : path(path) {};
//};

struct MLightComponent {
public:
	glm::vec3 color = glm::vec3(1.0);
	glm::vec3 position = glm::vec3(0.0);
	float radius;

	MLightComponent() = default;
	MLightComponent(glm::vec3&& color, glm::vec3&& position) : color(color), position(position) {};
	MLightComponent(const glm::vec3& color, const glm::vec3& position) : color(color), position(position) {};
};

struct LayerTagComponent {
public:
	std::string layerName = "";

	LayerTagComponent () = default;
	LayerTagComponent (const std::string& layerName) : layerName(layerName) {

	};
};