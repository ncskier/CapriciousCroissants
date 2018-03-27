#pragma once
#include "System.h"
#include "Components.h"

class MovementDumbSystem : public EntitySystem {
public:
	explicit MovementDumbSystem(std::shared_ptr<EntityManager>& manager) :
		EntitySystem(manager) {
		setRequiredComponents({ ecs::getComponentType<DumbMovementComponent>() });
	}

	virtual void updateEntity(EntityId entity, BoardModel board) override;
};
