#pragma once

#include "Widget.h"

class MenuWidget : public Widget
{
private:

public:
	MenuWidget();
	~MenuWidget();

	void render() override;
	void MainMenuBar();
	void FileMenu();
	void EditMenu();
	void ToolMenu();
	void WindowMenu();
	void HelpMenu();
};

