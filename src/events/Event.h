#pragma once
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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
	virtual std::string ToString() const { return GetName(); }
};

class MouseMoveEvent : public Event {
private:
	double m_x = 0; 
	double m_y = 0;

public:
	MouseMoveEvent(double x, double y) : m_x(x), m_y(y) {}

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
private:
	double m_x = 0;
	double m_y = 0;

public:
	MouseScrollEvent(double x, double y) : m_x(x), m_y(y) {}

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

class EventDispatcher
{
private:
	Event& m_Event;

public:
	EventDispatcher(Event& event) : m_Event(event) {}

	template<typename T, typename F>
	bool Dispatch(const F& func) {
		if (m_Event.GetEventType() == T::GetStaticType()) {
			m_Event |= func(static_cast<T&>(m_Event));
			return true;
		}
		return false;
	}
};