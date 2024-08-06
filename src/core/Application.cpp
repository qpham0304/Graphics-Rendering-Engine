#include "Application.h"

#include "../core/features/AppWindow.h"
#include "../../src/apps/particle-demo/ParticleDemo.h"
#include "../../src/apps/deferred-IBL-demo/deferredIBL_demo.h"
#include "../../src/apps/volumetric-light/VolumetricLightDemo.h"
#include "../../src/events/EventManager.h"
#include "features/Timer.h"
#include "./layers/AppLayer.h"
#include "./layers/EditorLayer.h"
#include "entt.hpp"
#include "FileWatch.hpp"
#include "features/Profiler.h"


void HandleWindowResize() {

}


Application::Application()
{
	isRunning = false;
	AppWindow::init(PLATFORM_OPENGL);
}

Application& Application::getInstance()
{
	static Application application;
	return application;
}

bool Application::addLayer(Layer* layer)
{
	uint32_t a = 0;
	return layerManager.AddLayer(layer);
}

bool Application::removeLayer(int&& index)
{
	return layerManager.RemoveLayer(std::move(index));
}

void Application::run() {
	isRunning = true;
	int width = AppWindow::width;
	int height = AppWindow::height;
	GLFWwindow*& window = AppWindow::window;

	AppWindow::start("Application");
	ImGuiController guiController;
	guiController.init(window, width, height);

	EventManager& eventManager = EventManager::getInstance();

	glfwSetWindowUserPointer(AppWindow::window, this);
	glfwSwapInterval(1);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_DEPTH_TEST);

	//addLayer(new AppLayer("Application"));
	addLayer(new ParticleDemo("Particle Demo"));
	//addLayer(new DeferredIBLDemo("Deferred IBL Demo"));
	addLayer(new EditorLayer("Editor"));
	//layerManager.DisableLayer(1);
	//layerManager.EnableLayer(1);
	//layerManager[1].m_Enabled = false;
	//layerManager[1].m_Enabled = true;

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
	
	AsyncEvent addComponentEvent;
	auto func = [](Event& event) -> void {
		Component reimu("Models/reimu/reimu.obj");
		SceneManager::addComponent(reimu);
	};
	eventManager.Queue(addComponentEvent, func);

	AsyncEvent addComponentEvent1;
	auto func1 = [](Event& event) -> void {
		Component reimu("Models/sponza/sponza.obj");
		SceneManager::addComponent(reimu);
	};
	eventManager.Queue(addComponentEvent1, func1);

	AsyncEvent addComponentEvent2;
	auto func2 = [](Event& event) -> void {
		Component reimu("Models/aru/aru.gltf");
		SceneManager::addComponent(reimu);
	};
	eventManager.Queue(addComponentEvent2, func2);

	Component* component = nullptr;
	bool* ready = nullptr;
	ModelLoadAsyncEvent addComponentEvent3(component);
	auto func3 = [&component, &ready](Event& event) -> void {
		ModelLoadAsyncEvent& e = static_cast<ModelLoadAsyncEvent&>(event);
		component = new Component("Models/aru/aru.gltf");
		ready = &e.isCompleted;
	};
	eventManager.Queue(addComponentEvent3, func3);

	bool joined = false;
	int i = 0;
	std::vector<std::thread> threads;
	std::vector<bool*> statuses;
	while (isRunning) {
		eventManager.OnUpdate();

		if (component && *ready) {
			SceneManager::addComponent(*component);
		}

		for (const auto& layer : layerManager) {
			if (!layer->m_Enabled) {
				continue;
			}
			layer->OnUpdate();
		}

		guiController.start();
		guiController.render();
		for (auto& layer : layerManager) {
			if (!layer->m_Enabled) {
				continue;
			}
			layer->OnGuiUpdate();
		}
		if (ImGui::Begin("right side bar")) {
			if (ImGui::Button("add layer")) {
				addLayer(new AppLayer("Application"));
				//layerManager.AddLayer(new DeferredIBLDemo("Deferred IBL Demo"));
			}
			if (ImGui::Button("remove layer")) {
				removeLayer(1);
				//layerManager.AddLayer(new DeferredIBLDemo("Deferred IBL Demo"));
			}
			ImGui::End();
		}
		for (auto& layer : layerManager) {
			if (ImGui::Begin("right side bar")) {
				ImGui::Checkbox(layer->GetName().c_str(), &layer->m_Enabled);
				ImGui::End();
			}
		}

		guiController.end();

		AppWindow::pollEvents();
		AppWindow::swapBuffer();
	}
	AppWindow::end();
}

bool Application::running()
{
	return isRunning;
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

void MouseMove(GLFWwindow* window, double xpos, double ypos) {
	//application->mouse_callback(window, xpos, ypos);
}

void MouseScroll(GLFWwindow* window, double xpos, double ypos) {
	//application->scroll_callback(window, xpos, ypos);
}

void KeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//application->key_callback(window, key, scancode, action, mods);
}