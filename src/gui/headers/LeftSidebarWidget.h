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
	std::vector<EntityControlWidget> entities;
	std::vector<std::string> nodes;
	std::unordered_map<std::string, std::string> entities_map;

public:
	LeftSidebarWidget();

	void render() override;
};

