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

class EventManager
{
private:
	EventManager() = default;
	void PublishAsync(EventListener& eventListener);

public:
	using EventCallback = std::function<void(Event&)>;

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

	void Subscribe(EventType eventType, EventCallback callback);
	void Publish(Event& event);
	void Queue(AsyncEvent event, EventCallback callback);
	void OnUpdate();
	std::vector<std::pair<std::thread, bool*>> threads;

private:
	std::unordered_map<std::string, std::vector<EventListener>> listeners;
	std::unordered_map<EventType, std::vector<EventCallback>> callbacks;

	std::queue<std::pair<AsyncEvent, EventCallback>> eventQueue;
	std::mutex queueMutex;
	std::vector<std::future<void>> futures;
};