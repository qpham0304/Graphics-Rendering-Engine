#pragma once

#include <iostream>
#include "imgui.h"

class Widget {
private:
	bool showWidget = true;

protected:

public:
    // default constructor
	Widget();
    // Copy constructor
    Widget(const Widget& other);
    // Move constructor
    Widget(Widget&& other);
    // Copy assignment operator
    Widget& operator=(const Widget& other);
    // Move assignment operator
    Widget& operator=(Widget&& other) noexcept;
    // destructor
    virtual ~Widget();

	virtual void render();
};