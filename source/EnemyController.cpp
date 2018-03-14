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
bool EnemyController::init(const std::shared_ptr<BoardModel>& board) {
    _board = board;
    
    _debug = false;
    _complete = false;
    
	//this returns a list of all enemies in the level
	//_board-> _enemies;

	//this returns the lis of allies
	//_board-> _allies;


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

int EnemyController::playerDistance(PlayerPawnModel enemy, PlayerPawnModel player) {
	return (abs(enemy.x - player.x) + abs(enemy.y - player.y));
}

void EnemyController::enemyMove(PlayerPawnModel enemy, int enemyIdx) {
//    int dx = 0;
//    int dy = 0;
//    //dx = (rand() % 3)-1; //choose a random direction between -1, 0 and 1
//    //int dy = (1 - std::max(abs(dx),0)) - (rand() % 3) - 1;
//
//    while (dx == dy || (dx != 0 && dy != 0)) {
//        dx = (rand() % 3) - 1;
//        dy = (rand() % 3) - 1;
//    }

    // Move in direction facing
    int x = enemy.x;
    int y = enemy.y;
    x += enemy.dx;
    y += enemy.dy;
    
    // Check bounds
    if (x < 0 || _board->getWidth() <= x) {
        _board->getEnemyPtr(enemyIdx)->turnAround();
    } else if (y < 0 || _board->getHeight() <= y) {
        _board->getEnemyPtr(enemyIdx)->turnAround();
    } else {
		bool enemyInWay = false;
		for (int i = 0; i < _board->getNumEnemies(); i++) {
			if (i != enemyIdx) {
				PlayerPawnModel temp = _board->getEnemy(i);
				if (temp.x == x && temp.y == y) {
					enemyInWay = true;
				}
			}
		}
		if (!enemyInWay) {
			_board->getEnemyPtr(enemyIdx)->step();
		}
    }
}

void EnemyController::enemyAttack(PlayerPawnModel enemy, PlayerPawnModel *player) {
	//unsure how to implement without or player death
    player->x = -1;
    player->y = -1;
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


	//Loop through every enemy and ally. Move the enemies 1 square randomly in any direction.
	for (int i = 0; i < _board->getNumEnemies(); i++) {
        PlayerPawnModel *enemy = _board->getEnemyPtr(i);
        enemyMove(*enemy, i);
		for (int j = 0; j < _board->getNumAllies(); j++) {
            PlayerPawnModel ally = _board->getAlly(j);
			if (playerDistance(ally, *enemy) < 1) {
				_board->removeAlly(j);
            }
            //this is assuming all enemies are "dumb"
		}
	}
    setComplete(true);

	lose = true;
	for (int i = 0; i < _board->getNumAllies(); i++) {
		PlayerPawnModel temp = _board->getAlly(i);
		if (temp.x != -1) {
			lose = false;
		}
	}
}



/**
 * Resets the status of the game so that we can play again.
 */
void EnemyController::reset() {
    _complete = false;
}
