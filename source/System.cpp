#include "System.h"


EntitySystem::EntitySystem(std::shared_ptr<EntityManager>& manager) :
	manager(manager) {
}

EntitySystem::~EntitySystem() {
}

size_t EntitySystem::updateEntities(std::shared_ptr<BoardModel> board) {
	size_t numEntitiesUpdated = 0;
	for (auto entity = entities.begin(); entity != entities.end(); ++entity) {
		if (updateEntity(*entity, board)) {
			numEntitiesUpdated++;
		}
	}

	return numEntitiesUpdated;
}

