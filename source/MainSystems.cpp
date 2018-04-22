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
					idle.sprite->setFrame(1);
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
					idle.sprite->setFrame(2);
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
					idle.sprite->setFrame(3);
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
					idle.sprite->setFrame(0);
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
						idle.sprite->setFrame(1);
						break;
					case LocationComponent::DOWN:
						loc.dir = LocationComponent::UP;
						idle.sprite->setFrame(2);
						break;
					case LocationComponent::LEFT:
						loc.dir = LocationComponent::RIGHT;
						idle.sprite->setFrame(3);
						break;
					case LocationComponent::RIGHT:
						loc.dir = LocationComponent::LEFT;
						idle.sprite->setFrame(0);
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
		int movementDistance = manager->getComponent<SmartMovementComponent>(entity).movementDistance;


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
			if (loc.y == board->getHeight() - movementDistance) {
				loc.dir = LocationComponent::DOWN;
				targetX = loc.x;
				targetY = loc.y - movementDistance;
				idle.sprite->setFrame(1);
			}
			else {
				targetX = loc.x;
				targetY = loc.y + movementDistance;
			}
			break;
		case LocationComponent::DOWN:
			if (loc.y == 0) {
				loc.dir = LocationComponent::UP;
				targetX = loc.x;
				targetY = loc.y + movementDistance;
				idle.sprite->setFrame(2);
			}
			else {
				targetX = loc.x;
				targetY = loc.y - movementDistance;
			}
			break;
		case LocationComponent::LEFT:
			if (loc.x == 0) {
				loc.dir = LocationComponent::RIGHT;
				targetX = loc.x + movementDistance;
				targetY = loc.y;
				idle.sprite->setFrame(3);
			}
			else {
				targetX = loc.x - movementDistance;
				targetY = loc.y;
			}
			break;
		case LocationComponent::RIGHT:
			if (loc.x == board->getWidth() - movementDistance) {
				loc.dir = LocationComponent::LEFT;
				targetX = loc.x - movementDistance;
				targetY = loc.y;
				idle.sprite->setFrame(0);
			}
			else {
				targetX = loc.x + movementDistance;
				targetY = loc.y;
			}
			break;
		}

		switch (loc.dir) {
			case LocationComponent::UP:
				idle.sprite->setFrame(2);
				break;
			case LocationComponent::DOWN:
				idle.sprite->setFrame(1);
				break;
			case LocationComponent::LEFT:
				idle.sprite->setFrame(0);
				break;
			case LocationComponent::RIGHT:
				idle.sprite->setFrame(3);
				break;
		}

		for (auto enemy = board->getEnemies().begin(); enemy != board->getEnemies().end(); enemy++) {
			LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
			if (loc2.x == targetX && loc2.y == targetY) {
				isFree = false;

				switch (loc.dir) {
				case LocationComponent::UP:
					loc.dir = LocationComponent::DOWN;
					idle.sprite->setFrame(1);
					break;
				case LocationComponent::DOWN:
					loc.dir = LocationComponent::UP;
					idle.sprite->setFrame(2);
					break;
				case LocationComponent::LEFT:
					loc.dir = LocationComponent::RIGHT;
					idle.sprite->setFrame(3);
					break;
				case LocationComponent::RIGHT:
					loc.dir = LocationComponent::LEFT;
					idle.sprite->setFrame(0);
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
	CULog("AttackMeleeSystem");

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
						idle.sprite->setFrame(1);
					}
					break;
				case LocationComponent::DOWN:
					if (loc.y == 0) {
						loc.dir = LocationComponent::UP;
						idle.sprite->setFrame(2);
					}
					break;
				case LocationComponent::LEFT:
					if (loc.x == 0) {
						loc.dir = LocationComponent::RIGHT;
						idle.sprite->setFrame(3);
					}
					break;
				case LocationComponent::RIGHT:
					if (loc.x == board->getWidth() - 1) {
						loc.dir = LocationComponent::LEFT;
						idle.sprite->setFrame(0);
					}
					break;
			}
		}
		
		//There's probably a better way to do this
		if (manager->hasComponent<SmartMovementComponent>(entity)) {
			if (board->getNumAllies() > 0) {

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
	
			}
		}

		manager->addComponent<LocationComponent>(entity, loc);
		manager->addComponent<IdleComponent>(entity, idle);
	}

	return true;
}


bool AttackRangedSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) {

	CULog("AttackRangedSystem");
	if (manager->hasComponent<LocationComponent>(entity)) {
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);
		IdleComponent idle = manager->getComponent<IdleComponent>(entity);

		//find closest ally who is on x or y
		//turn to x or y
		//attack

		std::shared_ptr<PlayerPawnModel> closestAlignedAlly = nullptr;
		for (int i = 0; i < board->getNumAllies(); i++) {
			std::shared_ptr<PlayerPawnModel> ally = board->getAlly(i);
			if (ally->getX() == loc.x || ally->getY() == loc.y) {
				if (closestAlignedAlly == nullptr || (abs(ally->getX() - loc.x) + abs(ally->getY() - loc.y) < abs(closestAlignedAlly->getX() - loc.x) + abs(closestAlignedAlly->getY() - loc.y)))
					closestAlignedAlly = ally;
					
			}
		}

			if (closestAlignedAlly != nullptr) {
				for (int i = 0; i < board->getNumAllies(); i++) {
					std::shared_ptr<PlayerPawnModel> ally = board->getAlly(i);
					if (ally->getX() == closestAlignedAlly->getX() && ally->getY() == closestAlignedAlly->getY()) {
						board->removeAlly(i);
						if (i == 0) {
							board->lose = true;
						}
						int shootDirectionX = (ally->getY() == loc.y)*copysign(1, ally->getX() - loc.x);
						int shootDirectionY = (ally->getX() == loc.x)*copysign(1, ally->getY() - loc.y);
						if(shootDirectionX > 0){
							loc.dir = LocationComponent::RIGHT;
							idle.sprite->setFrame(3);
						}
						if (shootDirectionX < 0) {
							loc.dir = LocationComponent::LEFT;
							idle.sprite->setFrame(0);
						}
						if (shootDirectionY > 0) {
							loc.dir = LocationComponent::UP;
							idle.sprite->setFrame(2);
						}
						if (shootDirectionY < 0) {
							loc.dir = LocationComponent::DOWN;
							idle.sprite->setFrame(1);
						}

						std::stringstream key;
						key << "int_ally_remove_" << i;
						idle._actions->activate(key.str(), board->allyRemoveAction, ally->getSprite());
						idle._interruptingActions.insert(key.str());
					}

				}
				//if (ally->getX() == loc.x && ally->getY() == loc.y) {

			}
			manager->addComponent<LocationComponent>(entity, loc);
			manager->addComponent<IdleComponent>(entity, idle);
		}
	
	return true;
}




bool MovementImmobileSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board)
{
	return true;
}

