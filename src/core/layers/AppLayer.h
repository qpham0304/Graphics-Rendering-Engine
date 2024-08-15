#pragma once

#include <FrameBuffer.h>
#include <memory>
#include <Shader.h>
#include "Layer.h"
#include "../../core/features/AppWindow.h"
#include "camera.h"
#include "../../src/events/EventManager.h"
#include "LayerManager.h"

class AppLayer : public Layer
{
protected:
	FrameBuffer applicationFBO;
	Camera camera;
	std::unique_ptr<Camera> layerCamera;
	std::unique_ptr<SkyboxComponent> skybox;
	unsigned int VAO, VBO;
	bool isActive;

public:
	AppLayer(const std::string& name);
	~AppLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnGuiUpdate() override;
	void OnEvent(Event& event) override;
	const int& GetTextureID();
};

