#pragma once

#include <chrono>
#include <Animation.h>
#include <Animator.h>
#include <FrameBuffer.h>
#include <DepthMap.h>
#include <imgui_internal.h>
#include "./graphics/components/headers/SkyboxComponent.h"
#include "./graphics/components/headers/PlaneComponent.h"
#include "./gui/GuiController.h"
#include "./graphics/GraphicsController.h"
#include "camera.h"


class SceneRenderer
{
private:
	static const unsigned int DEFAULT_WIDTH = 720;
	static const unsigned int DEFAULT_HEIGHT = 1280;
	unsigned int width = DEFAULT_WIDTH;
	unsigned int height = DEFAULT_HEIGHT;

	float lastFrame = 0;
	float lf = 0;
	float lastTime = 0;
	unsigned int frameCounter = 0;
	float rotationAngle = 0;
	float angle = 0.0f;
	float radius = 1.0f;
	float angularSpeed = 0.01f;
	Camera* cameraController = nullptr;

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 4.5f, 5.5f);
	float ambient = 0.5f;
	int sampleRadius = 2.0f;

	std::unique_ptr<GraphicsController> graphicsController;
	std::unique_ptr<GuiController> guiController;


public:
	SceneRenderer();
	SceneRenderer(GuiController& guiController, GraphicsController& graphicsController);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processProgramInput(GLFWwindow* window);
	void renderQuad();
	void setUniform(Shader& shader, glm::mat4& matrix, bool hasAnimation, Light& light, glm::mat4& lightCastMatrix, glm::mat4& modelMatrix, Camera& camera);
	int renderScene();

};

