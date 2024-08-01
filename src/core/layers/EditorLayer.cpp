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
	Shader frameShaderProgram("src/apps/frame-buffer/framebuffer.vert", "src/apps/frame-buffer/framebuffer.frag");
	frameShaderProgram.Activate();
	frameShaderProgram.setFloat("screenTexture", 0);

	auto fbo = LayerManager::getFrameBuffer("first pass");
	fbo->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo->texture);
	Utils::OpenGL::Draw::drawQuad();
	fbo->Unbind();
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
	else {
		Console::println("cannot get texture");
	}
	ImGui::EndChild();
	ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{

}



