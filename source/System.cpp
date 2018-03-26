#include "System.h"


EntitySystem::EntitySystem(EntityManager& manager) :
	manager(manager) {
}

EntitySystem::~EntitySystem() {
}

size_t EntitySystem::updateEntities() {
	size_t numEntitiesUpdated = 0;
	for (auto entity = entities.begin(); entity != entities.end(); ++entity) {
		updateEntity(*entity);
		numEntitiesUpdated++;
	}

	return numEntitiesUpdated;
}
