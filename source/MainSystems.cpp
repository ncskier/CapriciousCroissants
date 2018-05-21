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
		int targetX = -1;
		int targetY = -1;

		if (loc.hasTemporaryDirection) {
			loc.dir = loc.realDir;
			loc.hasTemporaryDirection = false;
		}


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
//                        idle.sprite->setFrame(ENEMY_FRAME_DOWN);
						break;
					case LocationComponent::DOWN:
						loc.dir = LocationComponent::UP;
//                        idle.sprite->setFrame(ENEMY_FRAME_UP);
						break;
					case LocationComponent::LEFT:
						loc.dir = LocationComponent::RIGHT;
//                        idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
						break;
					case LocationComponent::RIGHT:
						loc.dir = LocationComponent::LEFT;
//                        idle.sprite->setFrame(ENEMY_FRAME_LEFT);
						break;
				}
				break;
			}
		}

		if (isFree) {
            if (targetY < loc.y) {
                idle.sprite->setZOrder(board->calculateDrawZ(targetX, targetY, false));
                board->getNode()->sortZOrder();
            }
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
			loc.isMoving = true;
		}

		loc.realDir = loc.dir;

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

		LocationComponent::direction lastFace = loc.dir;
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
		else if (std::abs(dY) <= std::abs(dX)) {
			if (dX >= 0) {
				loc.dir = LocationComponent::RIGHT;
			}
			else {
				loc.dir = LocationComponent::LEFT;
			}
		}
		else {
			loc.dir = lastFace;
		}


		

		//TODO Update the comp based on board/direction of entity

		cugl::Rect oldBounds = board->calculateDrawBounds(loc.x, loc.y);
		bool isFree = true;
		int targetX;
		int targetY;

		switch (loc.dir) {
		case LocationComponent::UP:
			targetX = loc.x;
			targetY = loc.y + movementDistance;
			break;
		case LocationComponent::DOWN:
			targetX = loc.x;
			targetY = loc.y - movementDistance;
			break;
		case LocationComponent::LEFT:
			targetX = loc.x - movementDistance;
			targetY = loc.y;
			break;
		case LocationComponent::RIGHT:
			targetX = loc.x + movementDistance;
			targetY = loc.y;
			break;
		}

		for (auto enemy = board->getEnemies().begin(); enemy != board->getEnemies().end(); enemy++) {
			LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
			if (loc2.x == targetX && loc2.y == targetY) {
				isFree = false;
			}
		}
		if (!isFree) {
			if (std::abs(dY) < std::abs(dX)) {
				if (dY >= 0) {
					loc.dir = LocationComponent::UP;
				}
				else {
					loc.dir = LocationComponent::DOWN;
				}
			}
			else if (std::abs(dY) > std::abs(dX)) {
				if (dX >= 0) {
					loc.dir = LocationComponent::RIGHT;
				}
				else {
					loc.dir = LocationComponent::LEFT;
				}
			}
			else {
				loc.dir = lastFace;
			}

			isFree = true;

			switch (loc.dir) {
			case LocationComponent::UP:
				targetX = loc.x;
				targetY = loc.y + movementDistance;
				break;
			case LocationComponent::DOWN:
				targetX = loc.x;
				targetY = loc.y - movementDistance;
				break;
			case LocationComponent::LEFT:
				targetX = loc.x - movementDistance;
				targetY = loc.y;
				break;
			case LocationComponent::RIGHT:
				targetX = loc.x + movementDistance;
				targetY = loc.y;
				break;
			}

			for (auto enemy = board->getEnemies().begin(); enemy != board->getEnemies().end(); enemy++) {
				LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
				if (loc2.x == targetX && loc2.y == targetY) {
					isFree = false;
				}
			}
		}


		if (isFree) {
            if (targetY < loc.y) {
                idle.sprite->setZOrder(board->calculateDrawZ(targetX, targetY, false));
                board->getNode()->sortZOrder();
            }
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
			loc.isMoving = true;
		}

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
				if (i == 0) {
                    board->getRemovedAllies().insert(ally);
//                    board->lose = true;
                } else {
                    ally->getSprite()->setVisible(false);
                    ally->getEndSprite()->setVisible(true);
                    ally->getEndSprite()->setFrame(ALLY_DEATH_IMG_START);
                    board->removeAlly(i);
                    std::stringstream key;
                    key << "int_ally_remove_" << i;
                    if (!idle._actions->isActive(key.str())) {
                        idle._actions->activate(key.str(), board->allyDeathAction, ally->getEndSprite());
                        idle._interruptingActions.insert(key.str());
                    }
                }
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

	//CULog("AttackRangedSystem");
	if (manager->hasComponent<LocationComponent>(entity)) {
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);
		IdleComponent idle = manager->getComponent<IdleComponent>(entity);
		RangeOrthoAttackComponent ranged = manager->getComponent<RangeOrthoAttackComponent>(entity);

		loc.isMoving = false;

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
				//std::set<size_t>::iterator enemyIter;
				//std::set<size_t>::iterator enemyIter;

				//for (enemyIter = _board->getAttackingEnemies().begin(); enemyIter != _board->getAttackingEnemies().end(); ++enemyIter) {


				
				//std::vector<std::shared_ptr<PlayerPawnModel>>::iterator
				for (int i = 0; i < board->getNumAllies(); i++) {
					std::shared_ptr<PlayerPawnModel> ally = board->getAlly(i);
					if (ally->getX() == closestAlignedAlly->getX() && ally->getY() == closestAlignedAlly->getY()) {
						board->insertAttackingEnemy(entity);
						ranged.target = ally;
                        if (i == 0) {
                            board->getRemovedAllies().insert(ally);
//                            board->lose = true;
                        } else {
                            ally->getSprite()->setVisible(false);
                            ally->getEndSprite()->setVisible(true);
                            ally->getEndSprite()->setFrame(ALLY_DEATH_IMG_START);
                            board->removeAlly(i);
                            std::stringstream key;
                            key << "int_ally_remove_" << i;
                            if (!idle._actions->isActive(key.str())) {
                                idle._actions->activate(key.str(), board->allyDeathAction, ally->getEndSprite());
                                idle._interruptingActions.insert(key.str());
                            }
                        }
						loc.isAttacking = true;
                        // Commented out so ranged enemies will not turn incorrectly
                        int shootDirectionX = (ally->getY() == loc.y)*copysign(1, ally->getX() - loc.x);
                        int shootDirectionY = (ally->getX() == loc.x)*copysign(1, ally->getY() - loc.y);
						loc.realDir = loc.dir;
						loc.hasTemporaryDirection = true;
						if(shootDirectionX > 0){
                            loc.dir = LocationComponent::RIGHT;
//                            idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
                        }
                        if (shootDirectionX < 0) {
                            loc.dir = LocationComponent::LEFT;
//                            idle.sprite->setFrame(ENEMY_FRAME_LEFT);
                        }
                        if (shootDirectionY > 0) {
                            loc.dir = LocationComponent::UP;
//                            idle.sprite->setFrame(ENEMY_FRAME_UP);
                        }
                        if (shootDirectionY < 0) {
                            loc.dir = LocationComponent::DOWN;
//                            idle.sprite->setFrame(ENEMY_FRAME_DOWN);
                        }
					}

				}

			}
			


			if (manager->hasComponent<DumbMovementComponent>(entity)) {
				switch (loc.dir) {
				case LocationComponent::UP:
					if (loc.y == board->getHeight() - 1) {
						loc.dir = LocationComponent::DOWN;
//                        idle.sprite->setFrame(ENEMY_FRAME_DOWN);
					}
					break;
				case LocationComponent::DOWN:
					if (loc.y == 0) {
						loc.dir = LocationComponent::UP;
//                        idle.sprite->setFrame(ENEMY_FRAME_UP);
					}
					break;
				case LocationComponent::LEFT:
					if (loc.x == 0) {
						loc.dir = LocationComponent::RIGHT;
//                        idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
					}
					break;
				case LocationComponent::RIGHT:
					if (loc.x == board->getWidth() - 1) {
						loc.dir = LocationComponent::LEFT;
//                        idle.sprite->setFrame(ENEMY_FRAME_LEFT);
					}
					break;
				}
			}

			manager->addComponent<LocationComponent>(entity, loc);
			manager->addComponent<IdleComponent>(entity, idle);
			manager->addComponent<RangeOrthoAttackComponent>(entity, ranged);
		}
	

	return true;
}




bool MovementImmobileSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board)
{
	return true;
}


bool SmartMovementFacingSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) {
	//Can assume entity has a SmartMovementComponent(required)

	if (manager->hasComponent<LocationComponent>(entity)) {
		//        DumbMovementComponent move = manager->getComponent<DumbMovementComponent>(entity);
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);
		IdleComponent idle = manager->getComponent<IdleComponent>(entity);
//        int movementDistance = manager->getComponent<SmartMovementComponent>(entity).movementDistance;

		cugl::Vec2 tile = cugl::Vec2(board->xOfIndex(board->getSelectedTile()), board->yOfIndex(board->getSelectedTile()));
		bool isOffset = board->offset != 0.0f;
		bool isRow = board->offsetRow;

		int offsetAmount = board->lengthToCells(board->offset, board->offsetRow);

		int ownX = loc.x;
		int ownY = loc.y;

		if (isOffset) {
			if (tile.y == loc.y && isRow) {
				ownX += offsetAmount;
				ownX = ownX % board->getWidth() + (ownX % board->getWidth() < 0 ? board->getWidth() : 0);
			}
			else if (tile.x == loc.x && !isRow) {
				ownY += offsetAmount;
				ownY = ownY % board->getHeight() + (ownY % board->getHeight() < 0 ? board->getHeight() : 0);
			}
		}


		if (board->getNumAllies() == 0) {
			return true;
		}
		std::shared_ptr<PlayerPawnModel> nearest = board->getAlly(0);
		int nearestX = nearest->getX();
		int nearestY = nearest->getY();
		if (isOffset) {
			if (tile.y == nearestY && isRow) {
				nearestX += offsetAmount;
				nearestX = nearestX % board->getWidth() + (nearestX % board->getWidth() < 0 ? board->getWidth() : 0);
			}
			else if (tile.x == nearestX && !isRow) {
				nearestY += offsetAmount;
				nearestY = nearestY % board->getHeight() + (nearestY % board->getHeight() < 0 ? board->getHeight() : 0);
			}
		}

		int minDist = (abs(ownX- nearestX) + abs(ownY- nearestY));
		int targetInt = 0;
		for (int i = 1; i < board->getNumAllies(); i++) {
			std::shared_ptr<PlayerPawnModel> temp = board->getAlly(i);
			int allyX = temp->getX();
			int allyY = temp->getY();
			if (isOffset) {
				if (tile.y == allyY && isRow) {
					allyX += offsetAmount;
					allyX = allyX % board->getWidth() + (allyX % board->getWidth() < 0 ? board->getWidth() : 0);
				}
				else if (tile.x == allyX && !isRow) {
					allyY += offsetAmount;
					allyY = allyY % board->getHeight() + (allyY % board->getHeight() < 0 ? board->getHeight() : 0);
				}
			}
			int dist = abs(ownX - allyX) + abs(ownY - allyY);
			if (dist <= minDist) {
				minDist = dist;
				nearestX = allyX;
				nearestY = allyY;
				targetInt = i;
			}
		}

		int dX = nearestX - ownX;
		int dY = nearestY - ownY;

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
		bool isFree = true;
		int targetX;
		int targetY;

		switch (loc.dir) {
		case LocationComponent::UP:
			targetX = ownX;
			targetY = ownY + 1;
			break;
		case LocationComponent::DOWN:
			targetX = ownX;
			targetY = ownY - 1;
			break;
		case LocationComponent::LEFT:
			targetX = ownX - 1;
			targetY = ownY;
			break;
		case LocationComponent::RIGHT:
			targetX = ownX + 1;
			targetY = ownY;
			break;
		}

		for (auto enemy = board->getEnemies().begin(); enemy != board->getEnemies().end(); enemy++) {
			LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
			int allyY = loc2.y;
			int allyX = loc2.x;
			if (isOffset) {
				if (tile.y == allyY && isRow) {
					allyX += offsetAmount;
					allyX = allyX % board->getWidth() + (allyX % board->getWidth() < 0 ? board->getWidth() : 0);
				}
				else if (tile.x == allyX && !isRow) {
					allyY += offsetAmount;
					allyY = allyY % board->getHeight() + (allyY % board->getHeight() < 0 ? board->getHeight() : 0);
				}
			}
			if (allyX == targetX && allyY == targetY) {
				isFree = false;
			}
		}
		if (!isFree) {
			if (std::abs(dY) < std::abs(dX)) {
				if (dY > 0) {
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
			isFree = true;

			switch (loc.dir) {
			case LocationComponent::UP:
				targetX = ownX;
				targetY = ownY + 1;
				break;
			case LocationComponent::DOWN:
				targetX = ownX;
				targetY = ownY - 1;
				break;
			case LocationComponent::LEFT:
				targetX = ownX - 1;
				targetY = ownY;
				break;
			case LocationComponent::RIGHT:
				targetX = ownX + 1;
				targetY = ownY;
				break;
			}

			for (auto enemy = board->getEnemies().begin(); enemy != board->getEnemies().end(); enemy++) {
				LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
				int allyY = loc2.y;
				int allyX = loc2.x;
				if (isOffset) {
					if (tile.y == allyY && isRow) {
						allyX += offsetAmount;
						allyX = allyX % board->getWidth() + (allyX % board->getWidth() < 0 ? board->getWidth() : 0);
					}
					else if (tile.x == allyX && !isRow) {
						allyY += offsetAmount;
						allyY = allyY % board->getHeight() + (allyY % board->getHeight() < 0 ? board->getHeight() : 0);
					}
				}
				if (allyX == targetX && allyY == targetY) {
					isFree = false;
				}
			}
		}



		//TODO Update the comp based on board/direction of entity

		switch (loc.dir) {
		case LocationComponent::UP:
//            idle.sprite->setFrame(ENEMY_FRAME_UP);
			break;
		case LocationComponent::DOWN:
//            idle.sprite->setFrame(ENEMY_FRAME_DOWN);
			break;
		case LocationComponent::LEFT:
//            idle.sprite->setFrame(ENEMY_FRAME_LEFT);
			break;
		case LocationComponent::RIGHT:
//            idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
			break;
		}
		manager->addComponent<LocationComponent>(entity, loc);
		manager->addComponent<IdleComponent>(entity, idle);
	}

	return true;
}

bool DumbMovementFacingSystem::updateEntity(EntityId entity, std::shared_ptr<BoardModel> board) {
	//Can assume entity has a SmartMovementComponent(required)

	if (manager->hasComponent<LocationComponent>(entity)) {
		//        DumbMovementComponent move = manager->getComponent<DumbMovementComponent>(entity);
		LocationComponent loc = manager->getComponent<LocationComponent>(entity);
		IdleComponent idle = manager->getComponent<IdleComponent>(entity);
		//        int movementDistance = manager->getComponent<SmartMovementComponent>(entity).movementDistance;

		cugl::Vec2 tile = cugl::Vec2(board->xOfIndex(board->getSelectedTile()), board->yOfIndex(board->getSelectedTile()));
		bool isOffset = board->offset != 0.0f;
		bool isRow = board->offsetRow;

		int offsetAmount = board->lengthToCells(board->offset, board->offsetRow);

		int ownX = loc.x;
		int ownY = loc.y;

		LocationComponent::direction startingDir = loc.dir;

		if (loc.hasTemporaryDirection) {
			loc.dir = loc.realDir;
		}


		if (isOffset) {
			if (tile.y == loc.y && isRow) {
				ownX += offsetAmount;
				ownX = ownX % board->getWidth() + (ownX % board->getWidth() < 0 ? board->getWidth() : 0);
			}
			else if (tile.x == loc.x && !isRow) {
				ownY += offsetAmount;
				ownY = ownY % board->getHeight() + (ownY % board->getHeight() < 0 ? board->getHeight() : 0);
			}
		}

		bool isFree = true;
		int targetX = -1;
		int targetY = -1;

		switch (loc.dir) {
		case LocationComponent::UP:
			if (ownY == board->getHeight() - 1) {
				loc.hasTemporaryDirection = true;
				loc.dir = LocationComponent::DOWN;
				targetX = ownX;
				targetY = ownY - 1;
			}
			else {
				targetX = ownX;
				targetY = ownY + 1;
			}
			break;
		case LocationComponent::DOWN:
			if (ownY == 0) {
				loc.hasTemporaryDirection = true;
				loc.dir = LocationComponent::UP;
				targetX = ownX;
				targetY = ownY + 1;
			}
			else {
				targetX = ownX;
				targetY = ownY - 1;
			}
			break;
		case LocationComponent::LEFT:
			if (ownX == 0) {
				loc.hasTemporaryDirection = true;
				loc.dir = LocationComponent::RIGHT;
				targetX = ownX + 1;
				targetY = ownY;
			}
			else {
				targetX = ownX - 1;
				targetY = ownY;
			}
			break;
		case LocationComponent::RIGHT:
			if (ownX == board->getWidth() - 1) {
				loc.hasTemporaryDirection = true;
				loc.dir = LocationComponent::LEFT;
				targetX = ownX - 1;
				targetY = ownY;
			}
			else {
				targetX = ownX + 1;
				targetY = ownY;
			}
			break;
		default:
			break;
		}

		std::vector<size_t> enemies = board->getEnemies();
		for (auto enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
			if ((*enemy) != entity) {
				LocationComponent loc2 = manager->getComponent<LocationComponent>((*enemy));
				int allyY = loc2.y;
				int allyX = loc2.x;
				if (isOffset) {
					if (tile.y == allyY && isRow) {
						allyX += offsetAmount;
						allyX = allyX % board->getWidth() + (allyX % board->getWidth() < 0 ? board->getWidth() : 0);
					}
					else if (tile.x == allyX && !isRow) {
						allyY += offsetAmount;
						allyY = allyY % board->getHeight() + (allyY % board->getHeight() < 0 ? board->getHeight() : 0);
					}
				}
				if (allyX == targetX && allyY == targetY) {
					isFree = false;
					loc.hasTemporaryDirection = true;

					switch (loc.dir) {
					case LocationComponent::UP:
						loc.dir = LocationComponent::DOWN;
						break;
					case LocationComponent::DOWN:
						loc.dir = LocationComponent::UP;
						break;
					case LocationComponent::LEFT:
						loc.dir = LocationComponent::RIGHT;
						break;
					case LocationComponent::RIGHT:
						loc.dir = LocationComponent::LEFT;
						break;
					}
					break;
				}
			}
		}

		if (startingDir != loc.dir) {
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
		}
		
		manager->addComponent<LocationComponent>(entity, loc);
		manager->addComponent<IdleComponent>(entity, idle);
	}

	return true;
}
