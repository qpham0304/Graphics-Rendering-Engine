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

void EventManager::Queue(AsyncEvent event, EventCallback callback)
{
	std::scoped_lock<std::mutex> lock(queueMutex);
	std::pair pair = std::make_pair(event, callback);
	eventQueue.push(pair);
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
	int count = 0;
	if (eventQueue.empty()) {
		int counter = 0;

		for (auto& [thread, status] : threads) {
			if (status != nullptr) {
				if (*status == true) {
					counter++;
				}

				if (counter == threads.size()) {
					if (thread.joinable()) {
						Console::println("...threads joined...");
						thread.join();
					}
				}
			}
		}
		//if (!threads.empty()) {
		//	threads.clear();
		//}
	}
		
	else {
		while (!eventQueue.empty()) {
			auto& [event, callback] = eventQueue.front();

			count++;
			std::thread thread = std::thread([callback, &event, this, &count]() {
				//std::scoped_lock<std::mutex> lock(queueMutex);
				//std::this_thread::sleep_for(std::chrono::seconds(3));
				callback(event);
				event.isCompleted = true;
				});
			threads.push_back(std::make_pair(std::move(thread), &event.isCompleted));

			//auto func = [callback, &event, this, count]() {
			//	std::scoped_lock<std::mutex> lock(queueMutex);
			//	std::this_thread::sleep_for(std::chrono::seconds(3));
			//	//callback(*event);
			//	Console::println("hello world" + std::to_string(count));
			//};
			//std::future<void> future = std::async(std::launch::async, func);
			//futures.push_back(std::async(std::launch::async, func));
			eventQueue.pop();

		}
	}
}
