#pragma once

#include "widget.h"
#include "EntityControlWidget.h"
//#include "../../graphics/GraphicsController.h"
#include <vector>
#include <string>
#include <unordered_map>>

class LeftSidebarWidget : public Widget
{
private:
	std::vector<std::string> nodes;
	unsigned int selectedIndex;

public:
	LeftSidebarWidget();

	void render() override;
};

