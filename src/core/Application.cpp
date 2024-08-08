#include "Application.h"

#include "../core/features/AppWindow.h"
#include "../../src/apps/particle-demo/ParticleDemo.h"
#include "../../src/apps/deferred-IBL-demo/deferredIBL_demo.h"
#include "../../src/apps/volumetric-light/VolumetricLightDemo.h"
#include "features/Timer.h"
#include "layers/AppLayer.h"
#include "FileWatch.hpp"
#include "features/Profiler.h"

Application::Application() : isRunning(false)
{
	AppWindow::init(PLATFORM_OPENGL);
}

Application& Application::getInstance()
{
	static Application application;
	return application;
}

void Application::run() {
	isRunning = true;
	AppWindow::start("Application");

	int width = AppWindow::width;
	int height = AppWindow::height;
	GLFWwindow*& window = AppWindow::window;
	guiController.init(window, width, height);
	editorLayer.init(guiController);

	//std::bind(&Application::onClose, this, std::placeholders::_1);
	eventManager.Subscribe(EventType::WindowClose, [this](Event& event) {
		onClose();
	});

	//TODO: experimentation file watcher for now
	std::unique_ptr<filewatch::FileWatch<std::string>> fileWatcher;
	fileWatcher.reset(new filewatch::FileWatch<std::string>(
		"./Shaders",
		[](const std::string& path, const filewatch::Event change_type) {
			std::cout << path << "-" << (int)change_type << "\n";
		}
	));


	glfwSetWindowUserPointer(AppWindow::window, this);
	//glfwSwapInterval(1);
	while (isRunning) {
		eventManager.OnUpdate();
		editorLayer.onUpdate();
		sceneManager.onUpdate(glfwGetTime());

		guiController.start();
		editorLayer.onGuiUpdate();
		sceneManager.onGuiUpdate(glfwGetTime());
		guiController.end();

		AppWindow::pollEvents();
		AppWindow::swapBuffer();
	}
	AppWindow::end();
}

void Application::onClose()
{
	for (auto& [thread, status] : EventManager::getInstance().threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	isRunning = false;
}