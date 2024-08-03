//#include "ApplicationWindow.h"
//
//unsigned int AppWindow::width = DEFAULT_WIDTH;
//unsigned int AppWindow::height = DEFAULT_HEIGHT;
//bool AppWindow::VsyncEnabled = false;
//
//ImGuizmo::OPERATION AppWindow::GuizmoType = ImGuizmo::TRANSLATE;
//Platform AppWindow::platform = PLATFORM_UNDEFINED;
//const std::set<Platform> AppWindow::supportPlatform = { PLATFORM_OPENGL };
//GLFWwindow* AppWindow::window = nullptr;
//
//int Window::init(Platform platform) {
//	Window::platform = platform;
//	if (supportPlatform.find(platform) == supportPlatform.end()) {
//		throw std::runtime_error("Platform unsupported");
//	}
//
//	if (Window::platform == PLATFORM_OPENGL) {
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//		// Initialize GLFW
//		if (!glfwInit())
//			return -1;
//
//		// Get primary monitor
//		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//		if (!monitor) {
//			std::cerr << "Failed to get primary monitor" << std::endl;
//			glfwTerminate();
//			return -1;
//		}
//
//		// Get monitor video mode
//		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//		if (!mode) {
//			std::cerr << "Failed to get video mode" << std::endl;
//			glfwTerminate();
//			return -1;
//		}
//
//		width = mode->width * 2 / 3;
//		height = mode->height * 2 / 3;
//	}
//
//	return 0;
//}
//
//int Window::start(const char* title)
//{
//	if (platform == PLATFORM_UNDEFINED)
//		throw std::runtime_error("Platform not specified have you called init() with supported platform yet?");
//	else if (platform == PLATFORM_OPENGL) {
//		window = glfwCreateWindow(width, height, title, NULL, NULL);
//
//		if (window == NULL)
//		{
//			std::cerr << "Failed to create GLFW window" << std::endl;
//			glfwTerminate();
//			return -1;
//		}
//		glfwMakeContextCurrent(window);
//		gladLoadGL();
//	}
//	return 0;
//}
//
//int Window::close()
//{
//	if (platform == PLATFORM_OPENGL) {
//		ImGui_ImplOpenGL3_Shutdown();
//		ImGui_ImplGlfw_Shutdown();
//		ImGui::DestroyContext();
//
//		glfwDestroyWindow(window);
//		glfwTerminate();
//	}
//	return 0;
//}
