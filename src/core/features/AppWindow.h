#pragma once

#include <Animation.h>
#include <Animator.h>
#include <FrameBuffer.h>
#include <DepthMap.h>
#include <DepthCubeMap.h>
#include <imgui_internal.h>
#include <set>
#include <future>
#include <mutex>
#include <thread>
#include "../../gui/GuiController.h"
#include "../../core/scene/SceneManager.h"
#include "../../graphics/utils/Utils.h"
#include "../../core/components/SkyboxComponent.h"
#include "../../core/components/GridComponent.h"
#include "../../core/components/LightComponent.h"

enum Platform {
	PLATFORM_UNDEFINED, PLATFORM_OPENGL, PLATFORM_VULKAN, PLATFORM_DIRECTX,
};

class AppWindow
{
private:
	static const unsigned int DEFAULT_WIDTH = 720;
	static const unsigned int DEFAULT_HEIGHT = 1280;
	AppWindow();
	static void setEventCallback();

public:
	static bool VsyncEnabled;
	static unsigned int width;
	static unsigned int height;
	static Platform platform;
	static const std::set<Platform> supportPlatform;
	static ImGuizmo::OPERATION GuizmoType;
	static GLFWwindow* window;

	// currently bind with an app inside window, remove after done refactoring
	static ImGuiController guiController;

	static int init(Platform platform);		// set up and init the graphics api depending on the platform
	static int start(const char* title);	// start creating windows and context
	static int end();						// close and terminate the program
	static void pollEvents();
	static void swapBuffer();
	
	static void renderShadowScene(DepthMap& shadowMap, Shader& shadowMapShader, Light& light);
	static void renderObjectsScene(FrameBuffer& framebuffer, DepthMap& depthMap, std::vector<Light> lights, unsigned int depthMapPoint);
	static int renderScene();
	static int renderScene(std::function<int()> runFunction);
	static void renderGuizmo(Component& component, const bool drawCube, const bool drawGrid);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void processProgramInput(GLFWwindow* window);
};

