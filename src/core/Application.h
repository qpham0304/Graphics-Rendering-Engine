#pragma once

#include <functional>
#include <camera.h>
#include "layers/LayerManager.h"

class Application
{
private:
	bool isRunning;
	LayerManager layerManager;

public:
	Application();
	~Application() = default;

	void run();
	bool running();
	void close();
	//void AppProcessMouse(GLFWwindow* window, std::function<void(double, double)> cb);
	//void AppProcessKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
};

