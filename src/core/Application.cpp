#include "Application.h"

#include "../core/features/AppWindow.h"
#include "../../src/apps/particle-demo/ParticleDemo.h"
#include "../../src/apps/deferred-IBL-demo/deferredIBL_demo.h"
#include "../../src/events/EventManager.h"
#include "features/Timer.h"
#include "./layers/AppLayer.h"
#include "./layers/EditorLayer.h"
#include "entt.hpp"

//Application* application;
bool guiOn = true;

Application::Application()
{
	//application = this;
	isRunning = false;
	AppWindow::init(PLATFORM_OPENGL);
}
struct Point {
	int x;
	int y;
	Point(int x, int y) : x(x), y(y) {};
};
void cursorPos() {
	std::cout << "Cursor Event" << std::endl;
}
// /components/headers
void HandleMouseMoveEvent(Event& event) {
	MouseMoveEvent& mouseEvent = static_cast<MouseMoveEvent&>(event);
	if (SceneManager::cameraController) {
		SceneManager::cameraController->processMouse(mouseEvent.window);
	}
	event.Handled = true;
};

void HandleMouseScrollEvent(Event& event) {
	MouseScrollEvent& mouseEvent = static_cast<MouseScrollEvent&>(event);
	SceneManager::cameraController->scroll_callback(mouseEvent.window, mouseEvent.m_x, mouseEvent.m_y);
	//ImGui_ImplGlfw_ScrollCallback(mouseEvent.window, mouseEvent.m_x, mouseEvent.m_y);
};

void Application::run() {
	isRunning = true;
	int width = AppWindow::width;
	int height = AppWindow::height;
	GLFWwindow*& window = AppWindow::window;

	AppWindow::start("Application");
	ImGuiController guiController;
	guiController.init(window, width, height);

	EventManager& eventManager = EventManager::getInstance();

	Point p(1, 1);
	auto lambda = []() -> void {
		Console::println("clicked");
	};
	auto clickEvent = std::function<void()>(lambda);
	EventListener buttonClickListener(clickEvent);
	eventManager.Subscribe("buttonClickEvent", buttonClickListener);

	//auto f = std::function<void(double, double)>([](double, double) {
	//	Timer time("f event");
	//});
	//auto f1 = std::function<void(Point)>(mycb);
	//auto f1 = std::function<void(Point)>([](Point p) -> void {
	//	Timer timer;
	//	glm::mat4 model;
	//	glm::translate(model, glm::vec3(1.0));
	//	glm::scale(model, glm::vec3(1.0));
	//});
	//EventListener scrollListener(f);
	//EventListener mouseMoveListener(f);
	//EventListener xyListener(f1);
	//eventManager.Subscribe("mouseScrollEvent", scrollListener);
	//eventManager.Subscribe("mouseMoveEvent", mouseMoveListener);
	//eventManager.Subscribe("xyListener", xyListener);

	eventManager.Subscribe(EventType::MouseMoved, HandleMouseMoveEvent);
	eventManager.Subscribe(EventType::MouseScrolled, HandleMouseScrollEvent);

	SkyboxComponent skybox;
	FrameBuffer applicationFBO(width, height, GL_RGBA16F);

	glfwSetWindowUserPointer(AppWindow::window, this);
	glfwSwapInterval(1);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_DEPTH_TEST);

	//layerManager.AddLayer(new AppLayer("Application"));
	//layerManager.AddLayer(new ParticleDemo("Particle Demo"));
	layerManager.AddLayer(new DeferredIBLDemo("Deferred IBL Demo"));
	layerManager.AddLayer(new EditorLayer("Editor"));
	layerManager.DisableLayer(1);
	//layerManager.EnableLayer(1);
	//layerManager[1].m_Enabled = false;
	//layerManager[1].m_Enabled = true;

	while (isRunning) {
		for (const auto& layer : layerManager) {
			if (!layer->m_Enabled) {
				continue;
			}
			layer->OnUpdate();
		}

		if (SceneManager::cameraController != nullptr) {
			SceneManager::cameraController->onUpdate();
			SceneManager::cameraController->processKeyboard(window);
		}

		if (guiOn) {
			guiController.start();
			guiController.render();
			for (auto& layer : layerManager) {
				layer->OnGuiUpdate();
			}
			guiController.end();
		}

		AppWindow::pollEvents();
		AppWindow::swapBuffer();
	}
	AppWindow::end();
}

bool Application::running()
{
	return isRunning;
}

void Application::close()
{
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