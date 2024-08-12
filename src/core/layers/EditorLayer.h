#pragma once

#include <memory>
#include "../..//gui/GuiController.h"
#include "../../core/scene/SceneManager.h"
#include "../../events/EventManager.h"

class EditorLayer
{
private:
	unsigned int VAO, VBO;
	Camera editorCamera;
	SceneManager& sceneManager = SceneManager::getInstance();
	EventManager& eventManager = EventManager::getInstance();
	ImGuiController* guiController;

	void mockThreadTasks();

public:
	EditorLayer();
	~EditorLayer() = default;

	void init(ImGuiController& controller);
	void onAttach();
	void onDetach();
	void onUpdate();
	void onGuiUpdate();
	void onEvent(Event& event);
};

