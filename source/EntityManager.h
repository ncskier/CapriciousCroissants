//
// EntityManager.h
// Header file for a manager of entitys and their components
#pragma once


#include "Components.h"
#include "Entity.h"
#include "ComponentType.h"
#include "ComponentStore.h"
#include "System.h"

#include <map>
#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <cassert>
#include <vector>

// Class of an entity manager that maintains the connections between entities and their components
class EntityManager {
private:
	long nextEntityId = 0;
	std::unordered_map<EntityId, ComponentTypeSet> entityCache;
	std::map<ComponentType, IComponentStore::Ptr> componentStores;
	std::vector<EntitySystem::Ptr> systems;

public:
	EntityManager();
	virtual ~EntityManager();

	//Creates an empty component store for the type C
	template<typename C>
	bool createComponentStore() {
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		return componentStores.insert(std::make_pair(typeid(C).hash_code(), IComponentStore::Ptr(new ComponentStore<C>()))).second;
	}

	//Gets a component store for the type C, if it does not exist then it will create a new empty one and return that
	template<typename C>
	ComponentStore<C>& getComponentStore() {
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		auto componentStore = componentStores.find(typeid(C).hash_code());
		if (componentStore == componentStores.end()) {
			createComponentStore<C>();
			componentStore = componentStores.find(typeid(C).hash_code());
		}
		return reinterpret_cast<ComponentStore<C>&>(*(componentStore->second));
	}

	//Returns the id for the next created entity, an empty entity
	EntityId createEntity() {
		assert(nextEntityId <= std::numeric_limits<EntityId>::max());
		entityCache.insert(std::make_pair(nextEntityId, ComponentTypeSet()));
		return nextEntityId++;
	}

	//Adds a component to an entity in both the component store and on the entity cache, utilizes an r-value reference
	template<typename C>
	bool addComponent(const EntityId entityId, C&& component) {
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		auto entity = entityCache.find(entityId);
		if (entity == entityCache.end()) {
			throw std::runtime_error("Entity requested does not exist");
		}
		(*entity).second.insert(typeid(C).hash_code());
		return getComponentStore<C>().add(entityId, std::move(component));
	}

	//Adds a component to an entity in both the component store and on the entity cache
	template<typename C>
	bool addComponent(const EntityId entityId, C component) {
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		auto entity = entityCache.find(entityId);
		if (entity == entityCache.end()) {
			throw std::runtime_error("Entity requested does not exist");
		}
		(*entity).second.insert(typeid(C).hash_code());
		return getComponentStore<C>().add(entityId, std::move(component));
	}

	//Returns if an entity has a component of type C or not
	template<typename C>
	bool hasComponent(const EntityId entityId) {
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		return getComponentStore<C>().has(entityId);
	}

	//Returns a component of type C on an entity, DOES NOT CHECK IF THE ENTITY ACTUALLY HAS ONE OR NOT
	template<typename C>
	C getComponent(const EntityId entityId) {
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		ComponentStore<C> store = getComponentStore<C>();
		return store.extractCopy(entityId);
	}

	void addSystem(const EntitySystem::Ptr& systemPtr);

	size_t registerEntity(const EntityId entity);

	size_t unregisterEntity(const EntityId entity);

	size_t updateEntities();
};
