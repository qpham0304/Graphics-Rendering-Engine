#pragma once

#include <vector>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <ImGuizmo.h>
#include <GLFW/glfw3.h>
#include "headers/widget.h"
#include "headers/Themes.h"
#include "headers/MenuWidget.h"
#include "headers/LeftSidebarWidget.h"
#include "headers/RightSidebarWidget.h"
#include "headers/ConsoleLogWidget.h"
#include "IconsFontAwesome5.h"

class GuiController {
private:

protected:
	std::vector<std::unique_ptr<Widget>> widgets;
	bool darkTheme = false;
	bool closeable = true;
	int width = 0;
	int height = 0;
	int count = 0;

public:
	virtual void init(GLFWwindow* window, int width, int height) {};
	virtual void start() {}
	virtual void render() {}
	virtual void end() {}

	virtual void setTheme(bool darkTheme) {}
	virtual void useLightTheme() {}
	virtual void useDarkTheme() {}
};

//TODO: Move this class to framework/imgui folder once there are other gui framework
class ImGuiController : public GuiController {
private:
	LeftSidebarWidget leftSidebar;
	RightSidebarWidget rightSidebar;
	ConsoleLogWidget console;
	MenuWidget menu;

public:
	ImGuiController();
	ImGuiController(bool darkTheme);


	// Copy constructor
	ImGuiController(const ImGuiController& other);

	// Move constructor
	ImGuiController(ImGuiController&& other) noexcept;

	// Copy assignment operator
	ImGuiController& operator=(const ImGuiController& other) {}

	// Move assignment operator
	ImGuiController& operator=(ImGuiController&& other) noexcept {}

	~ImGuiController();

	void init(GLFWwindow* window, int width, int height) override;
	void start() override;
	void render() override;
	void end() override;

	void setTheme(bool darkTheme) override;
	void useLightTheme() override;
	void useDarkTheme() override;

	// TODO: add closable tab, ability to on/off open close tab
	// then reopen them in navigation bar;
};

