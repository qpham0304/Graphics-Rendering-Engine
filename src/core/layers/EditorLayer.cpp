#include "EditorLayer.h"

#include "../../core/features/AppWindow.h"
#include "LayerManager.h"

EditorLayer::EditorLayer(const std::string& name) : Layer(name)
{

}

void EditorLayer::OnAttach()
{

}

void EditorLayer::OnDetach()
{
    
}

void EditorLayer::OnUpdate()
{
	Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
	Shader modelShader("Shaders/models.vert", "Shaders/model.frag");
	Shader bloomShader("Shaders/bloom/bloom.vert", "Shaders/bloom/bloom.frag");

	Shader frameShaderProgram("src/apps/frame-buffer/framebuffer.vert", "src/apps/frame-buffer/framebuffer.frag");
	frameShaderProgram.Activate();
	frameShaderProgram.setFloat("screenTexture", 0);
	auto fbo = LayerManager::getFrameBuffer("first pass");
	if (fbo) {
		fbo->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo->texture);
		Utils::OpenGL::Draw::drawQuad();
		fbo->Unbind();
	}
}

void EditorLayer::OnGuiUpdate()
{
	ImGui::Begin("test board");
	ImGui::BeginChild("test child");
	ImVec2 wsize = ImGui::GetWindowSize();
	auto fbo = LayerManager::getFrameBuffer("first pass");
	if (fbo) {
		ImGui::Image((ImTextureID)fbo->texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::EndChild();
	ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{

}



