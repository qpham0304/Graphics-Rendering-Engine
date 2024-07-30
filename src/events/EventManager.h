#pragma once

#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <future>
#include <thread>
#include "Event.h"
#include "EventListener.h"
#include "../core/features/Timer.h"

template<typename... Args>
using EventCallback = std::function<void(Args...)>;

//using EventCallback = std::function<void()>;
//typedef std::function<void()> EventCallback;
class EventManager
{
private:
	EventManager() = default;

public:
	~EventManager() = default;
	
	static EventManager& getInstance();
	
	void Subscribe(const std::string& event, EventListener& listener);

	template<typename... Args>
	void Publish(const std::string& event, Args... args) {
		if (listeners.find(event) != listeners.end()) {
			std::vector<EventListener>& events = listeners[event];
			for (EventListener& e : events) {
				e.onEvent(args...);
			}
		}
		else {
			Console::error("Event not found\n");
		}
	}

	using EventCallback = std::function<void(Event&)>;
	void Subscribe(EventType eventType, EventCallback callback) {
		if (callbacks.find(eventType) != callbacks.end()) {
			callbacks[eventType].emplace_back(std::move(callback));
		}
		else {
			callbacks[eventType] = { std::move(callback) };
		}
	}
	void Publish(Event& event) {
		if (!event.Handled) {
			if (callbacks.find(event.GetEventType()) != callbacks.end()) {
				for (const auto& callback : callbacks[event.GetEventType()]) {
					callback(event);
				}
			}
		}
		//auto it = callbacks.find(event.GetEventType());
		//if (it != callbacks.end()) {
		//	for (const auto& callback : it->second) {
		//		callback(event);
		//	}
		//}
	}

	void Publish(const std::string& event);
	void Queue(const std::string& event);
	void OnUpdate();

private:
	std::unordered_map<std::string, std::vector<EventListener>> listeners;
	std::unordered_map<EventType, std::vector<EventCallback>> callbacks;

	std::queue<std::string> eventQueue;
	std::mutex queueMutex;
};