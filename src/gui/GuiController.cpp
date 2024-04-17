#include "GuiController.h"

ImGuiController::ImGuiController()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
}

ImGuiController::ImGuiController(bool darkTheme)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	this->darkTheme = darkTheme;
}

void ImGuiController::init(GLFWwindow* window, int width, int height)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::GetStyle().ScaleAllSizes(1.5); // Scale up all sizes
	ImGui::GetIO().FontGlobalScale = 1.5; // Scale up font size

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Multi-Viewport / Platform Windows

	// Setup ImGui GLFW and OpenGL bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImFont* font1 = io.Fonts->AddFontFromFileTTF("src/gui/fonts/Roboto/Roboto-Regular.ttf", 16.0f);

	darkTheme ? useDarkTheme() : useLightTheme();


	std::unique_ptr<MenuWidget> menu = std::make_unique<MenuWidget>();
	widgets.push_back(std::move(menu));
	//widgets.push_back(std::move(leftPanel));
	//widgets.push_back(std::move(rightPanel));
	//widgets.push_back(std::move(consolePanel));
	//widgets.push_back(std::move(debugPanel));
	//widgets.push_back(std::move(applicationPanel));
}

ImGuiController::~ImGuiController()
{
	
}

void ImGuiController::start()
{
	if(width == -1 && height == -1)
		throw std::logic_error("Object not fully initialized. Did you call init() function before start?");
	
	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create a dock space
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void ImGuiController::render()
{
	for (const auto& widget : widgets) {
		widget->render();
	}

	ImGui::Begin("left");
	ImGui::Text("left window text");
	ImGui::End();
	ImGui::Begin("right");
	ImGui::Text("This is some example text.");
	ImGui::End();
	ImGui::Begin("console");
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();

}

//ImGui
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
