#pragma once

#include "Layer.h"
#include <memory>

class EditorLayer : public Layer
{
private:

public:
	EditorLayer(const std::string& name);
	~EditorLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnGuiUpdate() override;
	void OnEvent(Event& event) override;

};

