#include "GuiController.h"
#include "../graphics/GraphicsController.h"

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
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	float fontSize = 16.0f;

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = fontSize;
	icons_config.GlyphOffset.y = -0.5f;
	icons_config.GlyphOffset.x = -1.75f;

	// Setup ImGui GLFW and OpenGL bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	io.Fonts->AddFontFromFileTTF("src/gui/fonts/Roboto/Roboto-Regular.ttf", fontSize);
	io.Fonts->AddFontFromFileTTF("src/gui/fonts/fa/" FONT_ICON_FILE_NAME_FAS, fontSize*0.75, &icons_config, icons_ranges);

	darkTheme ? useDarkTheme() : useLightTheme(); // preset style from others
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.FramePadding = ImVec2(10, 4);
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(1, 6);
	style.FrameBorderSize = 0.0f;
	style.WindowBorderSize = 0.0f;
	style.TabRounding = 8;
	style.GrabMinSize = 15;

	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Tab] = ImVec4(1.000f, 0.684f, 0.000f, 0.208f);
	colors[ImGuiCol_TabHovered] = ImVec4(1.000f, 0.682f, 0.000f, 0.549f);
	colors[ImGuiCol_TabActive] = ImVec4(1.000f, 0.682f, 0.000f, 0.549f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.853f, 0.567f, 0.000f, 0.668f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.506f, 0.337f, 0.000f, 0.522f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 0.782f, 0.000f, 1.000f);
	colors[ImGuiCol_Header] = ImVec4(0.267f, 0.267f, 0.267f, 0.681f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.316f, 0.316f, 0.316f, 0.360f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.184f, 0.184f, 0.184f, 1.000f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.163f, 0.163f, 0.163f, 0.000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.338f, 0.338f, 0.338f, 0.540f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.281f, 0.281f, 0.281f, 0.540f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.078f, 0.078f, 0.078f, 0.540f);
	colors[ImGuiCol_Border] = ImVec4(0.212f, 0.212f, 0.212f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.137f, 0.137f, 0.137f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.137f, 0.137f, 0.137f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.137f, 0.137f, 0.137f, 1.000f);
	colors[ImGuiCol_Button] = ImVec4(0.303f, 0.303f, 0.303f, 0.540f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.379f, 0.379f, 0.379f, 0.540f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);

	std::unique_ptr<MenuWidget> menu = std::make_unique<MenuWidget>();
	widgets.push_back(std::move(menu));
}

ImGuiController::~ImGuiController() = default;

void ImGuiController::start()
{
	if (width == -1 && height == -1) {
		throw std::logic_error("Object not fully initialized. Did you call init() function before start?");
	}
	
	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create a dock space
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void ImGuiController::debugWindow(ImTextureID texture)
{
	glm::vec3 camPos = OpenGLController::cameraController->getPosition();
	std::string x = "x: " + std::to_string(camPos.x).substr(0, 4);
	std::string y = "y: " + std::to_string(camPos.y).substr(0, 4);
	std::string z = "z: " + std::to_string(camPos.z).substr(0, 4);

	if (ImGui::Begin("Debug Window"))
	{
		std::string countVertices = "Vertices: " + std::to_string(OpenGLController::getNumVertices() * 3);
		ImGui::Text(countVertices.c_str());
		countVertices = "Triangles: " + std::to_string(OpenGLController::getNumVertices());
		ImGui::SameLine();
		ImGui::Text(countVertices.c_str());
		ImGui::Text("Camera positon");
		ImGui::SameLine();
		ImGui::Text(x.c_str());
		ImGui::SameLine();
		ImGui::Text(y.c_str());
		ImGui::SameLine();
		ImGui::Text(z.c_str());
		// Using a Child allow to fill all the space of the window.
		// It also alows customization
		ImGui::BeginChild("Debug shadow window");
		// Get the size of the child (i.e. the whole draw size of the windows).
		ImVec2 wsize = ImGui::GetWindowSize();
		ImGui::Image(texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();
	}
	ImGui::End();
}

void ImGuiController::applicationWindow()
{
	//start group
	ImGui::SetCursorPos(ImVec2(10.0f, 10.0f));
	ImGui::BeginGroup();
	ImGui::Button("A");
	ImGui::SameLine();
	ImGui::Button("B");
	ImGui::SameLine();
	ImGui::Button("C");
	ImGui::EndGroup();

	ImGuiIO& io = ImGui::GetIO();

	//center group
	ImVec4 buttonActiveColor = ImVec4{ 0.0f, (float)140 / 255, (float)184 / 255, 0.8 };
	ImVec2 buttonSize = ImVec2(36.0f, 36.0f);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x / 2, 10.0f));
	ImGui::BeginGroup();
	ImGui::Button(ICON_FA_PAUSE, buttonSize);
	ImGui::SameLine();
	ImGui::Button(ICON_FA_PLAY, buttonSize);
	ImGui::SameLine();
	ImGui::Button(ICON_FA_STOP, buttonSize);
	ImGui::EndGroup();
	ImGui::PopStyleColor();
}

void ImGuiController::render()
{
	for (const auto& widget : widgets) {
		widget->render();
	}

	menu.render();
	leftSidebar.render();
	rightSidebar.render();
	console.render();
}

//ImGui
void ImGuiController::end()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
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
