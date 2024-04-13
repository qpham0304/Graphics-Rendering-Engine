#pragma once

#include <memory>
#include <Shader.h>
#include <camera.h>

struct Light {
	glm::vec3 position;
	glm::vec4 color;
	float ambient;
	//float specular;
	glm::mat4 lightProjection;

	Light(const glm::vec3 position, const glm::vec4 color, const float ambient = 0.5f, const glm::mat4& lightProjection = glm::mat4(0.0f)) {
		this->position = position;
		this->color = color;
		this->ambient = ambient;
		this->lightProjection = lightProjection;
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
	bool isSelected;
	bool showAxis;

public:
	std::unique_ptr<Shader> shaderProgram_ptr;

	Component();
	~Component();

	// renderer
	virtual void setUniform();
	virtual void render(Camera& camera, const Light& light);

	// getter
	glm::mat4 getModelMatrix();
	bool getSelectedState();
	bool getShowAxisState();

	// setter

	//TODO: translate by 1.0f without being affected by scale
	void scale(glm::vec3& scale);
	void rotate(glm::vec3& matrix);
	void translate(glm::vec3& translate);
};

