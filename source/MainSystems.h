#pragma once
#include "System.h"
#include "Components.h"
#include "BoardModel.h"

class MovementDumbSystem : public EntitySystem {
public:
	explicit MovementDumbSystem(std::shared_ptr<EntityManager>& manager) :
		EntitySystem(manager) {
		setRequiredComponents({ ecs::getComponentType<DumbMovementComponent>() });
	}

	virtual bool updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) override;
};

class MovementSmartSystem : public EntitySystem {
public:
	explicit MovementSmartSystem(std::shared_ptr<EntityManager>& manager) :
		EntitySystem(manager) {
		setRequiredComponents({ ecs::getComponentType<SmartMovementComponent>() });
	}

	virtual bool updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) override;
};

class AttackMeleeSystem : public EntitySystem {
public:
	explicit AttackMeleeSystem(std::shared_ptr<EntityManager>& manager) :
		EntitySystem(manager) {
		setRequiredComponents({ ecs::getComponentType<MeleeAttackComponent>() });
	}

	virtual bool updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) override;

};