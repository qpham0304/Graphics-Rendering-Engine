#pragma once

#include <string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "../core/components/legacy/component.h"
#include "../core/entities/Entity.h"

enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowUpdate,
	KeyPressed, KeyReleased, KeyTyped,
	MousePressed, MouseReleased, MouseMoved, MouseScrolled,
	AsyncEvent, ModelLoadEvent
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

class KeyPressedEvent : public Event
{
public:
	int keyCode = 0;

	KeyPressedEvent(int keyCode) : keyCode(keyCode) {}

	EventType GetEventType() const override {
		return EventType::KeyPressed;
	}

	const char* GetName() const override {
		return "KeyPressedEvent";
	};

	std::string ToString() const override {
		return "KeyPressedEvent";
	}
};

class WindowCloseEvent : public Event
{
public:

	WindowCloseEvent() {}

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

	AsyncEvent(const std::string id) : isCompleted(false), id(id)
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

class ComponentLoadAsyncEvent : public AsyncEvent
{
public:
	Component* component = nullptr;

	ComponentLoadAsyncEvent(Component* component) : component(component)
	{

	};

	EventType GetEventType() const override {
		return EventType::AsyncEvent;
	}

	const char* GetName() const override {
		return "ComponentLoadAsyncEvent";
	};

	std::string ToString() const override {
		return "ComponentLoadAsyncEvent";
	}
};

class ModelLoadAsyncEvent : public AsyncEvent
{
public:
	std::string path = "None";

	ModelLoadAsyncEvent() = default;
	ModelLoadAsyncEvent(const std::string path) : path(path)
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


class ModelLoadEvent : public Event
{
public:
	std::string path = "None";
	Entity entity;

	ModelLoadEvent() = default;
	ModelLoadEvent(const std::string path, const Entity entity) : path(path), entity(entity)
	{

	};

	EventType GetEventType() const override {
		return EventType::ModelLoadEvent;
	}

	const char* GetName() const override {
		return "ModelLoadEvent";
	};

	std::string ToString() const override {
		return "ModelLoadEvent";
	}
};