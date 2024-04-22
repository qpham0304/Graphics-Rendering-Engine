#include "../headers/MenuWidget.h"

MenuWidget::MenuWidget() 
{

}

MenuWidget::~MenuWidget()
{

}

void MenuWidget::render()
{
	MainMenuBar();
}

void MenuWidget::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::SetNextWindowSize({ 250.0f, 0.0f });
		FileMenu();
		ImGui::SetNextWindowSize({ 250.0f, 0.0f });
		EditMenu();
		ImGui::SetNextWindowSize({ 250.0f, 0.0f });
		ToolMenu();
		ImGui::SetNextWindowSize({ 250.0f, 0.0f });
		WindowMenu();
		ImGui::SetNextWindowSize({ 250.0f, 0.0f });
		HelpMenu();


		ImGui::EndMainMenuBar();
	}
}

void MenuWidget::FileMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New"))
		{

		}
		
		if (ImGui::MenuItem("Open"))
		{

		}
		
		if (ImGui::MenuItem("Save"))
		{

		}
		
		if (ImGui::MenuItem("Exit"))
		{

		}
		ImGui::EndMenu();
	}
}

void MenuWidget::EditMenu()
{
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Undo"))
		{

		}

		if (ImGui::MenuItem("Redo"))
		{

		}

		if (ImGui::MenuItem("Cut"))
		{

		}

		if (ImGui::MenuItem("Select"))
		{

		}
		ImGui::EndMenu();
	}
}

void MenuWidget::ToolMenu()
{
	if (ImGui::BeginMenu("Tool"))
	{
		if (ImGui::MenuItem("Effects"))
		{

		}

		if (ImGui::MenuItem("Post processing"))
		{

		}

		if (ImGui::MenuItem("Color adjust"))
		{

		}

		if (ImGui::MenuItem("Blur"))
		{

		}
		ImGui::EndMenu();
	}
}

void MenuWidget::WindowMenu()
{
	if (ImGui::BeginMenu("Window"))
	{
		if (ImGui::MenuItem("Show Left Side Bar"))
		{

		}

		if (ImGui::MenuItem("Show Right Side Bar"))
		{

		}

		if (ImGui::MenuItem("Show Console Log"))
		{

		}

		if (ImGui::MenuItem("Show Sub View"))
		{

		}
		ImGui::EndMenu();
	}
}

void MenuWidget::HelpMenu()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Tutorial"))
		{

		}

		if (ImGui::MenuItem("Support"))
		{

		}

		if (ImGui::MenuItem("Contact"))
		{

		}

		if (ImGui::MenuItem("Send Feedback"))
		{

		}

		if (ImGui::MenuItem("Privacy"))
		{

		}

		if (ImGui::MenuItem("Version"))
		{

		}
		ImGui::EndMenu();
	}
}
