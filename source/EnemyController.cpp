//
//  EnemyController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include <sstream>
#include "EnemyController.h"

using namespace cugl;


#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
EnemyController::EnemyController() {
}

/**
 * Initializes the controller contents, and starts the enemy turn
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param board    The game board
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool EnemyController::init(std::shared_ptr<ActionManager>& actions, const std::shared_ptr<BoardModel>& board, std::shared_ptr<EntityManager>& manager) {
	_actions = actions;
	_board = board;
	_entityManager = manager;

	_debug = false;
	_complete = false;
	_state = State::MOVE;

	return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void EnemyController::dispose() {
    CULog("dispose EnemyController");
	_board = nullptr;
    _actions = nullptr;
    _entityManager = nullptr;
    _complete = false;
    _state = State::MOVE;
    lose = false;
}

/*
int playerDistance(EnemyPawnModel enemy, PlayerPawnModel player);
void enemyMovement(EnemyPawnModel enemy);
void enemyAttack(EnemyPawnModel enemy, PlayerPawnModel player);
*/

int EnemyController::playerDistance(std::shared_ptr<EnemyPawnModel> enemy, std::shared_ptr<PlayerPawnModel> player) {
	return (abs(enemy->getX() - player->getX()) + abs(enemy->getY() - player->getY()));
}

int EnemyController::playerDistanceX(std::shared_ptr<EnemyPawnModel> enemy, std::shared_ptr<PlayerPawnModel> player) {
	return  (player->getX() - enemy->getX());
}

int EnemyController::playerDistanceY(std::shared_ptr<EnemyPawnModel> enemy, std::shared_ptr<PlayerPawnModel> player) {
	return (player->getY() - enemy->getY());
}

void EnemyController::enemyMove(std::shared_ptr<EnemyPawnModel> enemy, int enemyIdx) {
	enemy->move(_board->getWidth(), _board->getHeight());
}

void EnemyController::enemyAttack(std::shared_ptr<EnemyPawnModel> enemy, std::shared_ptr<PlayerPawnModel> player) {
	//unsure how to implement without or player death
	player->setXY(-1, -1);
}

std::shared_ptr<PlayerPawnModel> EnemyController::getClosestAllytoEnemy(std::shared_ptr<EnemyPawnModel>enemy) {
	if (_board->getNumAllies() == 0) {
		return nullptr;
	}
	std::shared_ptr<PlayerPawnModel> nearestPlayer = _board->getAlly(0);
	int distance;
	int minDistance;
	minDistance = 999;
	for (int i = 0; i < _board->getNumAllies(); i++) {
		distance = playerDistance(enemy, _board->getAlly(i));
		if (distance <= minDistance){
			minDistance = distance;
			nearestPlayer = _board->getAlly(i);
		}
	}
	return nearestPlayer;
}

int EnemyController::createDirection(int dx, int dy) {
	int direction = 0;
	if ((dx == 0) && (dy == 1)) { direction = 0; }
	if ((dx == 1) && (dy == 0)) { direction = 2; }
	if ((dx == 0) && (dy == -1)) { direction = 1; }
	if ((dx == -1) && (dy == 0)) { direction = 3; }
    return direction;
}

int EnemyController::getDirectionXComponent(EnemyPawnModel::Direction direction) {
	int component = 0;
	//if (direction == EnemyPawnModel::Direction::NORTH || direction == EnemyPawnModel::Direction::SOUTH) { component = 0; }
	if (direction == EnemyPawnModel::Direction::WEST) { component = -1; }
	if (direction == EnemyPawnModel::Direction::EAST) { component = 1; }
    return component;
}

int EnemyController::getDirectionYComponent(EnemyPawnModel::Direction direction) {
	int component = 0;
	//if (direction == EnemyPawnModel::Direction::WEST || direction == EnemyPawnModel::Direction::EAST) { component = 0; }
	if (direction == EnemyPawnModel::Direction::SOUTH) { component = -1; }
	if (direction == EnemyPawnModel::Direction::NORTH) { component = 1; }
    return component;
}

bool EnemyController::checkPlaceFree(std::shared_ptr<EnemyPawnModel> enemy) {
	int checkX = enemy->getX() + getDirectionXComponent(enemy->getDirection());
	int checkY = enemy->getY() + getDirectionYComponent(enemy->getDirection());
	if ( ((checkX < 0) || (checkY < 0)) || ((checkX >= _board->getWidth()) || (checkY >= _board->getHeight()))) { 
		checkX = enemy->getX() - getDirectionXComponent(enemy->getDirection());
		checkY = enemy->getY() - getDirectionYComponent(enemy->getDirection());
	}
    return (_board->getEnemy(checkX, checkY) == 0); 
}

void EnemyController::enemyMoveSmart(std::shared_ptr<EnemyPawnModel> enemy, std::shared_ptr<PlayerPawnModel> player) {
	if (player == nullptr) {
		return;
	}
	int moveX, moveY, distanceX, distanceY, dir;
	moveX = 1;
	moveY = 0;
	distanceX = playerDistanceX(enemy, player);
	distanceY = playerDistanceY(enemy, player);
	if (abs(distanceY) >= abs(distanceX)){
		moveY = 1;
		moveX = 0;
	}

	moveX = copysign(moveX, distanceX);
	moveY = copysign(moveY, distanceY);
	dir = createDirection(moveX, moveY);
	enemy->setDirection(dir);
	enemy->updateSpriteDirection();

}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * The method called to update the enemy turn.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void EnemyController::update(float timestep) {
//    CULog("EnemyController Update");

    if (_state == State::MOVE) {
        // MOVE
        /*for (int i = 0; i < _board->getNumEnemies(); i++) {
            std::shared_ptr<EnemyPawnModel> enemy = _board->getEnemy(i);
            if (enemy->getX() != -1) {
                // Store enemy location
                Rect oldBounds = _board->calculateDrawBounds(enemy->getX(), enemy->getY());
                
                // Move enemy
				if (enemy->getAI() == 1) {
					enemyMoveSmart(enemy, getClosestAllytoEnemy(enemy));
				}
				if (checkPlaceFree(enemy)) {
					enemy->move(_board->getWidth(), _board->getHeight());
				}
				else if (enemy->getAI() == 0) {
					enemy->turnAround();
				}
				
                // Create animation
                Rect newBounds = _board->calculateDrawBounds(enemy->getX(), enemy->getY());
                Vec2 movement = newBounds.origin - oldBounds.origin;
                int tiles = _board->lengthToCells(movement.length());
                std::stringstream key;
                key << "int_enemy_move_" << i;
                std::shared_ptr<MoveBy> moveAction = MoveBy::alloc(movement, ((float)tiles)/ENEMY_IMG_SPEED);
                _actions->activate(key.str(), moveAction, enemy->getSprite());
                _interruptingActions.insert(key.str());
            }
        }*/
        
		// CALL FOR MOVEMENT UPDATE ON ENTITIES
        _entityManager->updateEntities(_board, EntityManager::movement);

        // Update z positions
        _board->updateNodes(false);
        
        _state = State::ATTACK;
    } else if (_state == State::ATTACK) {
        // ATTACK
        /*for (int i = 0; i < _board->getNumEnemies(); i++) {
            std::shared_ptr<EnemyPawnModel> enemy = _board->getEnemy(i);
			if (enemy->getAI() == 1) {
				enemyMoveSmart(enemy, getClosestAllytoEnemy(enemy));
			}
            if (enemy->getX() != -1) {
                // Attack
                for (int j = 0; j < _board->getNumAllies(); j++) {
                    std::shared_ptr<PlayerPawnModel> ally = _board->getAlly(j);
                    if (playerDistance(enemy, ally) < 1) {
                        _board->removeAlly(j);
                        // Lose when Mika is destroyed (Mika is index 0 ally)
                        lose = (j == 0);
                        
                        // Create animation
                        std::stringstream key;
                        key << "int_ally_remove_" << j;
                        _actions->activate(key.str(), _board->allyRemoveAction, ally->getSprite());
                        _interruptingActions.insert(key.str());
                    }
                }
            }*/
		_entityManager->updateEntities(_board, EntityManager::attack);
        _state = State::CHECK;
    } else {
        // CHECK
        std::set<std::shared_ptr<PlayerPawnModel>>::iterator it;
        for (it = _board->getRemovedAllies().begin(); it != _board->getRemovedAllies().end(); ++it) {
            _board->getNode()->removeChild((*it)->getSprite());
        }
        _board->clearRemovedAllies();
        
        setComplete(true);
        
//        lose = true;
//        for (int i = 0; i < _board->getNumAllies(); i++) {
//            std::shared_ptr<PlayerPawnModel> temp = _board->getAlly(i);
//            if (temp->getX() != -1) {
//                lose = false;
//            }
//        }
        
        // Update board node positions
        _board->updateNodes();
    }
}



/**
 * Resets the status of the game so that we can play again.
 */
void EnemyController::reset() {
    _complete = false;
    _state = State::MOVE;
}
