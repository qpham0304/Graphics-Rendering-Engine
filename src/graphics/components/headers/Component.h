#pragma once

#include <memory>
#include <Shader.h>
#include <camera.h>
#include <unordered_map>
#include "../../utils/headers/Math.h"

struct Light {
	glm::vec3 position;
	glm::vec4 color;
	float ambient;
	//float specular;
	glm::mat4 mvp;

	Light(const glm::vec3 position, const glm::vec4 color, const float ambient = 0.5f, const glm::mat4& mvp = glm::mat4(0.0f)) {
		this->position = position;
		this->color = color;
		this->ambient = ambient;
		this->mvp = mvp;
	};
};

struct Material {

};

class Component
{

private:

protected:
	glm::mat4 modelMatrix;
	glm::vec3 scaleVector = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 translateVector;
	bool selected;
	bool showAxis;
	bool hasAnimation;
	std::string id;			// better use true uuid but whatever
	std::unordered_map<std::string, std::vector<std::string>> attributes;


public:
	Component();
	std::unique_ptr<Shader> shaderProgram_ptr;


	// renderer
	virtual void setUniform();
	virtual void render(Camera& camera, const Light& light);
	virtual void renderShadow(Shader& shader, Camera& camera);

	// getter
	glm::mat4 getModelMatrix();
	bool getSelectedState();
	bool getShowAxisState();
	std::vector<std::string> uniforms();
	void select();
	void unSelect();
	bool isSelected();
	std::string getID();

	// setter
	void swapShader(Shader& shader);


	//TODO: translate by 1.0f without being affected by scale
	void scale(glm::vec3& scale);
	void rotate(glm::vec3& matrix);
	void translate(glm::vec3& translate);
};

