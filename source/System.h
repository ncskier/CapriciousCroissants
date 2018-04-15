#pragma once
#include "ComponentType.h"
#include "Entity.h"
#include "cugl\cugl.h"

#include <memory>
#include <set>

class EntityManager;
class BoardModel;

class EntitySystem {
private:
	ComponentTypeSet requiredComponents;
	std::set<EntityId> entities;

protected:
	std::shared_ptr<EntityManager> manager;

public:
	typedef std::shared_ptr<EntitySystem> Ptr;

	EntitySystem(std::shared_ptr<EntityManager>& manager);

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

	size_t updateEntities(std::shared_ptr<BoardModel> board);

	virtual bool updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) = 0;
};
