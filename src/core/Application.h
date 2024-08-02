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
};

