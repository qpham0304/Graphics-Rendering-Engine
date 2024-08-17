#pragma once

#include <memory>
#include "../..//gui/GuiController.h"
#include "../../core/scene/SceneManager.h"
#include "../../events/EventManager.h"

class EditorLayer
{
private:
	Camera editorCamera;
	SceneManager& sceneManager = SceneManager::getInstance();
	EventManager& eventManager = EventManager::getInstance();
	ImGuiController* guiController;
	bool GuizmoActive = false;
	bool drawGrid = false;
	bool editorActive = true;
	bool flipUV = true;
	bool faceCamera = false;
	ImGuizmo::OPERATION GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
	Shader modelShader;
	uint32_t keyEventID;

	void mockThreadTasks();
	void renderGuizmo();

public:
	EditorLayer();
	~EditorLayer() = default;

	void init(ImGuiController& controller);
	void onAttach();
	void onDetach();
	void onUpdate();
	void onGuiUpdate();
	void onEvent(Event& event);
	void handleKeyPressed(int keycode);
};

