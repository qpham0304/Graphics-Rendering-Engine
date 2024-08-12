#pragma once

#include "../../core/layers/AppLayer.h"
#include "ParticleGeometry.h"

class DeferredIBLDemo : public AppLayer
{
private:
	float frameCounter = 0.0f;
	float lastFrame = 0.0f;
	float deltaTime = 0.0f;
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	unsigned int sphereVAO = 0;
	unsigned int indexCount = 0;


	float speed = 0.001f;
	bool pause = true;
	bool reset = false;
	bool isPopulating = false;
	glm::vec3 spawnArea = glm::vec3(100.0, 10.0, 100.0);
	glm::vec3 direction = glm::vec3(0.0, 0.0, 0.0);
	unsigned int numInstances = 100000;
	int numRender = numInstances;
	float heightLimit = 100.0f;
	glm::vec2 randomRange = glm::vec2(-5.0, 5.0);

	glm::vec3 particleSize = glm::vec3(0.1, 0.1, 0.1);
	std::vector<glm::mat4> matrixModels;

	ParticleControl particleControl = ParticleControl(randomRange, spawnArea, heightLimit, -heightLimit, numInstances, particleSize);
	ParticleGeometry particleRenderer;

	std::unique_ptr<Shader> pbrShader;
	std::unique_ptr<Shader> equirectangularToCubemapShader;
	std::unique_ptr<Shader> irradianceShader;
	std::unique_ptr<Shader> backgroundShader;
	std::unique_ptr<Shader> prefilterShader;
	std::unique_ptr<Shader> brdfShader;
	std::unique_ptr<Shader> modelShader;

	std::vector<Light> lights;

	unsigned int captureFBO, captureRBO;
	unsigned int envCubemapTexture;
	unsigned int hdrTexture;
	std::string texRes;

	unsigned int prefilterMap;
	unsigned int irradianceMap;
	unsigned int brdfLUTTexture;

public:
	DeferredIBLDemo(const std::string& name);
	~DeferredIBLDemo() = default;
	
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnGuiUpdate() override;
	void OnEvent(Event& event) override;

	//can run the demo without the editor
	static int show_demo();
	static int run();
};

