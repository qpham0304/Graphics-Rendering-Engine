#include "EventManager.h"
#include "../graphics/utils/headers/Utils.h"

EventManager& EventManager::getInstance()
{
	static EventManager instance;
	return instance;
}

void EventManager::Publish(const std::string& event)
{
	std::vector<EventListener>& events = listeners[event];
	for (EventListener& e : events) {
		e.onEvent();
	}
}

void EventManager::Queue(const std::string& event)
{
	eventQueue.push(event);
}

void EventManager:: Subscribe(const std::string& event, EventListener& listener) {
	if (listeners.find(event) != listeners.end()) {
		listeners[event].emplace_back(std::move(listener));
	}
	else {
		listeners[event] = { std::move(listener) };
		Console::println(event);
	}
}

void EventManager::OnUpdate()
{
	while (!eventQueue.empty()) {
		std::string event = eventQueue.front();
		//PublishAsync(event);
		Publish(event);
		eventQueue.pop();
	}
}
