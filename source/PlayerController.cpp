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
bool PlayerController::init(std::shared_ptr<ActionManager>& actions, const std::shared_ptr<BoardModel>& board, InputController *input) {
    _actions = actions;
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
 * Calculate real offset from input offset
 *
 * @return {row, offsetValue} where [row] is true if the row is offset and false if the column is offset
 */
std::tuple<bool, float> PlayerController::calculateOffset(Vec2 inputOffset) {
    if (abs(inputOffset.x) > abs(inputOffset.y)) {
        // Offset Row
        float offsetValue = (inputOffset.x > 0) ? inputOffset.x-abs(inputOffset.y) : inputOffset.x+abs(inputOffset.y);
        return {true, offsetValue};
    } else {
        // Offset Col
        float offsetValue = (inputOffset.y > 0) ? inputOffset.y-abs(inputOffset.x) : inputOffset.y+abs(inputOffset.x);
        return {false, offsetValue};
    }
}

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
            position = _board->getNode()->worldToNodeCoords(position);
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
            cugl::Vec2 inputOffset = _input->getMoveOffset();
//            float threshold = _board->getCellLength()/5.0f;
            bool row;
            float offsetValue;
            std::tie(row, offsetValue) = calculateOffset(inputOffset);
            _board->offsetReset();
            if (row) {
                // Offset Row
                _board->setOffsetRow(offsetValue);
            } else {
                // Offset Column
                _board->setOffsetCol(offsetValue);
            }
            
        } else {
            // END
            // Calculate movement
            Vec2 inputOffset = _input->getMoveOffset();
            bool row;
            float offsetValue;
            std::tie(row, offsetValue) = calculateOffset(inputOffset);
            int cells = _board->lengthToCells(offsetValue);
            // Check if valid move
            if (abs(cells) > 0) {
                // Update board
                _board->slide(cells);
                setComplete(true);
            }
            _board->deselectTile();
            _input->clear();
        }
	}
    
    // Update board node positions
    _board->updateNodes();
}

/**
 * Resets the status of the game so that we can play again.
 */
void PlayerController::reset() {
    _complete = false;
}
