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
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::mat4 mvp;
	int sampleRadius;

	Light(
		const glm::vec3& position, 
		const glm::vec4& color, 
		const glm::vec3& ambient = glm::vec3(0.2f, 0.2f, 0.2f),
		const glm::vec3& diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
		const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f),
		const glm::mat4& mvp = glm::mat4(0.0f), 
		const int sampleRadius = 2) 
	{
		this->position = position;
		this->color = color;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->mvp = mvp;
		this->sampleRadius = sampleRadius;
	};
};

struct PointLight {

};

struct SpotLight {

};

struct DirectionLight {

};

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess) {
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
	}
};

struct MaterialPBR {
	glm::vec3 albedo;
	float metalic;
	float roughness;
	float ao;

	MaterialPBR(const glm::vec3 albedo, const float metalic, const float roughness, const float ao) {
		this->albedo = albedo;
		this->metalic = metalic;
		this->roughness = roughness;
		this->ao = ao;
	}
};

struct UniformProperties {
	bool enableFog;
	float explodeRadius;
	bool gammaCorrection;

	UniformProperties(const bool enableFog = false, const float explodeRadius = 0.0f, const bool gammaCorrection = true) {
		this->enableFog = enableFog;
		this->explodeRadius = explodeRadius;
		this->gammaCorrection = gammaCorrection;
	}
};

class Component
{

private:

protected:
	bool selected;
	bool showAxis;
	bool hasAnimation;
	int countVertices = 0;
	float deltaTime = 0;
	std::string id;			// better use true uuid but good enough for now
	std::unordered_map<std::string, std::vector<std::string>> attributes;
	std::string name;
	std::unique_ptr<Animation> animation_ptr;
	std::unique_ptr<Animator> animator_ptr;

	std::unique_ptr<Shader> shaderProgram_ptr;
	glm::mat4 modelMatrix;
	glm::mat3 normalMatrix;

public:
	Component();
	Component(const char* path);
	Component(Component&& other) noexcept;
	Component& operator=(Component&& other) noexcept;
	~Component() = default;

	std::unique_ptr<Model> model_ptr;

	glm::vec3 scaleVector = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 translateVector = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotationVector = glm::vec3(0.0f, 0.0f, 0.0f);
	
	Material material = Material(
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		32.0f
	);

	MaterialPBR materialPBR = MaterialPBR(glm::vec3(0.5, 0.5, 0.5), 1.0, 1.0, 1.0);


	// renderer
	virtual void setUniform();
	virtual void renderPBR(Camera& camera, const Light& light, const UniformProperties& uniforms, const std::vector<Light*> lights);
	virtual void render(Camera& camera, const Light& light, const UniformProperties& uniforms);
	virtual void renderShadow(Shader& shader);
	virtual void loadAnimation(const char* path);
	virtual void updateAnimation(float deltaTime);

	// getter
	int getNumVertices();
	glm::mat4 getModelMatrix();
	glm::mat4 getNormalMatrix();
	bool getShowAxisState();
	bool isSelected();
	std::string getID();
	std::string getName();
	bool canAnimate();

	// setter
	void select();
	void unSelect();
	void swapShader(Shader& shader);		// Ideally swap to a different shader
	void translate(glm::vec3 translate);
	void rotate(glm::vec3 matrix);
	void scale(glm::vec3 scale);
	glm::mat4 getTransform();
};

