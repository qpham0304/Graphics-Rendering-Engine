#include "EventManager.h"
#include "../graphics/utils/headers/Utils.h"

EventManager& EventManager::getInstance()
{
	static EventManager instance;
	return instance;
}

void EventManager::Subscribe(EventType eventType, EventCallback callback)
{
	if (callbacks.find(eventType) != callbacks.end()) {
		callbacks[eventType].emplace_back(std::move(callback));
	}
	else {
		callbacks[eventType] = { std::move(callback) };
	}
}

void EventManager::Publish(Event& event)
{
	if (callbacks.find(event.GetEventType()) != callbacks.end()) {
		for (const auto& callback : callbacks[event.GetEventType()]) {
			callback(event);
			if (event.Handled) {
				break;
			}
		}
	}
}

void EventManager::Queue(EventType eventType, EventCallback callback)
{
//	eventQueue.push(event);
}

void EventManager:: Subscribe(const std::string& event, EventListener& listener) {
	if (listeners.find(event) != listeners.end()) {
		listeners[event].emplace_back(std::move(listener));
	}
	else {
		listeners[event] = { std::move(listener) };
	}
}

void EventManager::OnUpdate()
{
	//while (!eventQueue.empty()) {
		//EventType event = eventQueue.front();
		//PublishAsync(event);
		//Publish(event);
		//eventQueue.pop();
	//}
}
