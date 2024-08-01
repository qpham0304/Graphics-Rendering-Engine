#pragma once

#include <FrameBuffer.h>
#include <memory>
#include <Shader.h>
#include "Layer.h"
#include "../../core/features/AppWindow.h"

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

