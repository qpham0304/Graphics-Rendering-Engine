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

enum Platform
{
	PLATFORM_UNDEFINED, PLATFORM_OPENGL, PLATFORM_VULKAN, PLATFORM_DIRECTX,
};

class SceneRenderer
{
private:
	static const unsigned int DEFAULT_WIDTH = 720;
	static const unsigned int DEFAULT_HEIGHT = 1280;
	SceneRenderer();
public:
	static unsigned int width;
	static unsigned int height;
	static Platform platform;
	static const Platform supportPlatform[];
	
	static float lastFrame;
	static float lf;
	static float lastTime;
	static unsigned int frameCounter;
	static float rotationAngle;
	static float angle;
	static float radius;
	static float angularSpeed;

	static glm::vec4 lightColor;
	static glm::vec3 lightPos;
	static float ambient;
	static int sampleRadius;
	static Camera* cameraController;

	static ImGuiController guiController;
	static GLFWwindow* window;

	static int init(Platform platform);		// set up and init the graphics api depending on the platform
	static int start(const char* title);	// start creating windows and context
	static int end();						// close and terminate the program
	static int renderScene();				// the main rendering part that run the ui and the graphics

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void processProgramInput(GLFWwindow* window);
	static void renderQuad();
	static void setUniform(Shader& shader, glm::mat4& matrix, bool hasAnimation, Light& light, glm::mat4& lightCastMatrix, glm::mat4& modelMatrix, Camera& camera);

};

