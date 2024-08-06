#pragma once
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include <string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "../core/components/Component.h"

enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowUpdate,
	KeyPressed, KeyReleased, KeyTyped,
	MousePressed, MouseReleased, MouseMoved, MouseScrolled,
	AsyncEvent
};

class Event
{
public:
	bool Handled = false;

	virtual ~Event() = default;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual std::string ToString() const { return GetName(); }
};

class MouseMoveEvent : public Event
{
public:
	double m_x = 0;
	double m_y = 0;
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

class MouseScrollEvent : public Event
{
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

class WindowCloseEvent : public Event
{
public:
	GLFWwindow* window;

	WindowCloseEvent(GLFWwindow*& window) : window(window) {}

	EventType GetEventType() const override {
		return EventType::WindowClose;
	}

	const char* GetName() const override {
		return "WindowCloseEvent";
	};

	std::string ToString() const override {
		return "WindowCloseEvent";
	}
};

class WindowResizeEvent : public Event
{
public:
	int m_width;
	int m_height;
	GLFWwindow* window;

	WindowResizeEvent(GLFWwindow*& window, int width, int height) : m_width(width), m_height(height), window(window) {}

	EventType GetEventType() const override {
		return EventType::WindowResize;
	}

	const char* GetName() const override {
		return "WindowResizeEvent";
	};

	std::string ToString() const override {
		return "WindowResizeEvent";
	}
};

class AsyncEvent : public Event
{
public:
	bool isCompleted;
	std::string id;
	
	AsyncEvent() : isCompleted(false), id("")
	{

	};

	virtual EventType GetEventType() const override {
		return EventType::AsyncEvent;
	}

	virtual const char* GetName() const override {
		return "AsyncEvent";
	};

	virtual std::string ToString() const override {
		return "AsyncEvent";
	}
};

class ModelLoadAsyncEvent : public AsyncEvent
{
public:
	Component* component = nullptr;

	ModelLoadAsyncEvent(Component* component) : component(component)
	{

	};

	EventType GetEventType() const override {
		return EventType::AsyncEvent;
	}

	const char* GetName() const override {
		return "ModelLoadAsyncEvent";
	};

	std::string ToString() const override {
		return "ModelLoadAsyncEvent";
	}
};