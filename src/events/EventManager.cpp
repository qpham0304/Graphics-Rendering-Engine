#include "EventManager.h"
#include "../graphics/utils/Utils.h"

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

void EventManager::PublishAsync(EventListener& eventListener)
{
	std::scoped_lock<std::mutex> lock(queueMutex);
	eventListener.onEvent();
}

void EventManager::CleanUpThread()
{
	int counter = 0;

	for (auto& [thread, status] : threads) {
		if (status != nullptr) {
			if (*status) {
				counter++;
				if (thread.joinable()) {
					Console::println("...threads joined...");
					thread.join();
				}
			}
		}
	}
	if (!threads.empty() && counter == threads.size()) {
		threads.clear();
		Console::println("All threads cleaned up");
	}
}

void EventManager::Queue(AsyncEvent event, AsyncCallback callback)
{
	std::scoped_lock<std::mutex> lock(queueMutex);
	eventQueue.push(std::make_pair(std::move(event), std::move(callback)));
	runningTasks++;
}

void EventManager::Subscribe(const std::string& event, EventListener& listener) {
	if (listeners.find(event) != listeners.end()) {
		listeners[event].emplace_back(std::move(listener));
	}
	else {
		listeners[event] = { std::move(listener) };
	}
}

void EventManager::OnUpdate()
{
	Timer timer("thread queue", true);

	while (!eventQueue.empty()) {
		auto& [event, callback] = eventQueue.front();
		
		Console::println("num Tasks: ", runningTasks);
		if (runningTasks <= 5) {
			AsyncEvent mutableEvent(event);
			std::thread thread = std::thread([this, &event, callback, mutableEvent]() mutable {
				//if (runningTasks > 5) {
					//std::scoped_lock<std::mutex> lock(queueMutex);
				//	Console::println("lock queue");
				//}
				callback(mutableEvent);
				//event.isCompleted = true;
				runningTasks--;
			});
			threads.push_back(std::make_pair(std::move(thread), &event.isCompleted));
			eventQueue.pop();
		}
	}

	//if (eventQueue.empty()) {
	//	CleanUpThread();
	//}
}

bool EventManager::canDraw()
{
	return threads.empty();
}
