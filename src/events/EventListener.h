#pragma once

#include <functional>
#include "../graphics/utils/headers/Utils.h"
#include <iostream>
#include <any>
#include <variant>
#include <stdexcept>
#include "../core/features/Timer.h"
#include "./Event.h"

class EventListener 
{
private:
	std::any callback;
	
	template<typename... Args>
	void setCallback(std::function<void(Args ...)> cb) {
		callback = cb;
	}

	template<typename... Args>
	bool isCallbackValid() const {
		if (callback.type() == typeid(std::function<void(Args...)>)) {
			return true;
		}
		return false;
	}


public:
	template<typename... Args>
	EventListener(std::function<void(Args... args)> cb) {
		setCallback(cb);
	}

	template<typename... Args>
	EventListener(std::function<void(Args... args)>&& cb) {
		setCallback(cb);
	}

	//overhead might be high
	//TODO: if client provides wrong arguments when publishing the event, the app would crash
	//i.e: the function takes two double but pubslish only give one or three => crash
	template<typename... Args>
	void onEvent(Args... args) { 
		if (callback.has_value()) {
			// TLDR: turns C++ into javascript, type safety? what's that
			if (isCallbackValid<Args...>()) {	// this can kinda avoid the crash
				auto& cb = std::any_cast<std::function<void(Args...)>&>(callback);
				Timer time("conversion event");
				cb(std::forward<Args>(args)...);
			}
			else {
				Console::error("Invalid callback provided\n");
			}
		}
		else {
			Console::error("Callback handled or Error retriving erased type\n");
		}
	};
};