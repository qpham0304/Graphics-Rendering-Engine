#pragma once

#include "../headers/Widget.h"

class ConsoleLogWidget : public Widget
{
private:
	bool scrollToBottom = false;

public:
	ConsoleLogWidget();

	void render() override;
};

