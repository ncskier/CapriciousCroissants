#pragma once
#include "ComponentType.h"
#include "Entity.h"

#include <memory>
#include <set>

class EntityManager;

class EntitySystem {
private:
	ComponentTypeSet requiredComponents;
	std::set<EntityId> entities;
	EntityManager& manager;

public:
	typedef std::shared_ptr<EntitySystem> Ptr;

	EntitySystem(EntityManager& manager);

	~EntitySystem();

	void setRequiredComponents(ComponentTypeSet&& required) {
		requiredComponents = std::move(required);
	}

	const ComponentTypeSet& getRequiredComponents() const {
		return requiredComponents;
	}

	bool registerEntity(EntityId entity) {
		return entities.insert(entity).second;
	}

	size_t unregisterEntity(EntityId entity) {
		return entities.erase(entity);
	}

	bool hasEntity(EntityId entity) {
		return entities.find(entity) != entities.end();
	}

	size_t updateEntities();

	virtual void updateEntity(EntityId entity) = 0;
};
