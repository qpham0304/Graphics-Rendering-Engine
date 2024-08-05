#include "EditorLayer.h"

#include "../../core/features/AppWindow.h"
#include "LayerManager.h"
#include "../features/Timer.h"


EditorLayer::EditorLayer(const std::string& name) : Layer(name)
{
	editorCamera.Init(AppWindow::width, AppWindow::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0));
}

void EditorLayer::OnAttach()
{

}

void EditorLayer::OnDetach()
{
    
}

void EditorLayer::OnUpdate()
{
	editorCamera.onUpdate();

	Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
	Shader bloomShader("Shaders/bloom/bloom.vert", "Shaders/bloom/bloom.frag");

	Shader frameShaderProgram("src/apps/frame-buffer/framebuffer.vert", "src/apps/frame-buffer/framebuffer.frag");
	frameShaderProgram.Activate();
	frameShaderProgram.setFloat("screenTexture", 0);
	auto fbo = LayerManager::getFrameBuffer("ParticleDemo");
	if (fbo) {
		fbo->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo->texture);
		Utils::OpenGL::Draw::drawQuad();

		lightShader.Activate();
		lightShader.setMat4("mvp", SceneManager::cameraController->getMVP());
		lightShader.setMat4("matrix", glm::mat4(1.0));
		lightShader.setVec3("lightColor", glm::vec3(0.7, 0.8, 0.5));
		Utils::OpenGL::Draw::drawCube(VAO, VBO);

		fbo->Unbind();
	}
	else {
		// some place holder scene here
		Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
		lightShader.Activate();
		lightShader.setMat4("mvp", SceneManager::cameraController->getMVP());
		lightShader.setMat4("matrix", glm::mat4(1.0));
		lightShader.setVec3("lightColor", glm::vec3(0.7, 0.8, 0.5));
		Utils::OpenGL::Draw::drawCube(VAO, VBO);
	}
}

void EditorLayer::OnGuiUpdate()
{
	ImGui::Begin("test board");
	ImGui::BeginChild("test child");
	ImVec2 wsize = ImGui::GetWindowSize();
	auto fbo = LayerManager::getFrameBuffer("ParticleDemo");
	if (fbo) {
		ImGui::Image((ImTextureID)fbo->texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::EndChild();
	ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{

}



