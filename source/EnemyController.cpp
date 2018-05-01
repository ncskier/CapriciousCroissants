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
		// CALL FOR MOVEMENT UPDATE ON ENTITIES
        _entityManager->updateEntities(_board, EntityManager::movement);

        // Update z positions
//        _board->updateNodes(false);
        
        _state = State::ATTACK;
    } else if (_state == State::ATTACK) {
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
