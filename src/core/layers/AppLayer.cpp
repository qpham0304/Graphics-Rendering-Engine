#include "AppLayer.h"

unsigned int VAO, VBO;

AppLayer::AppLayer(const std::string& name) : Layer(name)
{
	
}

void AppLayer::OnAttach()
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
	camera.Init(AppWindow::width, AppWindow::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0));
	SceneManager::cameraController = new Camera(
		AppWindow::width, 
		AppWindow::height, 
		glm::vec3(-6.5f, 3.5f, 8.5f), 
		glm::vec3(0.5, -0.2, -1.0f)
	);
	skybox.reset(new SkyboxComponent());
}

void AppLayer::OnDetach()
{

}

void AppLayer::OnUpdate()
{
	camera.onUpdate();
	camera.processInput(AppWindow::window);
	applicationFBO.Bind();
	glViewport(0.0, 0.0, AppWindow::width, AppWindow::height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
	lightShader.Activate();
	lightShader.setMat4("mvp", camera.getMVP());
	lightShader.setMat4("matrix", glm::mat4(1.0));
	lightShader.setVec3("lightColor", glm::vec3(0.7, 0.8, 0.5));
	Utils::OpenGL::Draw::drawCube(VAO, VBO);
	skybox->render(camera);
	applicationFBO.Unbind();
}

void AppLayer::OnGuiUpdate()
{
	if (ImGui::Begin("control")) {

		ImGui::End();
	}

	if (ImGui::Begin("Application window")) {
		ImGui::BeginChild("Child");	// spand fullscreen so it won't scroll on resize
		ImVec2 wsize = ImGui::GetWindowSize();
		int wWidth = static_cast<int>(ImGui::GetWindowWidth());
		int wHeight = static_cast<int>(ImGui::GetWindowHeight());
		ImGui::Image((ImTextureID)applicationFBO.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::IsItemHovered()) {
			//camera.processInput(AppWindow::window);
		}
		ImGui::EndChild();
		ImGui::End();
	}
}

void AppLayer::OnEvent(Event& event)
{

}

const int& AppLayer::GetTextureID()
{
	return applicationFBO.texture;
}
