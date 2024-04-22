#include "../headers/RightSidebarWidget.h"

RightSidebarWidget::RightSidebarWidget()
{

}

void RightSidebarWidget::render()
{
	ImGui::Begin("right side bar");
	ImGui::Text("left window text");
	ImGui::End();
}
