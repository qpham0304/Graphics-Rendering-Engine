#include "ImGuiController.h"


ImGuiController::ImGuiController()
{
	IMGUI_CHECKVERSION();
	// Initialize ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io_ptr = &io;
}

ImGuiController::ImGuiController(bool darkTheme)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	this->darkTheme = darkTheme;
	io_ptr = &io;
}

void ImGuiController::init(GLFWwindow* window, int width, int height)
{
	this->width = width;
	this->height = height;

	ImGui::GetStyle().ScaleAllSizes(1.5); // Scale up all sizes
	ImGui::GetIO().FontGlobalScale = 1.5; // Scale up font size

	// Setup ImGui GLFW and OpenGL bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

	io_ptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io_ptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;	// Enable docking

	ImFont* font1 = io_ptr->Fonts->AddFontFromFileTTF("src/gui/fonts/Roboto/Roboto-Regular.ttf", 16.0f);
	//io_ptr->Fonts->AddFontDefault();
	//ImFont* mainFont = io_ptr->Fonts->AddFontFromFileTTF("./src/gui/fonts/Roboto/Roboto-Regular.ttf", 16.0f, nullptr, io_ptr->Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(mainFont != nullptr);

	darkTheme ? useDarkTheme() : useLightTheme();
	std::unique_ptr<MenuWidget> menu = std::make_unique<MenuWidget>();
	widgets.push_back(std::move(menu));
}

void ImGuiController::start()
{
	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create a dock space
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void ImGuiController::render()
{
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io_ptr->Framerate, io_ptr->Framerate);
	for (const auto& widget : widgets) {
		widget->render();
	}

	ImGui::ShowDemoWindow();
	ImGui::Begin("main", &closeable);
	ImGui::Text("This is some example text.");
	ImGui::End();
	ImGui::Begin("left");
	ImGui::Text("This is some example text.");
	ImGui::End();
	ImGui::Begin("right");
	ImGui::Text("This is some example text.");
	ImGui::End();
	ImGui::Begin("console");
	ImGui::Text("This is some example text.");
	ImGui::End();

}

void ImGuiController::end()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



void ImGuiController::setTheme(bool darkTheme)
{
	this->darkTheme = darkTheme;
}

void ImGuiController::useLightTheme()
{
	Themes::purpleTheme();
}

void ImGuiController::useDarkTheme()
{
	Themes::darkTheme();
}
