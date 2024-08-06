#pragma once

#include "widget.h"
#include "EntityControlWidget.h"
#include <vector>
#include <string>
#include <unordered_map>

class LeftSidebarWidget : public Widget
{
private:
	std::vector<std::string> nodes;
	size_t selectedIndex;
	std::string loadedID = "None";

public:
	LeftSidebarWidget();

	void AddEntityButton();
	void LightTab();
	void EntityTab();
	void render() override;
};

