#include "EntityManager.h"

#include <algorithm>

EntityManager::EntityManager() :
	entityCache(),
	componentStores(),
	attackSystems(),
	movementSystems(), 
	damageSystems(), 
	playerLimitSystems(), 
	onTurnSystems(),
	allSystems() {

}

EntityManager::~EntityManager() {
}

void EntityManager::addSystem(const EntitySystem::Ptr& systemPtr, SystemType type) {
	if ((!systemPtr) || (systemPtr->getRequiredComponents().empty())) {
		throw std::runtime_error("System needs required components");
	}
	switch (type) {
	case attack:
		attackSystems.push_back(systemPtr);
		break;
	case movement:
		movementSystems.push_back(systemPtr);
		break;
	case damage:
		damageSystems.push_back(systemPtr);
		break;
	case playerLimit:
		playerLimitSystems.push_back(systemPtr);
		break;
	case onTurn:
		onTurnSystems.push_back(systemPtr);
		break;
	}

	allSystems.push_back(systemPtr);
}

size_t EntityManager::registerEntity(const EntityId entityId) {
	size_t numSystems = 0;

	auto entity = entityCache.find(entityId);
	if (entity == entityCache.end()) {
		throw std::runtime_error("Entity does not exist");
	}
	auto entityComponents = (*entity).second;

	for (auto system = allSystems.begin(); system != allSystems.end(); ++system) {
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
	for (auto system = allSystems.begin(); system != allSystems.end(); ++system) {
		numSystems += (*system)->unregisterEntity(entityId);
	}

	return numSystems;
}

size_t EntityManager::updateEntities(std::shared_ptr<BoardModel> board, SystemType type) {
	size_t numUpdated = 0;
	systems tempSystem;
	switch (type) {
	case attack:
		tempSystem = attackSystems;
		break;
	case movement:
		tempSystem = movementSystems;
		break;
	case damage:
		tempSystem = damageSystems;
		break;
	case playerLimit:
		tempSystem = playerLimitSystems;
		break;
	case onTurn:
		tempSystem = onTurnSystems;
		break;
	}
	for (auto system = tempSystem.begin(); system != tempSystem.end(); ++system) {
		numUpdated += (*system)->updateEntities(board);
	}

	return numUpdated;
}

size_t EntityManager::updateEntities(std::shared_ptr<BoardModel> board, SystemType type, std::shared_ptr<EnemyController> controller) {
	size_t numUpdated = 0;
	systems tempSystem;
	switch (type) {
	case attack:
		tempSystem = attackSystems;
		break;
	case movement:
		tempSystem = movementSystems;
		break;
	case damage:
		tempSystem = damageSystems;
		break;
	case playerLimit:
		tempSystem = playerLimitSystems;
		break;
	case onTurn:
		tempSystem = onTurnSystems;
		break;
	}
	for (auto system = tempSystem.begin(); system != tempSystem.end(); ++system) {
		numUpdated += (*system)->updateEntities(board, controller);
	}

	return numUpdated;
}
