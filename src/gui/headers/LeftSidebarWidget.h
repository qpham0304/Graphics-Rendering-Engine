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

public:
	LeftSidebarWidget();

	void AddEntityButton();
	void LightTab();
	void EntityTab();
	void render() override;
};

