#include "MainSystems.h"
#include "System.h"
#include "EntityManager.h"


void MovementDumbSystem::updateEntity(EntityId entity, BoardModel board){
	//Can assume entity has a DumbMovementComponent(required)
	if (manager->hasComponent<LocationComponent>(entity)) {
//        DumbMovementComponent move = manager->getComponent<DumbMovementComponent>(entity);
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);

		//TODO Update the comp based on board/direction of entity
		loc.x++;
		loc.y++;

		manager->addComponent<LocationComponent>(entity, loc);
	}	
}
