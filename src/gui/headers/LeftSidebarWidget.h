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
	const std::string ACTIVE_SCENE = "default";

public:
	LeftSidebarWidget();

	void AddEntityButton();
	void LightTab();
	void EntityTab();
	void render() override;
};

