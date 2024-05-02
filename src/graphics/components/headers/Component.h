#pragma once

#include <memory>
#include <Shader.h>
#include <camera.h>
#include <unordered_map>
#include <model.h>
#include <Animation.h>
#include <Animator.h>
#include "../../utils/headers/Utils.h"

struct Light {
	glm::vec3 position;
	glm::vec4 color;
	float ambient;
	//float specular;
	glm::mat4 mvp;
	int sampleRadius;

	Light(const glm::vec3 position, const glm::vec4 color, const float ambient = 0.5f, const glm::mat4& mvp = glm::mat4(0.0f), const int sampleRadius = 2) {
		this->position = position;
		this->color = color;
		this->ambient = ambient;
		this->mvp = mvp;
		this->sampleRadius = sampleRadius;
	};
};

struct Material {

};

class Component
{

private:

protected:

	bool selected;
	bool showAxis;
	bool hasAnimation;
	int countVertices = 0;
	float deltaTime;
	std::string id;			// better use true uuid but whatever
	std::unordered_map<std::string, std::vector<std::string>> attributes;
	std::string name;
	std::unique_ptr<Animation> animation_ptr;
	std::unique_ptr<Animator> animator_ptr;

public:
	Component();
	Component(const char* path);
	~Component() = default;

	std::unique_ptr<Model> model_ptr;
	std::unique_ptr<Shader> shaderProgram_ptr;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::vec3 scaleVector = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 translateVector = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotationVector = glm::vec3(0.0f, 0.0f, 0.0f);

	// renderer
	virtual void setUniform();
	virtual void render(Camera& camera, const Light& light);
	virtual void renderShadow(Shader& shader);
	virtual void loadAnimation(const char* path);
	virtual void updateAnimation(float deltaTime);

	// getter
	int getNumVertices();
	glm::mat4 getModelMatrix();
	bool getShowAxisState();
	std::vector<std::string> uniforms();
	void select();
	void unSelect();
	bool isSelected();
	std::string getID();
	std::string getName();
	bool canAnimate();

	// setter
	void swapShader(Shader& shader);


	//TODO: translate by 1.0f without being affected by scale
	void translate(glm::vec3& translate);
	void rotate(glm::vec3& matrix);
	void scale(glm::vec3& scale);
	glm::mat4 getTransform();
};

