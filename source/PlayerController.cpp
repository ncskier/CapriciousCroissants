//
//  PlayerController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "PlayerController.h"

using namespace cugl;


#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
PlayerController::PlayerController() :
_debug(false),
_complete(false){
}

/**
 * Initializes the controller contents, and starts the player turn
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param board    The game board
 * @param input    The input controller
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool PlayerController::init(const std::shared_ptr<BoardModel>& board, InputController *input) {
    _board = board;
    _input = input;
    
    _debug = false;
    _complete = false;
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void PlayerController::dispose() {
    _board = nullptr;
    _input = nullptr;
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * The method called to update the player turn.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void PlayerController::update(float timestep) {
//    CULog("PlayerController Update");
    InputController::MoveEvent moveEvent = _input->getMoveEvent();
    if (moveEvent != InputController::MoveEvent::NONE) {
        if (moveEvent == InputController::MoveEvent::START) {
            // START
            Vec2 position = _input->getTouchPosition();
            CULog("position: %s", position.toString().c_str());
//            position *= inputLimiter;
            // Check if On Tile
            if (_board->selectTileAtPosition(position)) {
                // Valid move start
                _input->recordMove();
            } else {
                // Invalid move start
                _input->clear();
            }
        } else if (moveEvent == InputController::MoveEvent::MOVING) {
            // MOVING
            cugl::Vec2 offset = _input->getMoveOffset();
//            offset *= inputLimiter;
            float threshold = 20.0f;
            // Reset offset if below threshold
            if (_board->offsetRow && abs(offset.x) < threshold/2.0f) {
                _board->offsetReset();
            }
            if (_board->offsetCol && abs(offset.y) < threshold/2.0f) {
                _board->offsetReset();
            }
            // New offset
            if (!_board->offsetCol && abs(offset.x) >= threshold) {
                _board->setOffsetRow(offset.x);
            }
            if (!_board->offsetRow && abs(offset.y) >= threshold) {
                _board->setOffsetCol(offset.y);
            }
        } else {
//            // END
//            // Calculate movement
//            // Update board
//            // Set to board board
//            // Check if valid move
            _input->clear();
//            setComplete(true);
        }
    }
}

/**
 * Resets the status of the game so that we can play again.
 */
void PlayerController::reset() {
    _complete = false;
}
