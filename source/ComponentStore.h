#pragma once

#include <Components.h>
#include <Entity.h>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <ComponentType.h>

class IComponentStore {
public:
	typedef std::unique_ptr<IComponentStore> Ptr;
};

//Class for storing components of a certain type and the entities they belong to
template<typename C>
class ComponentStore : IComponentStore {
	static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
private:
	std::unordered_map<EntityId, C> storage;
	ComponentType _type = typeid(C).hash_code();
public:
	

	ComponentStore() {
	}

	~ComponentStore() {
	}

	bool remove(const EntityId entity) {
		return (0 < storage.erase(entity));
	}

	bool has(EntityId entity) {
		return storage.find(entity) != storage.end();
	}

	bool add(const EntityId entity, C&& component) {
		/*if (storage.find(entity)) {
			storage.erase(entity);
		}
		return storage.insert(std::make_pair(entity, std::move(component))).second;*/
		storage[entity] = std::move(component);
		return true;
	}

	C extractCopy(EntityId entity) {
		C component = std::move(storage.at(entity));
		return component;
	}

	const std::unordered_map<EntityId, C>& getComponents() {
		return storage;
	}

};