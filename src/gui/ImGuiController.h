#pragma once

#include <vector>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include "headers/widget.h"
#include "headers/Themes.h"
#include "headers/MenuWidget.h"

class ImGuiController
{
private:
	std::vector<std::unique_ptr<Widget>> widgets;
	bool darkTheme = false;
	int width;
	int height;
	ImGuiIO* io_ptr;

	bool closeable = true;
	int count = 0;


public:

	ImGuiController();
	ImGuiController(bool darkTheme);

	// TODO: add closable tab, ability to on/off open close tab
	// then reopen them in navigation bar 

	void init(GLFWwindow* window, int width, int height);

	void start();
	void render();
	void end();

	void setTheme(bool darkTheme);
	static void useLightTheme();
	static void useDarkTheme();

};

