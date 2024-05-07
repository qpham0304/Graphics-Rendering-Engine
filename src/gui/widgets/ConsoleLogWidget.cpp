#include "../headers/ConsoleLogWidget.h"

ConsoleLogWidget::ConsoleLogWidget()
{

}

bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
{
	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	return ImGui::Button(label);
}

void ConsoleLogWidget::render()
{
	//ImGui::SetNextItemAllowOverlap();
	//ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());

	ImGui::Begin("Assets");
	ImGui::Text("assets placeholder");
	ImGui::End();

	ImGui::Begin("console");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::ShowDebugLogWindow();
	ImGui::End();

	ImGui::ShowDemoWindow();
}