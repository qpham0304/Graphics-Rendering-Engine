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
	void CleanUpThread();
	int runningTasks = 0;

public:
	using EventCallback = std::function<void(Event&)>;
	using AsyncCallback = std::function<void(AsyncEvent&)>;

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
	void Queue(AsyncEvent event, AsyncCallback callback);
	void OnUpdate();
	bool canDraw();
	std::vector<std::pair<std::thread, bool*>> threads;

private:
	std::unordered_map<std::string, std::vector<EventListener>> listeners;
	std::unordered_map<EventType, std::vector<EventCallback>> callbacks;

	std::queue<std::pair<AsyncEvent, AsyncCallback>> eventQueue;
	std::mutex queueMutex;
	std::vector<std::future<void>> futures;
};