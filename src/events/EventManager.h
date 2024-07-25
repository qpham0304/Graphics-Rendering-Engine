#pragma once

#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "Event.h"
#include "EventListener.h"

class EventManager
{
public:
	using EventCallback = std::function<void()>;
	void Publish();
	void Subscribe();
	void OnUpdate();

private:
	std::unordered_map<std::string, std::vector<EventCallback>> listeners;
	std::queue<std::string> eventQueue;
	std::mutex queueMutex;
};

