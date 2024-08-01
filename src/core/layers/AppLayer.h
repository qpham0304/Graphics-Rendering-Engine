#pragma once

#include "Layer.h"
#include <FrameBuffer.h>
#include <memory>
#include "../../core/features/AppWindow.h"
#include <Shader.h>

class AppLayer : public Layer
{
protected:
	FrameBuffer applicationFBO;
	Camera camera;
	std::unique_ptr<SkyboxComponent> skybox;

public:
	AppLayer(const std::string& name);
	~AppLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnGuiUpdate() override;
	void OnEvent(Event& event) override;
	const int& GetTextureID();
};

