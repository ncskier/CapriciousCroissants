#include <EntityManager.h>

#include <algorithm>

EntityManager::EntityManager() :
	entityCache(),
	componentStores(),
	systems() {

}

EntityManager::~EntityManager() {
}

void EntityManager::addSystem(const EntitySystem::Ptr& systemPtr) {
	if ((!systemPtr) || (systemPtr->getRequiredComponents().empty())) {
		throw std::runtime_error("System needs required components");
	}

	systems.push_back(systemPtr);
}

size_t EntityManager::registerEntity(const EntityId entityId) {
	size_t numSystems = 0;

	auto entity = entityCache.find(entityId);
	if (entity == entityCache.end()) {
		throw std::runtime_error("Entity does not exist");
	}
	auto entityComponents = (*entity).second;

	for (auto system = systems.begin(); system != systems.end(); ++system) {
		auto systemRequiredComponents = (*system)->getRequiredComponents();
		if (std::includes(entityComponents.begin(), entityComponents.end(), systemRequiredComponents.begin(), systemRequiredComponents.end())) {
			(*system)->registerEntity(entityId);
			numSystems++;
		}
	}

	return numSystems;
}


size_t EntityManager::unregisterEntity(const EntityId entityId) {
	size_t numSystems = 0;

	auto entity = entityCache.find(entityId);
	if (entity == entityCache.end()) {
		throw std::runtime_error("Entity does not exist");
	}

	auto entityComponents = (*entity).second;
	for (auto system = systems.begin(); system != systems.end(); ++system) {
		numSystems += (*system)->unregisterEntity(entityId);
	}

	return numSystems;
}

size_t EntityManager::updateEntities() {
	size_t numUpdated = 0;
	for (auto system = systems.begin(); system != systems.end(); ++system) {
		numUpdated += (*system)->updateEntities();
	}

	return numUpdated;
}
