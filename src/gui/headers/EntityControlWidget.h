#pragma once

#include "Widget.h"

class EntityControlWidget : public Widget
{
private: 

	bool isSelected = false;
	int id;

public:
	EntityControlWidget();
	void render() override;

	void scale(glm::mat4& scaleMatrix);
	void translate(glm::mat4& modelMatrix);
	void rotate(glm::mat4& rotate);
};

