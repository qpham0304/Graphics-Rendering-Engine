#include "Application.h"

#include "../graphics/renderer/SceneRenderer.h"
#include "../../src/apps/image-based-rendering/pbr_demo.h"
#include "../../src/apps/deferred-rendering/deferred_render_demo.h"
#include "../../src/apps/area-light/area_light_demo.h"
#include "../../src/apps/SSAO-demo/SSAO_demo.h"
#include "../../src/apps/volumetric-light/volumetricLightDemo.h"
#include "../../src/apps/SSR-demo/SSR_demo.h"
#include "../../src/apps/SSR-view-demo/SSR_view_demo.h"
#include "../../src/apps/deferred-IBL-demo/deferredIBL_demo.h"
#include "../../src/apps/ray-tracing/BasicRayTracing.h"
#include "../../src/apps/particle-demo/ParticleDemo.h"
#include "../../src/events/EventManager.h";

Application::Application()
{
	isRunning = false;
	SceneRenderer::init(PLATFORM_OPENGL);
}

void Application::run() {
	isRunning = true;
	int width = SceneRenderer::width;
	int height = SceneRenderer::height;

	SceneRenderer::start("Application");
	ImGuiController guiController;
	guiController.init(SceneRenderer::window, width, height);

	while (isRunning) {
		guiController.start();
		Console::println("hello");
		guiController.render();
		guiController.end();
		//ParticleDemo::run();
		glfwPollEvents();
		glfwSwapBuffers(SceneRenderer::window);
	}
	SceneRenderer::end();
}

bool Application::running()
{
	return isRunning;
}

void Application::close()
{
	isRunning = false;
}
