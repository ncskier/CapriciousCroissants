//
//  EnemyController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

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
bool EnemyController::init(std::shared_ptr<ActionManager>& actions, const std::shared_ptr<BoardModel>& board) {
    _actions = actions;
    _board = board;
    
    _debug = false;
    _complete = false;
    _state = State::MOVE;

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void EnemyController::dispose() {
    _board = nullptr;
}

/*
int playerDistance(EnemyPawnModel enemy, PlayerPawnModel player);
void enemyMovement(EnemyPawnModel enemy);
void enemyAttack(EnemyPawnModel enemy, PlayerPawnModel player);
*/

int EnemyController::playerDistance(std::shared_ptr<EnemyPawnModel> enemy, std::shared_ptr<PlayerPawnModel> player) {
	return (abs(enemy->getX() - player->getX()) + abs(enemy->getY() - player->getY()));
}

void EnemyController::enemyMove(std::shared_ptr<EnemyPawnModel> enemy, int enemyIdx) {
    enemy->move(_board->getWidth(), _board->getHeight());
}

void EnemyController::enemyAttack(std::shared_ptr<EnemyPawnModel> enemy, std::shared_ptr<PlayerPawnModel> player) {
	//unsure how to implement without or player death
    player->setXY(-1, -1);
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
        for (int i = 0; i < _board->getNumEnemies(); i++) {
            std::shared_ptr<EnemyPawnModel> enemy = _board->getEnemy(i);
            if (enemy->getX() != -1) {
                // Store enemy location
                Rect oldBounds = _board->calculateDrawBounds(enemy->getX(), enemy->getY());
                
                // Move enemy
                enemy->move(_board->getWidth(), _board->getHeight());
                
                // Create animation
                Rect newBounds = _board->calculateDrawBounds(enemy->getX(), enemy->getY());
                Vec2 movement = newBounds.origin - oldBounds.origin;
                int tiles = _board->lengthToCells(movement.length());
                std::stringstream key;
                key << "int_enemy_move_" << i;
                std::shared_ptr<MoveBy> moveAction = MoveBy::alloc(movement, ((float)tiles)/ENEMY_IMG_SPEED);
    //            std::shared_ptr<MoveTo> moveAction = MoveTo::alloc(bounds.origin, 1.0f/ENEMY_IMG_SPEED);
                _actions->activate(key.str(), moveAction, enemy->getSprite());
                _interruptingActions.insert(key.str());
            }
        }
        
        // Update z positions
        _board->updateNodes(false);
        
        _state = State::ATTACK;
    } else if (_state == State::ATTACK) {
        // ATTACK
        for (int i = 0; i < _board->getNumEnemies(); i++) {
            std::shared_ptr<EnemyPawnModel> enemy = _board->getEnemy(i);
            if (enemy->getX() != -1) {
                // Attack
                for (int j = 0; j < _board->getNumAllies(); j++) {
                    std::shared_ptr<PlayerPawnModel> ally = _board->getAlly(j);
                    if (playerDistance(enemy, ally) < 1) {
                        _board->removeAlly(j);
                    }
                }
            }
        }
        _state = State::CHECK;
    } else {
        // CHECK
        setComplete(true);
        
        lose = true;
        for (int i = 0; i < _board->getNumAllies(); i++) {
            std::shared_ptr<PlayerPawnModel> temp = _board->getAlly(i);
            if (temp->getX() != -1) {
                lose = false;
            }
        }
        
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
