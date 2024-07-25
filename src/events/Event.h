#pragma once

#include <string>

enum class EventType {
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowUpdate,
	KeyPressed, KeyReleased, KeyTyped,
	MousePressed, MouseReleased, MouseMoved, MouseScrolled
};

class Event
{
public:
	virtual ~Event() = default;

	bool Handled = false;
	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

};

