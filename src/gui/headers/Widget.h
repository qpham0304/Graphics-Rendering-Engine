#pragma once

#include <iostream>
#include <imgui.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "../../src/gui/headers/IconsFontAwesome5.h"

class Widget {
private:
	bool showWidget = true;

protected:
	Widget();

public:
    // default constructor
    // Copy constructor
    Widget(const Widget& other);
    // Move constructor
    Widget(Widget&& other) noexcept;
    // Copy assignment operator
    Widget& operator=(const Widget& other);
    // Move assignment operator
    Widget& operator=(Widget&& other) noexcept;
    // destructor
    virtual ~Widget();

	virtual void render();
};