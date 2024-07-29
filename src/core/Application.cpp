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
#include "../../src/events/EventManager.h"
#include "../../src/events/WindowEvent.h"
#include "features/Timer.h"

//Application* application;
bool guiOn = true;

Application::Application()
{
	//application = this;
	isRunning = false;
	SceneRenderer::init(PLATFORM_OPENGL);
}
struct Point {
	int x;
	int y;
	Point(int x, int y) : x(x), y(y) {};
};
void cursorPos() {
	std::cout << "Cursor Event" << std::endl;
}
void HandleMyEvent(Event& event) {
	auto& myEvent = static_cast<MouseMoveEvent&>(event);
	std::cout << "AnotherEvent message: " << myEvent.GetName() << std::endl;
};

void Application::run() {
	isRunning = true;
	int width = SceneRenderer::width;
	int height = SceneRenderer::height;
	GLFWwindow*& window = SceneRenderer::window;

	SceneRenderer::start("Application");
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

	auto f = std::function<void(double, double)>([](double, double) {
		Timer time("f event");
	});
	//auto f1 = std::function<void(Point)>(mycb);
	auto f1 = std::function<void(Point)>([](Point p) -> void {
		Timer timer;
		glm::mat4 model;
		glm::translate(model, glm::vec3(1.0));
		glm::scale(model, glm::vec3(1.0));
	});
	EventListener scrollListener(f);
	EventListener mouseMoveListener(f);
	EventListener xyListener(f1);
	eventManager.Subscribe("mouseScrollEvent", scrollListener);
	eventManager.Subscribe("mouseMoveEvent", mouseMoveListener);
	eventManager.Subscribe("xyListener", xyListener);

	eventManager.Subscribe(EventType::MouseMoved, HandleMyEvent);

	camera.Init(width, height, glm::vec3(-3.5f, 1.5f, 5.5f), glm::vec3(0.5, -0.2, -1.0f));
	SkyboxComponent skybox;
	FrameBuffer applicationFBO(width, height, GL_RGBA16F);

	glfwSetWindowUserPointer(SceneRenderer::window, this);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_DEPTH_TEST);
	while (isRunning) {
		//guiController.render();

		//camera.onUpdate();
		applicationFBO.Bind();
		glViewport(0.0, 0.0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
		lightShader.Activate();
		lightShader.setMat4("mvp", camera.getMVP());
		lightShader.setMat4("matrix", glm::mat4(1.0));
		lightShader.setVec3("lightColor", glm::vec3(0.7, 0.8, 0.5));
		Utils::OpenGL::Draw::drawCube();
		skybox.render(camera);
		applicationFBO.Unbind();

		if (guiOn) {
			guiController.start();
			if (ImGui::Begin("control")) {
				ImGui::BeginChild("gBuffers textures");
				if (ImGui::Button("click")) {
					WindowEvent event;
					eventManager.Queue("buttonClickEvent");
				}
				if (ImGui::Button("release all")) {
					eventManager.OnUpdate();
				}

				if (ImGui::Button("update xy")) {
					p.x++;
					p.y++;
					eventManager.Publish("xyListener", p);
				}
				ImGui::EndChild();
				ImGui::End();
			}

			if (ImGui::Begin("Application window")) {
				ImGui::BeginChild("Child");
				ImVec2 wsize = ImGui::GetWindowSize();
				int wWidth = static_cast<int>(ImGui::GetWindowWidth());
				int wHeight = static_cast<int>(ImGui::GetWindowHeight());
				ImGui::Image((ImTextureID)applicationFBO.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
				if (ImGui::IsItemHovered()) {
					//camera.processInput(SceneRenderer::window);
				}
				ImGui::EndChild();
				ImGui::End();
			}
			guiController.end();
		}

		
		camera.onUpdate();
		camera.processKeyboard(window);

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y)
		{
			Timer time("total event cost");
			MouseMoveEvent myEvent(x, y);
			EventManager::getInstance().Publish(myEvent);
			Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
			application->camera.processMouse(window);
			//EventManager::getInstance().Publish("mouseMoveEvent", x, y);
			ImGui_ImplGlfw_CursorPosCallback(window, x, y);
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y)
		{
			//EventManager::getInstance().Publish("mouseScrollEvent", x, y);
			ImGui_ImplGlfw_ScrollCallback(window, x, y);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
			switch (action) {
				case GLFW_PRESS: {
					Console::println("key");

				}

				case GLFW_RELEASE: {

					Console::println("released");
				}

				case GLFW_REPEAT: {
					Console::println("key repeat");
					application->camera.processKeyboard(window);
				}
			}
		});

		glfwPollEvents();
		glfwSwapBuffers(SceneRenderer::window);
	}
	//ParticleDemo::run();
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

void MouseMove(GLFWwindow* window, double xpos, double ypos) {
	//application->mouse_callback(window, xpos, ypos);
}

void MouseScroll(GLFWwindow* window, double xpos, double ypos) {
	//application->scroll_callback(window, xpos, ypos);
}

void KeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//application->key_callback(window, key, scancode, action, mods);
}