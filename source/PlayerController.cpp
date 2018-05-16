//
//  PlayerController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "PlayerController.h"
#include "EntityManager.h"

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
bool PlayerController::init(std::shared_ptr<ActionManager>& actions, const std::shared_ptr<BoardModel>& board, std::shared_ptr<InputController>& input, std::shared_ptr<EntityManager>& manager) {
    _actions = actions;
    _board = board;
    _input = input;
	_entityManager = manager;
    
    _debug = false;
    _complete = false;
    _numberMoves = 0;
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void PlayerController::dispose() {
    CULog("dispose PlayerController");
    _input = nullptr;
    _board = nullptr;
    _actions = nullptr;
    _entityManager = nullptr;
    _complete = false;
    _debug = false;
    _interruptingActions.clear();
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
		bool hasRooting = false;
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
			_board->requestedRow = row;

			// Make sure no Rooting enemies
			int x = _board->xOfIndex(_board->getSelectedTile());
			int y = _board->yOfIndex(_board->getSelectedTile());

			if (_entityManager->updateEntities(_board, EntityManager::playerLimit) == 0) {
				_board->offsetReset();
				if (row) {
					// Check row
					for (int r = 0; r < _board->getWidth(); r++) {
						if (_entityManager->hasComponent<RootingComponent>(_board->getEnemy(r, y))) {
							hasRooting = true;
						}
					}
					if (!hasRooting) {
						// Offset Row
						_board->setOffsetRow(offsetValue);
					}
					else {
						_board->setOffsetRow(0);
					}
				}
				else {
					// Check column
					for (int c = 0; c < _board->getHeight(); c++) {
						if (_entityManager->hasComponent<RootingComponent>(_board->getEnemy(x, c))) {
							hasRooting = true;
						}
					}
					if (!hasRooting) {
						// Offset Column
						_board->setOffsetCol(offsetValue);
					}
					else {
						_board->setOffsetCol(0);
					}

					
				}
				bool onRootedX = false;
				bool onRootedY = false;
				std::shared_ptr<cugl::Texture> rootTexture = _board->getAssets()->get<Texture>("rooting");

				for (int xx = 0; xx < _board->getWidth(); xx++) {
					if (_entityManager->hasComponent<RootingComponent>(_board->getEnemy(xx, y))) { onRootedX = true; }
				}

				for (int yy = 0; yy < _board->getHeight(); yy++) {
					if (_entityManager->hasComponent<RootingComponent>(_board->getEnemy(x, yy))) { onRootedY = true; }
				}


				if (onRootedX && std::abs(inputOffset.x) > _board->getCellLength() / 2 && !drawX && _board->offsetRow) {
					for (int xx = 0; xx < _board->getWidth(); xx++) {
						CULog("herex");
						std::shared_ptr<cugl::AnimationNode> rootedSprite;
						rootedSprite = AnimationNode::alloc(rootTexture, 1, 2);
						rootedSprite->setFrame(1);
						Vec2 pos = Vec2(xx, y);
						rootedSprite->setPosition(_board->gridToScreenV(pos.x, pos.y));
						_board->getNode()->addChildWithName(rootedSprite, "selectedx", _board->calculateDrawZ(xx, y, true));
						drawX = true;
					}
				}

				
				if (onRootedY && std::abs(inputOffset.y) > _board->getCellLength() / 2 && !drawY && _board->offsetCol) {
					for (int yy = 0; yy < _board->getHeight(); yy++) {
						std::shared_ptr<cugl::AnimationNode> rootedSprite;
						rootedSprite = AnimationNode::alloc(rootTexture, 1, 2);
						rootedSprite->setFrame(0);
						Vec2 pos = Vec2(x, yy);
						rootedSprite->setPosition(_board->gridToScreenV(pos.x, pos.y));
						_board->getNode()->addChildWithName(rootedSprite, "selectedy", _board->calculateDrawZ(x, yy, true));
						drawY = true;
					}
				}

                if ((drawX && std::abs(inputOffset.x) < _board->getCellLength() / 2) || _board->offsetCol) {
					for (int xx = 0; xx < _board->getWidth(); xx++) {
						_board->getNode()->removeChildByName("selectedx");
					}
					drawX = false;
				}

                if ((drawY && std::abs(inputOffset.y) < _board->getCellLength() / 2) || _board->offsetRow) {
					for (int yy = 0; yy < _board->getWidth(); yy++) {
						_board->getNode()->removeChildByName("selectedy");
					}
					drawY = false;
				}

			}
        } else {
            // END
            // Calculate movement
            Vec2 inputOffset = _input->getMoveOffset();
            bool row;
            float offsetValue;
            std::tie(row, offsetValue) = calculateOffset(inputOffset);
			int cells = 0;
			_board->requestedRow = row;
			if (_entityManager->updateEntities(_board, EntityManager::playerLimit) == 0) {
				cells = _board->lengthToCells(offsetValue, _board->offsetRow);
			}
			
			// Make sure no Rooting enemies
			int x = _board->xOfIndex(_board->getSelectedTile());
			int y = _board->yOfIndex(_board->getSelectedTile());

			if (_entityManager->updateEntities(_board, EntityManager::playerLimit) == 0) {
				if (row) {
					// Check row
					for (int r = 0; r < _board->getWidth(); r++) {
						if (_entityManager->hasComponent<RootingComponent>(_board->getEnemy(r, y))) {
							hasRooting = true;
						}
					}
				}
				else {
					// Check column
					for (int c = 0; c < _board->getHeight(); c++) {
						if (_entityManager->hasComponent<RootingComponent>(_board->getEnemy(x, c))) {
							hasRooting = true;
						}
					}
				}
			}

            // Check if valid move
            if (abs(cells) > 0 && !hasRooting) {
                // Check for wrap around stagnant moves (moving to the same position)
                if (!(abs(cells) == _board->getWidth() && _board->offsetRow) && !(abs(cells) == _board->getHeight() && _board->offsetCol)) {
                    // Update board
                    _board->slide(cells);
                    _numberMoves++;
                    setComplete(true);
                }
            }
            _board->deselectTile();
            _input->clear();

			drawX = false;
			drawY = false;
			
			for (int xx = 0; xx < _board->getWidth(); xx++) {
				_board->getNode()->removeChildByName("selectedx");
			}
			for (int yy = 0; yy < _board->getHeight(); yy++) {
				_board->getNode()->removeChildByName("selectedy");
			}


        }
	}
    
    // Update board node positions
    _board->updateNodes();

	 //Update board for passing to facing
	_entityManager->updateEntities(_board, EntityManager::onPlayerMove);
}

/**
 * Resets the status of the game so that we can play again.
 */
void PlayerController::reset() {
    _complete = false;
}
