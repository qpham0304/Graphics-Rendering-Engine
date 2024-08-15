#pragma once

#include "widget.h"
#include "EntityControlWidget.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "../../core/entities/Entity.h"

class LeftSidebarWidget : public Widget
{

private:
	std::vector<std::string> nodes;
	size_t selectedIndex;
	Entity* selectedEntity;
	std::string selectedModel;
	bool errorPopupOpen = false;
	const std::string ACTIVE_SCENE = "default";

public:
	LeftSidebarWidget();

	void AddComponentDialog(Entity& entity);
	void ErrorModal(const char* message);
	void AddItemButton(const std::string&& label = "+ Add");
	void LightTab(Entity& entity);
	void EntityTab();
	void ModelsTab();
	void render() override;
};

