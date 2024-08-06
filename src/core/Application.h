#pragma once

#include <functional>
#include <camera.h>
#include "layers/LayerManager.h"
#include "future"
#include <mutex>

class Application
{
private:
	bool isRunning;
	Application();
	std::vector<std::future<void>> futures;

public:
	LayerManager layerManager; // user should be able to manage the layer system

	~Application() = default;

	static Application& getInstance();
	bool addLayer(Layer* layer);
	bool removeLayer(int&& index);
	void run();
	bool running();
	void onClose();
};

