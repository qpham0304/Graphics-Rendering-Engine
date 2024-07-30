#pragma once

#include "Layer.h"
#include <FrameBuffer.h>
#include <memory>
#include "../../graphics/renderer/SceneRenderer.h"
#include <Shader.h>

class AppLayer : public Layer
{
private:

public:
	FrameBuffer applicationFBO;
	//FrameBuffer applicationFBO;
	//Camera camera;
	Camera camera;
	std::unique_ptr<SkyboxComponent> skybox;


	AppLayer(const std::string& name);
	~AppLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(Event& event) override;
	const int GetTextureID();
};

