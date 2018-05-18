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
    //_arrowSprite = assets;
    //deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_7);
    
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
    //CULog(">");
    
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
        std::set<size_t>::iterator enemyIter;
        
        for (enemyIter = _board->getAttackingEnemies().begin(); enemyIter != _board->getAttackingEnemies().end(); ++enemyIter) {
            if (_entityManager->hasComponent<RangeOrthoAttackComponent>(*enemyIter)) {
                LocationComponent loc = _entityManager->getComponent<LocationComponent>((*enemyIter));
                RangeOrthoAttackComponent ranged = _entityManager->getComponent<RangeOrthoAttackComponent>((*enemyIter));
                IdleComponent idle = _entityManager->getComponent<IdleComponent>((*enemyIter));
                
                int drawZ = _board->calculateDrawZ(loc.x, loc.y, false);
                
                if (ranged.target->getY() > loc.y) {
                    drawZ = _board->calculateDrawZ(loc.x, loc.y, true);
                }
                if (ranged.target->getY() < loc.y) {
                    drawZ = _board->calculateDrawZ(ranged.target->getX(), ranged.target->getY(), true);
                }
                
                ranged.projectile->setPosition(_board->gridToScreenV(loc.x, loc.y));
                _board->getNode()->addChild(ranged.projectile, drawZ);
                _board->getNode()->sortZOrder();
                
                cugl::Rect oldBounds = _board->calculateDrawBounds(loc.x, loc.y);
                cugl::Rect newBounds = _board->calculateDrawBounds(ranged.target->getX(), ranged.target->getY());
                cugl::Vec2 movement = newBounds.origin - oldBounds.origin;
                
                
                
                
                int tiles = _board->lengthToCells(movement.length());
                std::stringstream key;
                key << "int_enemy_shoot_" << *enemyIter;
                std::shared_ptr<cugl::MoveBy> moveAction = cugl::MoveBy::alloc(movement, ((float)tiles) / 10*idle.speed[0]);
                idle._actions->activate(key.str(), moveAction, ranged.projectile);
                idle._interruptingActions.insert(key.str());
                ranged.projectile->setScale(Vec2(0.25, 0.25));
            }
        }
        
        _state = State::FADE;
    } else if (_state == State::FADE) {
        // Fade out destroyed allies before removing them
        std::set<std::shared_ptr<PlayerPawnModel>>::iterator it;
        int i = 0;
        for (it = _board->getRemovedAllies().begin(); it != _board->getRemovedAllies().end(); ++it) {
            std::stringstream key;
            key << "int_ally_fade_out_" << i;
            if (!_actions->isActive(key.str())) {
                _actions->activate(key.str(), _board->allyFadeOutAction, (*it)->getEndSprite());
                _interruptingActions.insert(key.str());
            }
            i++;
        }
        _state = State::CHECK;
    } else {
        // CHECK
        std::set<size_t>::iterator enemyIter;
        for (enemyIter = _board->getAttackingEnemies().begin(); enemyIter != _board->getAttackingEnemies().end(); ++enemyIter) {
            if (_entityManager->hasComponent<RangeOrthoAttackComponent>(*enemyIter)) {
                //                LocationComponent loc = _entityManager->getComponent<LocationComponent>((*enemyIter));
                _board->getNode()->removeChild(_entityManager->getComponent<RangeOrthoAttackComponent>((*enemyIter)).projectile);
            }
        }
        
        _board->clearAttackingEnemies();
        
        std::set<std::shared_ptr<PlayerPawnModel>>::iterator it;
        for (it = _board->getRemovedAllies().begin(); it != _board->getRemovedAllies().end(); ++it) {
            _board->getNode()->removeChild((*it)->getSprite());
            _board->getNode()->removeChild((*it)->getEndSprite());
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
