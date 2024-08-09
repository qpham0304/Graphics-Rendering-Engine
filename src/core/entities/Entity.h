#pragma once
#include "entt.hpp"

class Entity
{

private:
	entt::entity entity;
	entt::registry* registry;

public:
	Entity() = default;
	Entity(const entt::entity& entity, entt::registry& registry) : entity(entity), registry(&registry) {}
	Entity(const Entity& other) = default;
	~Entity() = default;


	template<typename T, typename ...Args>
	T& addComponent(Args&& ...args) {
		T& component = registry->emplace<T>(entity, std::forward<Args>(args)...);
		return component;
	}

	template<typename T>
	bool hasComponent() {
		return registry->all_of<T>(entity);
	}

	template<typename T>
	bool removeComponent() {
		if (hasComponent<T>()) {
			T& component = registry->remove<T>(entity);
			return true;
		}
		return false;
	}

	template<typename T>
	T& getComponent() {
		if (hasComponent<T>()) {
			return registry->get<T>(entity);
		}
		throw std::runtime_error("Component does not exist");
	}

	template<typename ...T>
	std::tuple<T&...> getComponents() {
		if (registry->all_of<T...>(entity)) {
			return registry->get<T...>(entity);
		}
		throw std::runtime_error("Component does not exist");
	}

	const entt::entity& getID() const;
};

