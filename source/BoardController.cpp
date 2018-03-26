//
//  BoardController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include <sstream>
#include "BoardController.h"

using namespace cugl;


#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
BoardController::BoardController() {
}

/**
 * Initializes the controller contents, and starts the board turn
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param board    The game board
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool BoardController::init(std::shared_ptr<ActionManager>& actions, const std::shared_ptr<BoardModel>& board) {
    _actions = actions;
    _board = board;
    
    _state = State::CHECK;
    _debug = false;
    _complete = false;
	_animating = false;
	counter = 0;
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void BoardController::dispose() {
    _board = nullptr;
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * The method called to update the board turn.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void BoardController::update(float timestep) {
    if (_state == State::CHECK) {
        // Check for matches
        bool matchesExist = _board->checkForMatches();
        if (!matchesExist) {
            setComplete(true);
        } else {
            _state = State::REMOVE;
        }
    } else if (_state == State::REMOVE) {
        // Remove Tiles &
        // Tile animations
        int i = 0;
        std::set<std::shared_ptr<TileModel>>::iterator tileIter;
        for (tileIter = _board->getRemovedTiles().begin(); tileIter != _board->getRemovedTiles().end(); ++tileIter) {
            std::set<std::shared_ptr<TileModel>>::iterator it = _board->getAddedTiles().find(*tileIter);
            if (it != _board->getAddedTiles().end()) {
                // [it] was in both the added tiles and removed tiles
                _board->getAddedTiles().erase(*it);
            } else {
                std::stringstream key;
                key << "int_tile_remove_" << i;
                _actions->activate(key.str(), _board->tileRemoveAction, (*tileIter)->getSprite());
                _interruptingActions.insert(key.str());
                i++;
            }
        }
        
        // Enemy animations
        i = 0;
        std::set<std::shared_ptr<EnemyPawnModel>>::iterator enemyIter;
        for (enemyIter = _board->getRemovedEnemies().begin(); enemyIter != _board->getRemovedEnemies().end(); ++enemyIter) {
            std::stringstream key;
            key << "int_enemy_remove_" << i;
            _actions->activate(key.str(), _board->enemyRemoveAction, (*enemyIter)->getSprite());
            _interruptingActions.insert(key.str());
            i++;
        }
        
        // Check win condition
        win = true;
        for (int i = 0; i < _board->getNumEnemies(); i++) {
            std::shared_ptr<EnemyPawnModel> temp = _board->getEnemy(i);
            if (temp->getX() != -1) {
                win = false;
            }
        }
        
        _state = State::ADD;
    } else {
        // ADD
        // Remove removed tiles from board node
        std::set<std::shared_ptr<TileModel>>::iterator it;
        for (it = _board->getRemovedTiles().begin(); it != _board->getRemovedTiles().end(); ++it) {
            _board->getNode()->removeChild((*it)->getSprite());
        }
        _board->clearRemovedTiles();

        // Remove removed enemies from board node
        std::set<std::shared_ptr<EnemyPawnModel>>::iterator enemyIter;
        for (enemyIter = _board->getRemovedEnemies().begin(); enemyIter != _board->getRemovedEnemies().end(); ++enemyIter) {
            _board->getNode()->removeChild((*enemyIter)->getSprite());
        }
        _board->clearRemovedEnemies();
        
        // Add added tiles to board node and animate
        int i = 0;
        for (it = _board->getAddedTiles().begin(); it != _board->getAddedTiles().end(); ++it) {
            _board->getNode()->addChild((*it)->getSprite());
            (*it)->getSprite()->setFrame(TILE_IMG_APPEAR_START);
//            Color4 color = (*it)->getSprite()->getColor();
//            color.a = 0.0f;
//            (*it)->getSprite()->setColor(color);
            std::stringstream key;
            key << "int_tile_add_" << i;
            _actions->activate(key.str(), _board->tileAddAction, (*it)->getSprite());
            _interruptingActions.insert(key.str());
            i++;
        }
        
        // Clear set
        _board->clearAddedTiles();
        
        // Set complete
        _state = State::CHECK;
    }
    
    // Update board node positions
    _board->updateNodes();
}

/**
 * Resets the status of the game so that we can play again.
 */
void BoardController::reset() {
    _complete = false;
	_animating = false;
    _state = State::CHECK;
}
