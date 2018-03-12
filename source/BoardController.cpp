//
//  BoardController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

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
bool BoardController::init(const std::shared_ptr<BoardModel>& board) {
    _board = board;
    
    _debug = false;
    _complete = false;
    
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
//    CULog("BoardController Update");
	setComplete(!_board->checkForMatches());
}

/**
 * Resets the status of the game so that we can play again.
 */
void BoardController::reset() {
    _complete = false;
}
