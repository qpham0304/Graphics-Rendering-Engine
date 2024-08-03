#pragma once
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include <string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

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
	virtual std::string ToString() const { return GetName(); }
};

class MouseMoveEvent : public Event {
private:
	double m_x = 0; 
	double m_y = 0;

public:
	GLFWwindow* window;

	MouseMoveEvent(GLFWwindow*& window, double x, double y) : m_x(x), m_y(y), window(window) {}

	EventType GetEventType() const override {
		return EventType::MouseMoved;
	}

	const char* GetName() const override {
		return "MouseMoveEvent";
	};

	std::string ToString() const override {
		return "MouseMoveEvent";
	}
};

class MouseScrollEvent : public Event {
public:
	double m_x = 0;
	double m_y = 0;
	GLFWwindow* window;

	MouseScrollEvent(GLFWwindow*& window, double x, double y) : m_x(x), m_y(y), window(window) {}

	EventType GetEventType() const override {
		return EventType::MouseScrolled;
	}

	const char* GetName() const override {
		return "MouseScrollEvent";
	};

	std::string ToString() const override {
		return "MouseScrollEvent";
	}
};