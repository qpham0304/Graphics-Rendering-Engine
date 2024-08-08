#pragma once
#include "entt.hpp"

class Scene;

class Entity
{
private:
	uint32_t id;
	Scene* scene;

public:
	Entity(const uint32_t& id) : id(id) 
	{
	}

	template<typename T, typename ...Args>
	bool addComponent(T& component) {
	
	}

	template<typename T, typename ...Args>
	bool removeComponent(T& component) {
	
	}

	template<typename T, typename ...Args>
	bool hasComponent(T& component) {
	
	}

	template<typename T, typename ...Args>
	std::vector<T&> getComponents() {
	
	}

	const uint32_t& getID() const;
};

