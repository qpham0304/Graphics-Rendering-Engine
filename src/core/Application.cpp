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
	
	AsyncEvent addComponentEvent2;
	auto func2 = [](Event& event) -> void {
		Component reimu("Models/reimu/reimu.obj");

		SceneManager::addComponent(reimu);
	};
	for (int i = 0; i < 3; i++) {
		eventManager.Queue(addComponentEvent2, func2);
	}
	bool joined = false;
	int i = 0;
	std::vector<std::pair<std::thread, bool*>> threads;
	while (isRunning) {
		//eventManager.OnUpdate();
		while (i < 10) {
			Timer time("thread time", true);

			i++;
			std::thread thread([func2, &addComponentEvent2]() {
				//Console::println("read gets called");

				func2(addComponentEvent2);
				addComponentEvent2.isCompleted = true;
			});
			threads.push_back(std::make_pair(std::move(thread), &addComponentEvent2.isCompleted));
		}
		Console::println(std::to_string(threads.size()));
		if (!joined) {
			int counter = 0;
			for (auto& [thread, status] : threads) {
				if (status != nullptr) {
					if (*status == true) {
						counter++;
						Console::println("...completed...");
					}

					if (counter == threads.size()) {
						if (thread.joinable()) {
							Console::println("...threads joined...");
							thread.join();
						}
					}
				}
			}
			joined = true;
			//threads.clear();
			//Console::println(std::to_string(threads.size()));
		}


		for (const auto& layer : layerManager) {
			if (!layer->m_Enabled) {
				continue;
			}
			layer->OnUpdate();
		}

		guiController.start();
		guiController.render();
		if (ImGui::Button("clear")) {
			eventManager.threads.clear();
		}
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