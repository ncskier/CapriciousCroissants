#include "MainSystems.h"
#include "System.h"
#include "EntityManager.h"
#include <sstream>


bool MovementDumbSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board){
	//Can assume entity has a DumbMovementComponent(required)
	if (manager->hasComponent<LocationComponent>(entity)) {
//        DumbMovementComponent move = manager->getComponent<DumbMovementComponent>(entity);
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);
		IdleComponent idle = manager->getComponent<IdleComponent>(entity);

		//TODO Update the comp based on board/direction of entity

		cugl::Rect oldBounds = board->calculateDrawBounds(loc.x, loc.y);
		bool isFree = true;
		int targetX;
		int targetY;

		switch (loc.dir) {
			case LocationComponent::UP:
				if (loc.y == board->getHeight() - 1) {
					loc.dir = LocationComponent::DOWN;
					targetX = loc.x;
					targetY = loc.y - 1;
					idle.sprite->setFrame(ENEMY_FRAME_DOWN);
				} else {
					targetX = loc.x;
					targetY = loc.y + 1;
				}
				break;
			case LocationComponent::DOWN:
				if (loc.y == 0) {
					loc.dir = LocationComponent::UP;
					targetX = loc.x;
					targetY = loc.y + 1;
					idle.sprite->setFrame(ENEMY_FRAME_UP);
				}
				else {
					targetX = loc.x;
					targetY = loc.y - 1;
				}
				break;
			case LocationComponent::LEFT:
				if (loc.x == 0) {
					loc.dir = LocationComponent::RIGHT;
					targetX = loc.x + 1;
					targetY = loc.y;
					idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
				}
				else {
					targetX = loc.x - 1;
					targetY = loc.y;
				}
				break;
			case LocationComponent::RIGHT:
				if (loc.x == board->getWidth() - 1) {
					loc.dir = LocationComponent::LEFT;
					targetX = loc.x - 1;
					targetY = loc.y;
					idle.sprite->setFrame(ENEMY_FRAME_LEFT);
				}
				else {
					targetX = loc.x + 1;
					targetY = loc.y;
				}
				break;
		}



		for (auto enemy = board->getEnemies().begin(); enemy != board->getEnemies().end(); enemy++) {
			LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
			if (loc2.x == targetX && loc2.y == targetY) {
				isFree = false;

				switch (loc.dir) {
					case LocationComponent::UP:
						loc.dir = LocationComponent::DOWN;
						idle.sprite->setFrame(ENEMY_FRAME_DOWN);
						break;
					case LocationComponent::DOWN:
						loc.dir = LocationComponent::UP;
						idle.sprite->setFrame(ENEMY_FRAME_UP);
						break;
					case LocationComponent::LEFT:
						loc.dir = LocationComponent::RIGHT;
						idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
						break;
					case LocationComponent::RIGHT:
						loc.dir = LocationComponent::LEFT;
						idle.sprite->setFrame(ENEMY_FRAME_LEFT);
						break;
				}
				break;
			}
		}

		if (isFree) {
			loc.x = targetX;
			loc.y = targetY;

			cugl::Rect newBounds = board->calculateDrawBounds(loc.x, loc.y);
			cugl::Vec2 movement = newBounds.origin - oldBounds.origin;
			int tiles = board->lengthToCells(movement.length());
			std::stringstream key;
			key << "int_enemy_move_" << entity;
			std::shared_ptr<cugl::MoveBy> moveAction = cugl::MoveBy::alloc(movement, ((float)tiles) / idle.speed[0]);
			idle._actions->activate(key.str(), moveAction, idle.sprite);
			idle._interruptingActions.insert(key.str());
		}

		manager->addComponent<LocationComponent>(entity, loc);
		manager->addComponent<IdleComponent>(entity, idle);
	}	

	return true;
}


bool MovementSmartSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) {
	//Can assume entity has a SmartMovementComponent(required)
	if (manager->hasComponent<LocationComponent>(entity)) {
		//        DumbMovementComponent move = manager->getComponent<DumbMovementComponent>(entity);
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);
		IdleComponent idle = manager->getComponent<IdleComponent>(entity);

		if (board->getNumAllies() == 0) {
			return true;
		}
		std::shared_ptr<PlayerPawnModel> nearest = board->getAlly(0);
		int minDist = (abs(loc.x - nearest->getX()) + abs(loc.y - nearest->getY()));
		for (int i = 1; i < board->getNumAllies(); i++) {
			std::shared_ptr<PlayerPawnModel> temp = board->getAlly(i);
			int dist = (abs(loc.x - temp->getX()) + abs(loc.y - temp->getY()));
			if (dist <= minDist) {
				minDist = dist;
				nearest = temp;
			}
		}

		int dX = nearest->getX() - loc.x;
		int dY = nearest->getY() - loc.y;


		if (std::abs(dY) >= std::abs(dX)) {
			if (dY >= 0) {
				loc.dir = LocationComponent::UP;
			}
			else {
				loc.dir = LocationComponent::DOWN;
			}
		}
		else {
			if (dX >= 0) {
				loc.dir = LocationComponent::RIGHT;
			}
			else {
				loc.dir = LocationComponent::LEFT;
			}
		}

		

		//TODO Update the comp based on board/direction of entity

		cugl::Rect oldBounds = board->calculateDrawBounds(loc.x, loc.y);
		bool isFree = true;
		int targetX;
		int targetY;

		switch (loc.dir) {
		case LocationComponent::UP:
			if (loc.y == board->getHeight() - 1) {
				loc.dir = LocationComponent::DOWN;
				targetX = loc.x;
				targetY = loc.y - 1;
				idle.sprite->setFrame(ENEMY_FRAME_DOWN);
			}
			else {
				targetX = loc.x;
				targetY = loc.y + 1;
			}
			break;
		case LocationComponent::DOWN:
			if (loc.y == 0) {
				loc.dir = LocationComponent::UP;
				targetX = loc.x;
				targetY = loc.y + 1;
				idle.sprite->setFrame(ENEMY_FRAME_UP);
			}
			else {
				targetX = loc.x;
				targetY = loc.y - 1;
			}
			break;
		case LocationComponent::LEFT:
			if (loc.x == 0) {
				loc.dir = LocationComponent::RIGHT;
				targetX = loc.x + 1;
				targetY = loc.y;
				idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
			}
			else {
				targetX = loc.x - 1;
				targetY = loc.y;
			}
			break;
		case LocationComponent::RIGHT:
			if (loc.x == board->getWidth() - 1) {
				loc.dir = LocationComponent::LEFT;
				targetX = loc.x - 1;
				targetY = loc.y;
				idle.sprite->setFrame(ENEMY_FRAME_LEFT);
			}
			else {
				targetX = loc.x + 1;
				targetY = loc.y;
			}
			break;
		}

		switch (loc.dir) {
			case LocationComponent::UP:
				idle.sprite->setFrame(ENEMY_FRAME_UP);
				break;
			case LocationComponent::DOWN:
				idle.sprite->setFrame(ENEMY_FRAME_DOWN);
				break;
			case LocationComponent::LEFT:
				idle.sprite->setFrame(ENEMY_FRAME_LEFT);
				break;
			case LocationComponent::RIGHT:
				idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
				break;
		}

		for (auto enemy = board->getEnemies().begin(); enemy != board->getEnemies().end(); enemy++) {
			LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
			if (loc2.x == targetX && loc2.y == targetY) {
				isFree = false;

				switch (loc.dir) {
				case LocationComponent::UP:
					loc.dir = LocationComponent::DOWN;
					idle.sprite->setFrame(ENEMY_FRAME_DOWN);
					break;
				case LocationComponent::DOWN:
					loc.dir = LocationComponent::UP;
					idle.sprite->setFrame(ENEMY_FRAME_UP);
					break;
				case LocationComponent::LEFT:
					loc.dir = LocationComponent::RIGHT;
					idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
					break;
				case LocationComponent::RIGHT:
					loc.dir = LocationComponent::LEFT;
					idle.sprite->setFrame(ENEMY_FRAME_LEFT);
					break;
				}
			}
		}

		if (isFree) {
			loc.x = targetX;
			loc.y = targetY;

			cugl::Rect newBounds = board->calculateDrawBounds(loc.x, loc.y);
			cugl::Vec2 movement = newBounds.origin - oldBounds.origin;
			int tiles = board->lengthToCells(movement.length());
			std::stringstream key;
			key << "int_enemy_move_" << entity;
			std::shared_ptr<cugl::MoveBy> moveAction = cugl::MoveBy::alloc(movement, ((float)tiles) / idle.speed[0]);
			idle._actions->activate(key.str(), moveAction, idle.sprite);
			idle._interruptingActions.insert(key.str());
		}

		manager->addComponent<LocationComponent>(entity, loc);
		manager->addComponent<IdleComponent>(entity, idle);
		manager->addComponent<LocationComponent>(entity, loc);
	}

	return true;
}


bool AttackMeleeSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) {
	//Can assume entity has a AttackMeleeComponent(required)

	if (manager->hasComponent<LocationComponent>(entity)) {
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);
		IdleComponent idle = manager->getComponent<IdleComponent>(entity);

		for (int i = 0; i < board->getNumAllies(); i++) {
			std::shared_ptr<PlayerPawnModel> ally = board->getAlly(i);
			if (ally->getX() == loc.x && ally->getY() == loc.y) {
				board->removeAlly(i);
				if (i == 0) {
					board->lose = true;
				}

				std::stringstream key;
				key << "int_ally_remove_" << i;
				idle._actions->activate(key.str(), board->allyRemoveAction, ally->getSprite());
				idle._interruptingActions.insert(key.str());
			}
		}

		if (manager->hasComponent<DumbMovementComponent>(entity)) {
			switch (loc.dir) { 
				case LocationComponent::UP:
					if (loc.y == board->getHeight() - 1) {
						loc.dir = LocationComponent::DOWN;
						idle.sprite->setFrame(ENEMY_FRAME_DOWN);
					}
					break;
				case LocationComponent::DOWN:
					if (loc.y == 0) {
						loc.dir = LocationComponent::UP;
						idle.sprite->setFrame(ENEMY_FRAME_UP);
					}
					break;
				case LocationComponent::LEFT:
					if (loc.x == 0) {
						loc.dir = LocationComponent::RIGHT;
						idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
					}
					break;
				case LocationComponent::RIGHT:
					if (loc.x == board->getWidth() - 1) {
						loc.dir = LocationComponent::LEFT;
						idle.sprite->setFrame(ENEMY_FRAME_LEFT);
					}
					break;
			}
		}

		manager->addComponent<LocationComponent>(entity, loc);
		manager->addComponent<IdleComponent>(entity, idle);
	}

	return true;
}
