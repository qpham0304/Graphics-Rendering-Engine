#include "AppLayer.h"

AppLayer::AppLayer(const std::string& name) : Layer(name), isActive(false), VAO(0), VBO(0)
{
	//applicationFBO.reset(new FrameBuffer(AppWindow::width, AppWindow::height));
	//camera.reset(new Camera(AppWindow::width, AppWindow::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0)));
	applicationFBO.Init(
		AppWindow::width,
		AppWindow::height,
		GL_RGBA32F,
		GL_RGBA,
		GL_FLOAT,
		nullptr
	);
	layerCamera.reset(new Camera(AppWindow::width, AppWindow::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0)));
	camera.Init(
		AppWindow::width, 
		AppWindow::height, 
		glm::vec3(-6.5f, 3.5f, 8.5f), 
		glm::vec3(0.5, -0.2, -1.0f)
	);
	SceneManager::cameraController = &camera;
	skybox.reset(new SkyboxComponent());
}

void AppLayer::OnAttach()
{
	EventManager& eventManager = EventManager::getInstance();
	eventManager.Subscribe(EventType::MouseScrolled, [this](Event& event) {
		MouseScrollEvent& mouseEvent = static_cast<MouseScrollEvent&>(event);
		if (isActive) {
			SceneManager::cameraController->scroll_callback(mouseEvent.m_x, mouseEvent.m_y);
			std::cout << "scrolling..." << mouseEvent.GetName() << std::endl;
		}
	});

	eventManager.Subscribe(EventType::MouseMoved, [this](Event& event) {
		MouseMoveEvent& mouseEvent = static_cast<MouseMoveEvent&>(event);
		if (isActive) {
			SceneManager::cameraController->processMouse(mouseEvent.window);
			std::cout << "moving..." << mouseEvent.GetName() << std::endl;
		}
	});

	eventManager.Subscribe(EventType::WindowResize, [this](Event& event) {
		WindowResizeEvent& windowResizeEvent = static_cast<WindowResizeEvent&>(event);
		camera.updateViewResize(windowResizeEvent.m_width, windowResizeEvent.m_height);
	});
}

void AppLayer::OnDetach()
{
	// detach the event
}

void AppLayer::OnUpdate()
{
	camera.onUpdate();
}

void AppLayer::OnGuiUpdate()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("control")) {

		ImGui::End();
	}

	if (ImGui::Begin("Application window")) {
		ImGui::BeginChild("Child");	// spand fullscreen so it won't scroll on resize
		ImVec2 wsize = ImGui::GetWindowSize();
		int wWidth = static_cast<int>(ImGui::GetWindowWidth());
		int wHeight = static_cast<int>(ImGui::GetWindowHeight());
		ImGui::Image((ImTextureID)applicationFBO.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
		//(ImGui::IsItemHovered() && ImGui::IsWindowFocused()) ? isActive = true : false;
		if (ImGui::IsItemHovered() && ImGui::IsWindowFocused()) {
			camera.processKeyboard(AppWindow::window);
			isActive = true;
		}
		else {
			isActive = false;
		}
		ImGui::EndChild();
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void AppLayer::OnEvent(Event& event)
{

}

const int& AppLayer::GetTextureID()
{
	return applicationFBO.texture;
}
