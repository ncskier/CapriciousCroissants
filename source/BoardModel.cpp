//
//  BoardModel.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <set>

#include "BoardModel.h"
#include <cugl/base/CUBase.h>

using namespace cugl;

// Initialize board attributes and call generateNewBoard to generate _tiles, _allies, and _enemies
BoardModel::BoardModel() :
_height(5),
_width(5),
_numColors(5),
_numAllies(1),
_numEnemies(4),
_selectedTile(-1),
_placeAllies(false),
_boardPadding(25.0f),
_tilePadding(10.0f),
offsetRow(false),
offsetCol(false),
offset(0.0f) {
}

/**
 * Initializes the board
 *
 * @param width     The board width (num tiles)
 * @param height    The board height (num tiles)
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool BoardModel::init(int width, int height) {
    return init(width, height, _numColors, _numAllies, _numEnemies, _placeAllies);
}

bool BoardModel::init(int width, int height, int colors, int allies, int enemies, bool placePawn) {
    _height = height;
    _width = width;
    _numColors = colors;
    _numAllies = allies;
    _numEnemies = enemies;
    _placeAllies = placePawn;
    colorLookup = { Color4::CORNFLOWER, Color4::RED, Color4::CYAN, Color4::MAGENTA, Color4::BLUE, Color4::GREEN, Color4::YELLOW, Color4::WHITE };
    
    // Reserve space for all tiles
//    _tiles.reserve(_width * _height);
    
    srand((int)time(NULL));
    generateNewBoard();
    while (checkForMatches());
    srand((int)time(NULL));
    return true;
}

bool BoardModel::init(int width, int height, int colors, int allies, int enemies, bool placePawn, std::shared_ptr<cugl::AssetManager>& assets, Size dimen) {
    _assets = assets;
    
    // Setup Board Node
    _node = Node::alloc();
    _node->setContentSize(dimen);
    _node->setAnchor(Vec2::ZERO);
    CULog("dimen: %s", dimen.toString().c_str());
    gameWidth = dimen.width;
    gameHeight = dimen.height;
    return init(width, height, colors, allies, enemies, placePawn);
}

// Destroy any values needed to be deleted for this class
void BoardModel::dispose() {
}

/**
 * Convert (x, y) coordinate to array index. (0, 0) is the upper left corner
 * Example for sideSize = 3
 *  0 1 2     (0,0) (1,0) (2,0)
 *  3 4 5     (0,1) (1,1) (2,1)
 *  6 7 8     (0,2) (1,2) (2,2) (3) (4)
 */
int BoardModel::indexOfCoordinate(int x, int y) const {
	return x + (y*_width);
}

// Convert array index to x
int BoardModel::xOfIndex(int i) const {
    return i % _width;
}

// Convert array index to y
int BoardModel::yOfIndex(int i) const {
    return i / _width;
}

// Returns the value at the give (x, y) coordinate
std::shared_ptr<TileModel>& BoardModel::getTile(int x, int y) {
	return _tiles[indexOfCoordinate(x, y)];
}

// Returns the ally pawn at index i of _allies
std::shared_ptr<PlayerPawnModel> BoardModel::getAlly(int i) {
	return _allies[i];
}

// Returns the ally pawn at (x, y)
std::shared_ptr<PlayerPawnModel> BoardModel::getAlly(int x, int y) {
    for (int i = 0; i < _numAllies; i++) {
        std::shared_ptr<PlayerPawnModel>& ally = _allies[i];
        if (ally->getX() == x && ally->getY() == y) {
            return ally;
        }
    }
    return nullptr;
}

// Returns the enemy pawn at index i of _enemies
std::shared_ptr<EnemyPawnModel> BoardModel::getEnemy(int i) {
	return _enemies[i];
}

// Returns the enemy pawn at (x, y)
std::shared_ptr<EnemyPawnModel> BoardModel::getEnemy(int x, int y) {
    for (int i = 0; i < _numEnemies; i++) {
        std::shared_ptr<EnemyPawnModel>& enemy = _enemies[i];
        if (enemy->getX() == x && enemy->getY() == y) {
            return enemy;
        }
    }
    return nullptr;
}

// Returns the allies
std::vector<std::shared_ptr<PlayerPawnModel>>& BoardModel::getAllies() {
	return _allies;
}

// Returns the enemies
std::vector<std::shared_ptr<EnemyPawnModel>>& BoardModel::getEnemies() {
	return _enemies;
}


// Set the value at the given (x, y) coordinate
void BoardModel::setTile(int x, int y, std::shared_ptr<TileModel> t) {
	_tiles[indexOfCoordinate(x, y)] = t;
//    _tiles[indexOfCoordinate(x, y)]->setSpriteBounds(calculateDrawBounds(x, y));
}

// Place ally at index i of _allies on location (x, y)
void BoardModel::placeAlly(int x, int y, int i) {
	_allies[i]->setXY(x, y);
}
	
// Place enemy at index i of _enemies on location (x, y)
void BoardModel::placeEnemy(int x, int y, int i) {
	_enemies[i]->setXY(x, y);
    _enemies[i]->getSprite()->setPosition(calculateDrawBounds(x, y).origin);
}

void BoardModel::moveEnemy(int dx, int dy, int enemyIdx) {
    _enemies[enemyIdx]->step();
}

// Remove ally at index i
void BoardModel::removeAlly(int i) {
    _node->removeChild(_allies[i]->getSprite());
	_allies[i]->setXY(-1, -1);
}

// Remove enemy at index i
void BoardModel::removeEnemy(int i) {
    _node->removeChild(_enemies[i]->getSprite());
	_enemies[i]->setXY(-1, -1);
}

// Check if any matches exist on the board, if so then remove them and check for pawn locations for damage/removal
bool BoardModel::checkForMatches() {
	std::set<int> replaceTiles;

	// Check for matches
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			// Check Row
			if (x >= 2) {
				if (_tiles[indexOfCoordinate(x,y)]->getColor() == _tiles[indexOfCoordinate(x - 1, y)]->getColor()
					&& _tiles[indexOfCoordinate(x - 1, y)]->getColor() == _tiles[indexOfCoordinate(x - 2, y)]->getColor()) {
					replaceTiles.insert(indexOfCoordinate(x, y));
					replaceTiles.insert(indexOfCoordinate(x - 1, y));
					replaceTiles.insert(indexOfCoordinate(x - 2, y));
				}
			}
			// Check Column
			if (y >= 2) {
				if (_tiles[indexOfCoordinate(x, y)]->getColor() == _tiles[indexOfCoordinate(x, y - 1)]->getColor()
					&& _tiles[indexOfCoordinate(x, y - 1)]->getColor() == _tiles[indexOfCoordinate(x, y - 2)]->getColor()) {
					replaceTiles.insert(indexOfCoordinate(x, y));
					replaceTiles.insert(indexOfCoordinate(x, y - 1));
					replaceTiles.insert(indexOfCoordinate(x, y - 2));
				}
			}
		}
	}

	bool matchExists = !replaceTiles.empty();

	// Replace Tiles
	std::set<int>::iterator iter;
	for (iter = replaceTiles.begin(); iter != replaceTiles.end(); ++iter) {
		// Replace tile
		replaceTile(*iter);
		// Remove enemies
        if (!_enemies.empty()) {
            for (int i = 0; i < _enemies.size(); i++) {
                if (indexOfCoordinate(_enemies[i]->getX(), _enemies[i]->getY()) == *iter) {
                    removeEnemy(i);
                }
            }
        }
	}

	return matchExists;
}


// TEMPORARY FUNCTIONS FOR COOL ANIMATION
// Check if any matches exist on the board, if so then remove them and check for pawn locations for damage/removal
bool BoardModel::checkForMatchesTemp() {

	// Check for matches
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			// Check Row
			if (x >= 2) {
				if (_tiles[indexOfCoordinate(x, y)]->getColor() == _tiles[indexOfCoordinate(x - 1, y)]->getColor()
					&& _tiles[indexOfCoordinate(x - 1, y)]->getColor() == _tiles[indexOfCoordinate(x - 2, y)]->getColor()) {
					totalReplaceTiles.insert(indexOfCoordinate(x, y));
					totalReplaceTiles.insert(indexOfCoordinate(x - 1, y));
					totalReplaceTiles.insert(indexOfCoordinate(x - 2, y));
				}
			}
			// Check Column
			if (y >= 2) {
				if (_tiles[indexOfCoordinate(x, y)]->getColor() == _tiles[indexOfCoordinate(x, y - 1)]->getColor()
					&& _tiles[indexOfCoordinate(x, y - 1)]->getColor() == _tiles[indexOfCoordinate(x, y - 2)]->getColor()) {
					totalReplaceTiles.insert(indexOfCoordinate(x, y));
					totalReplaceTiles.insert(indexOfCoordinate(x, y - 1));
					totalReplaceTiles.insert(indexOfCoordinate(x, y - 2));
				}
			}
		}
	}

	bool matchExists = !totalReplaceTiles.empty();
	if (matchExists) {
		animationCounter = 0;
	}

	return matchExists;
}

void BoardModel::triggerResets() {
	// Replace Tiles
	for (totalIter = totalReplaceTiles.begin(); totalIter != totalReplaceTiles.end(); totalIter++) {
		// Replace tile
		replaceTile(*totalIter);
		// Remove enemies
		if (!_enemies.empty()) {
			for (int i = 0; i < _numEnemies; i++) {
				if (indexOfCoordinate(_enemies[i]->getX(), _enemies[i]->getY()) == *totalIter) {
					removeEnemy(i);
				}
			}
		}
	}

	animationCounter = 60;

	
}



// Private function that allows for a tile to be replaced based on it's array index value in _tiles
void BoardModel::replaceTile(int tileLocation) {
    _node->removeChild(_tiles[tileLocation]->getSprite());
    // New random color
    int color = rand() % _numColors;
    Rect bounds = calculateDrawBounds(xOfIndex(tileLocation), yOfIndex(tileLocation));
    std::shared_ptr<TileModel> tile = TileModel::alloc(color, bounds, _assets);
    _node->addChild(tile->getSprite());
	_tiles[tileLocation] = tile;
}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void BoardModel::generateNewBoard() {
	// Setup Tiles
    _tiles.reserve(_width * _height);
    srand((int)time(NULL));
	int color;
    for (int i = 0; i < _height*_width; i++) {
        color = rand() % _numColors;        // random number in range [0, _numColors-1]
        Rect bounds = calculateDrawBounds(xOfIndex(i), yOfIndex(i));
        std::shared_ptr<TileModel> tile = TileModel::alloc(color, bounds, _assets);
        CULog("anchor: %s", tile->getSprite()->getAnchor().toString().c_str());
        CULog("origin: %s", tile->getSprite()->getPosition().toString().c_str());
        _node->addChild(tile->getSprite());
        _tiles.push_back(tile);
    }
    
    // Replace any matches
    while (checkForMatches());

    // Setup Allies
    int x;
    int y;
    for (int i = 0; i < _numAllies; i++) {
        x = rand() % _width;
        y = rand() % _height;
        while (true) {
            bool match = false;
            for (int j = 0; j < i; j++) {
                std::shared_ptr<PlayerPawnModel> temp = _allies[j];
                if (temp->getX() == x && temp->getY() == y) {
                    x = rand() % _width;
                    y = rand() % _height;
                    match = true;
                }
            }
            if (!match) {
                break;
            }
        }
        std::shared_ptr<PlayerPawnModel> ally = PlayerPawnModel::alloc(x, y, calculateDrawBounds(x, y), _assets);
        _node->addChild(ally->getSprite());
        _allies.push_back(ally);
    }

    //Setup Enemies
    for (int i = 0; i < _numEnemies; i++) {
        x = rand() % _width;
        y = rand() % _height;
        while (true) {
            bool match = false;
            for (int j = 0; j < _numAllies; j++) {
                std::shared_ptr<PlayerPawnModel> temp = _allies[j];
                if (temp->getX() == x && temp->getY() == y) {
                    x = rand() % _width;
                    y = rand() % _height;
                    match = true;
                }
            }
            if (i > 0) {
                for (int j = 0; j < i; j++) {
                    std::shared_ptr<EnemyPawnModel> temp = _enemies[j];
                    if (temp->getX() == x && temp->getY() == y) {
                        x = rand() % _width;
                        y = rand() % _height;
                        match = true;
                    }
                }
            }

            if (!match) {
                break;
            }
        }
        std::shared_ptr<EnemyPawnModel> enemy = EnemyPawnModel::alloc(x, y, calculateDrawBounds(x, y), _assets);
        enemy->setRandomDirection();
        _node->addChild(enemy->getSprite());
        _enemies.push_back(enemy);
    }
}

// Slide pawns in row or column [k] by [offset]
void BoardModel::slidePawns(bool row, int k, int offset) {
    // Slide Allies
    for (int i = 0; i < _numAllies; i++) {
        std::shared_ptr<PlayerPawnModel> pawn = _allies[i];
        if (pawn->getX() != -1 && pawn->getY() != -1) {
            if (row) {
                // Row
                if (k == pawn->getY()) {
                    float x = (pawn->getX() + offset) % _width;
                    while (x < 0) {
                        x += _width;
                    }
                    _allies[i]->setX(x);
//                    _allies[i]->setSpriteBounds(calculateDrawBounds(x, k));
                }
            }
            else {
                // Column
                if (k == pawn->getX()) {
                    float y = (pawn->getY() + offset) % _height;
                    while (y < 0) {
                        y += _height;
                    }
                    _allies[i]->setY(y);
//                    _allies[i]->setSpriteBounds(calculateDrawBounds(k, y));
                }
            }
        }
    }

    // Slide Enemies
    for (int i = 0; i < _numEnemies; i++) {
        std::shared_ptr<EnemyPawnModel> pawn = _enemies[i];
        if (pawn->getX() != -1 && pawn->getY() != -1) {
            if (row) {
                // Row
                if (k == pawn->getY()) {
                    float x = ((int)pawn->getX() + offset) % _width;
                    while (x < 0) {
                        x += _width;
                    }
                    _enemies[i]->setX(x);
//                    _enemies[i]->setSpriteBounds(calculateDrawBounds(x, k));
                }
            }
            else {
                // Column
                if (k == pawn->getX()) {
                    float y = (pawn->getY() + offset) % _height;
                    while (y < 0) {
                        y += _height;
                    }
                    _enemies[i]->setY(y);
//                    _enemies[i]->setSpriteBounds(calculateDrawBounds(k, y));
                }
            }
        }
    }
}

//Slide row or column by [offset]
void BoardModel::slide(bool row, int k, int offset) {
    CULog("SLIDE");
	// Copy
	int sideSize = row ? _width : _height;
    std::shared_ptr<TileModel>* line = new std::shared_ptr<TileModel>[sideSize];
	for (int i = 0; i < sideSize; i++) {
		int x = row ? i : k;
		int y = row ? k : i;
		line[i] = getTile(x, y);
	}

	// Slide/write row
	for (int i = 0; i < sideSize; i++) {
		int x = row ? i : k;
		int y = row ? k : i;
		int j = (i - offset) % sideSize;
		while (j < 0) {
			j += sideSize;
		}
		setTile(x, y, line[j]);
	}

	// Slide pawns
    slidePawns(row, k, offset);
}

//Offset view of row (not model)
void BoardModel::setOffsetRow(float value) {
	offsetRow = true;
	offset = value;
//    int y = yOfIndex(_selectedTile);
//    for (int x = 0; x < _width; x++) {
//        std::shared_ptr<TileModel>& tile = _tiles[indexOfCoordinate(x, y)];
//        Rect bounds = calculateDrawBounds(x, y);
//        bounds.origin += Vec2(value, 0);
//        tile->setSpriteBounds(bounds);
//    }
//    for (unsigned i = 0; i < _allies.size(); i++) {
//        std::shared_ptr<PlayerPawnModel>& ally = _allies[i];
//        if (ally->getY() == y) {
//            int x = ally->getX();
//            Rect bounds = calculateDrawBounds(x, y);
//            bounds.origin += Vec2(value, 0);
//            ally->setSpriteBounds(bounds);
//        }
//    }
//    for (unsigned i = 0; i < _enemies.size(); i++) {
//        std::shared_ptr<EnemyPawnModel>& enemy = _enemies[i];
//        if (enemy->getY() == y) {
//            int x = enemy->getX();
//            Rect bounds = calculateDrawBounds(x, y);
//            bounds.origin += Vec2(value, 0);
//            enemy->setSpriteBounds(bounds);
//        }
//    }
}

//Offset view of col (not model)
void BoardModel::setOffsetCol(float value) {
	offsetCol = true;
	offset = value;
//    int x = xOfIndex(_selectedTile);
//    for (int y = 0; y < _height; y++) {
//        std::shared_ptr<TileModel>& tile = _tiles[indexOfCoordinate(x, y)];
//        Rect bounds = calculateDrawBounds(x, y);
//        bounds.origin += Vec2(0, value);
//        tile->setSpriteBounds(bounds);
//    }
//    for (unsigned i = 0; i < _allies.size(); i++) {
//        std::shared_ptr<PlayerPawnModel>& ally = _allies[i];
//        if (ally->getX() == x) {
//            int y = ally->getY();
//            Rect bounds = calculateDrawBounds(x, y);
//            bounds.origin += Vec2(0, value);
//            ally->setSpriteBounds(bounds);
//        }
//    }
//    for (unsigned i = 0; i < _enemies.size(); i++) {
//        std::shared_ptr<EnemyPawnModel>& enemy = _enemies[i];
//        if (enemy->getX() == x) {
//            int y = enemy->getY();
//            Rect bounds = calculateDrawBounds(x, y);
//            bounds.origin += Vec2(0, value);
//            enemy->setSpriteBounds(bounds);
//        }
//    }
}

//Offset reset
void BoardModel::offsetReset() {
	offsetRow = false;
	offsetCol = false;
	offset = 0.0f;
}

// Deselect _selectedTile
void BoardModel::deselectTile() {
    _selectedTile = -1;
    offsetReset();
}

//Slide row [y] by [offset]
void BoardModel::slideRow(int y, int offset) {
	slide(true, y, offset);
}

//Slide column [x] by [offset]
void BoardModel::slideCol(int x, int offset) {
	slide(false, x, offset);
}

// Slide row/col by [offset]
void BoardModel::slide(int offset) {
    if (offsetRow) {
        slideRow(yOfIndex(_selectedTile), offset);
    } else {
        slideCol(xOfIndex(_selectedTile), offset);
    }
}

/**
 * Select tile at screen position [position]
 *
 * @param position on screen
 *
 * @return true if there is a tile, false otherwise
 */
bool BoardModel::selectTileAtPosition(Vec2 position) {
    int x;
    int y;
    std::tie(x, y) = screenToGrid(position);
    if (x < 0 || _width <= x) {
        return false;
    }
    if (y < 0 || _height <= y) {
        return false;
    }
    _selectedTile = indexOfCoordinate(x, y);
    return true;
}


#pragma mark -
#pragma mark Drawing/Animation

/** Update nodes */
void BoardModel::updateNodes() {
    // Tiles
    for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
            _tiles[indexOfCoordinate(x, y)]->setSpriteBounds(calculateDrawBounds(x, y));
            _tiles[indexOfCoordinate(x, y)]->getSprite()->setZOrder(calculateDrawZ(x, y, true));
        }
    }
    
    // Allies
    for (std::vector<std::shared_ptr<PlayerPawnModel>>::iterator it = _allies.begin(); it != _allies.end(); ++it) {
        (*it)->setSpriteBounds(calculateDrawBounds((*it)->getX(), (*it)->getY()));
        (*it)->getSprite()->setZOrder(calculateDrawZ((*it)->getX(), (*it)->getY(), false));
    }
    
    // Enemies
    for (std::vector<std::shared_ptr<EnemyPawnModel>>::iterator it = _enemies.begin(); it != _enemies.end(); ++it) {
        (*it)->setSpriteBounds(calculateDrawBounds((*it)->getX(), (*it)->getY()));
        (*it)->getSprite()->setZOrder(calculateDrawZ((*it)->getX(), (*it)->getY(), false));
    }
    
    // Resort z order
    _node->sortZOrder();
}

float BoardModel::getCellLength() {
    float cellWidth = (gameWidth - 2.0f*_boardPadding) / _width;
    float cellHeight = (gameHeight - 2.0f*_boardPadding) / _height;
    return fmin(cellWidth, cellHeight);
}

// Convert grid (x, y) to screen coordinates
cugl::Rect BoardModel::gridToScreen(int x, int y) {
    float cellLength = getCellLength();
    
    float xPos = _boardPadding + x*cellLength;
    float yPos = _boardPadding + y*cellLength;
    
    return Rect(xPos, yPos, cellLength, cellLength);
}

// Convert screen coordinates to grid (x, y)
std::tuple<int, int> BoardModel::screenToGrid(Vec2 position) {
    float cellLength = getCellLength();

    int x = (int)floor( (position.x - _boardPadding/2.0f) / cellLength );
    int y = (int)floor( (position.y - _boardPadding/2.0f) / cellLength );
    
    return {x, y};
}

// Convert screen length to grid length
int BoardModel::lengthToCells(float length) {
    float cellLength = getCellLength();
    return (int)round( length / cellLength );
}

// Apply padding, offset, and wrap to return tile bounds
Rect BoardModel::calculateDrawBounds(int gridX, int gridY) {
    Rect bounds = gridToScreen(gridX, gridY);
    
    // Apply Padding to Bounds
    float x = bounds.getMinX() + _tilePadding/2.0f;
    float y = bounds.getMinY() + _tilePadding/2.0f;
    float width = bounds.size.width - _tilePadding;
    float height = bounds.size.height - _tilePadding;
    bounds.set(x, y, width, height);
    
    // Calculate Offset
    float xOffset = (offsetRow && gridY == yOfIndex(_selectedTile)) ? offset : 0.0f;
    float yOffset = (offsetCol && gridX == xOfIndex(_selectedTile)) ? offset : 0.0f;
    
    // Calculate Wrap (wrapping tiles around the board as they are moved)
    float xWrap = 0.0f;
    float yWrap = 0.0f;
    float cellLength = getCellLength();
    float boardWidth = _width*cellLength;
    float boardHeight = _height*cellLength;
    if (bounds.getMidX() + xOffset <= _boardPadding)
        xWrap = boardWidth;
    if (bounds.getMidX() + xOffset >= boardWidth + _boardPadding)
        xWrap = -boardWidth;
    if (bounds.getMidY() + yOffset <= _boardPadding)
        yWrap = boardHeight;
    if (bounds.getMidY() + yOffset >= boardHeight + _boardPadding)
        yWrap = -boardHeight;
    
    // Calculate new bounds
    bounds.set(
               x + xOffset + xWrap,
               y + yOffset + yWrap,
               width,
               height);
    return bounds;
}

/**
 * Calculate z-axis coordinate given (x,y) cell in grid.
 *   Tiles at 10s
 *   Pawns at  Tile number + 5
 */
int BoardModel::calculateDrawZ(int x, int y, bool tile) {
    int row = y;
    if (offsetCol && _selectedTile != -1 && x == xOfIndex(_selectedTile)) {
        row = (row + lengthToCells(offset)) % _height;
        while (row < 0) {
            row += _height;
        }
    }
    int base = (_height-row) * 10;
    return tile ? base : base+5;
}

///**
// * Draws tile given tile bounds
// * Batch has already begun
// */
//void BoardModel::drawTile(const std::shared_ptr<cugl::SpriteBatch>& batch, cugl::Rect tileBounds, TileModel tile) {
//    if (tile.getColor() == 0) {
//        batch->draw(tile1Texture, tileBounds);
//    } else if (tile.getColor() == 1) {
//        batch->draw(tile2Texture, tileBounds);
//    } else {
//        batch->draw(tileTexture, colorLookup.at(tile.getColor()), tileBounds);
//    }
//}
//
///**
// * Draws ally given tile bounds
// * Batch has already begun
// */
//void BoardModel::drawAlly(const std::shared_ptr<cugl::SpriteBatch>& batch, Rect tileBounds) {
//    float width = playerTexture->getSize().width / playerTexture->getSize().height * tileBounds.size.height;
//    float height = tileBounds.size.height;
//    float x = tileBounds.getMinX() + (tileBounds.size.width-width)/2.0f;
//    float y = tileBounds.getMinY() + tileBounds.size.height*0.3f;
//    Rect bounds = Rect(x, y, width, height);
//    batch->draw(playerTexture, bounds);
//}
//
///**
// * Draws enemy given tile bounds
// * Batch has already begun
// */
//void BoardModel::drawEnemy(const std::shared_ptr<cugl::SpriteBatch>& batch, cugl::Rect tileBounds, EnemyPawnModel enemy) {
//    float width = tileBounds.size.width * 0.5f;
//    float height = tileBounds.size.height * 0.5f;
//    float x = tileBounds.getMinX() + (tileBounds.size.width-width)/2.0f;
//    float y = tileBounds.getMinY() + (tileBounds.size.height-height)/2.0f;
//    Rect bounds = Rect(x, y, width, height);
//    batch->draw(tileTexture, Color4::BLACK, bounds);
//
//    // Direction Indicator
//    float padding = _tilePadding * 0.5f;
//    width = bounds.size.width * 0.25f;
//    height = bounds.size.height * 0.25f;
//    x = bounds.getMidX() - width/2.0f;
//    y = bounds.getMidY() - height/2.0f;
//    if (enemy.getDirection() == EnemyPawnModel::Direction::EAST)
//        x = bounds.getMaxX() - width - padding;
//    if (enemy.getDirection() == EnemyPawnModel::Direction::WEST)
//        x = bounds.getMinX() + padding;
//    if (enemy.getDirection() == EnemyPawnModel::Direction::NORTH)
//        y = bounds.getMaxY() - height - padding;
//    if (enemy.getDirection() == EnemyPawnModel::Direction::SOUTH)
//        y = bounds.getMinY() + padding;
//    bounds.set(x, y, width, height);
//    batch->draw(tileTexture, Color4::RED, bounds);
//}
//
//// Draws all of the tiles and pawns(in that order)
//void BoardModel::draw(const std::shared_ptr<SpriteBatch>& batch) {
//    Rect bounds;
//    batch->begin();
//
////    if (animationCounter >= 0) {
////        animationCounter++;
////    }
////    if (animationCounter == 60) {
////        triggerResets();
////    }
////    if (animationCounter > 120) {
////        totalReplaceTiles.clear();
////        animationCounter = -1;
////    }
////    for (int x = 0; x < _width; x++) {
////        for (int y = 0; y < _height; y++) {  //USE FUNCTIONS, 90% OF THIS CODE IS REPEATED 4x, EDITING IT IS A NIGHTMARE
////            // Offset
////            float xOffset = (offsetRow && y == yOfIndex(_selectedTile)) ? offset : 0.0f;
////            float yOffset = (offsetCol && x == xOfIndex(_selectedTile)) ? offset : 0.0f;
////            // Wrap
////            bounds = gridToScreen(x, y);
////            float xWrap = 0.0f;
////            float yWrap = 0.0f;
////            if (bounds.getMidX()+xOffset <= 0)
////                xWrap = getCellLength() * _width;
////            if (bounds.getMidX()+xOffset > getCellLength() * _width)
////                xWrap = -getCellLength() * _width;
////            if (bounds.getMidY()+yOffset <= 0)
////                yWrap = getCellLength() * _height;
////            if (bounds.getMidY()+yOffset > getCellLength() * _height)
////                yWrap = -getCellLength() * _height;
////            // Bounds
////            float xf = bounds.getMinX() + xOffset + _tilePadding/2.0f + xWrap;
////            float yf = bounds.getMinY() + yOffset + _tilePadding/2.0f + yWrap;
////            float width = bounds.size.width - _tilePadding;
////            float height = bounds.size.height - _tilePadding;
////            //Temporary for cool animation
////            if (animationCounter >= 0) {
////                int locat = indexOfCoordinate(x, y);
////                if (totalReplaceTiles.find(locat) != totalReplaceTiles.end() && animationCounter < 100) {
////                    if (animationCounter < 60) { //Destroy
////                        if (animationCounter < 30) {
////                            bounds.set(xf + animationCounter * getCellLength() / 300, yf + animationCounter * getCellLength() / 300, width - animationCounter * getCellLength() / 150, height - animationCounter * getCellLength() / 150);
////                        }
////                        if (animationCounter < 54) {
////                            bounds.set(xf + getCellLength() / 20, yf + getCellLength() / 20, width - getCellLength() / 10, height - getCellLength() / 10);
////                        }
////                        else {
////                            bounds.set(xf + ((animationCounter-54) * getCellLength() / 42), yf + ((animationCounter - 54) * getCellLength() / 20), width - ((animationCounter - 54) * getCellLength() / 10), height - ((animationCounter - 54) * getCellLength() / 10));
////                        }
////                    }
////                    else { //Regenerate
////                        if (animationCounter > 66) {
////                            bounds.set(xf + getCellLength() / 20, yf + getCellLength() / 20, width - getCellLength() / 10, height - getCellLength() / 10);
////                        }
////                        else {
////                            bounds.set(xf + ((66 - animationCounter) * getCellLength() / 20), yf + ((66 - animationCounter) * getCellLength() / 20), width - ((66 - animationCounter) * getCellLength() / 10), height - ((66 - animationCounter) * getCellLength() / 10));
////                        }
////                    }
////                    if (_tiles[indexOfCoordinate(x, y)].getColor() == 0) {
////                        batch->draw(tile1Texture, bounds);
////                    } else if (_tiles[indexOfCoordinate(x, y)].getColor() == 1) {
////                        batch->draw(tile2Texture, bounds);
////                    } else {
////                        batch->draw(tileTexture, Color4(colorLookup.at(_tiles[indexOfCoordinate(x, y)].getColor())).scale(.85, false), bounds);
////                    }
////                }
////                else {
////                    bounds.set(xf, yf, width, height);
////                    if (_tiles[indexOfCoordinate(x, y)].getColor() == 0) {
////                        batch->draw(tile1Texture, bounds);
////                    } else if (_tiles[indexOfCoordinate(x, y)].getColor() == 1) {
////                        batch->draw(tile2Texture, bounds);
////                    } else {
////                        batch->draw(tileTexture, colorLookup.at(_tiles[indexOfCoordinate(x, y)].getColor()), bounds);
////                    }
////
////                }
////            }
////            else {
////                bounds.set(xf, yf, width, height);
////                if (_tiles[indexOfCoordinate(x, y)].getColor() == 0) {
////                    batch->draw(tile1Texture, bounds);
////                } else if (_tiles[indexOfCoordinate(x, y)].getColor() == 1) {
////                    batch->draw(tile2Texture, bounds);
////                } else {
////                    batch->draw(tileTexture, colorLookup.at(_tiles[indexOfCoordinate(x, y)].getColor()), bounds);
////                }
////            }
////            //end of cool animation
////            //bounds.set(xf, yf, width, height);
////        }
////    }
//
//    // Draw Loop
//    // TODO: bug where top tile get drawn in wrong order when stuff wraps around.
//    for (int y = _height-1; y >= 0; y--) {
//        for (int x = 0; x < _width; x++) {
//            Rect tileBounds = calculateDrawBounds(x, y);
//            // TODO: modify draw order based on vertical wrap offset (if in selected column)
//
//            // Show tile selection
//            if (_selectedTile != -1) {
//                int selX = xOfIndex(_selectedTile);
//                int selY = yOfIndex(_selectedTile);
//                if (selX == x && selY == y) {
//                    float padding = _tilePadding/2.0f;
//                    float newX = tileBounds.getMinX() + padding/2.0f;
//                    float newY = tileBounds.getMinY() - padding/2.0f;
//                    float newWidth = tileBounds.size.width - padding;
//                    float newHeight = tileBounds.size.height - padding;
//                    tileBounds.set(newX, newY, newWidth, newHeight);
//                }
//            }
//
//            // Draw Tile
//            drawTile(batch, tileBounds, getTile(x, y));
//
//            // Draw selection over selected tile (TODO: change this to maybe scale tileBounds down as if pressed down)
////            if (_selectedTile != -1) {
////                int selX = xOfIndex(_selectedTile);
////                int selY = yOfIndex(_selectedTile);
////                if (selX == x && selY == y) {
////                    batch->draw(tileTexture, Color4f(0.0f, 0.0f, 0.0f, 0.2f), tileBounds);
////                }
////            }
//
//            // Draw Ally
//            PlayerPawnModel *ally = getAllyPtr(x, y);
//            if (ally) { drawAlly(batch, tileBounds); }
//
//            // Draw Enemy
//            EnemyPawnModel *enemy = getEnemyPtr(x, y);
//            if (enemy) { drawEnemy(batch, tileBounds, *enemy); }
//        }
//    }
//
//    batch->end();
//}

/**
* Returns a string representation of the board for debugging purposes.
*
* @return a string representation of this vector for debuggging purposes.
*/
std::string BoardModel::toString() const {
    std::stringstream ss;
    for (int j = _height-1; j >= 0; j--) {
        for (int i = 0; i < _width; i++) {
            ss << " ";
            ss << _tiles[indexOfCoordinate(i, j)]->getColor();
        }
        ss << "\n";
    }
    ss << "allies: [";
    for (int i = 0; i < _numAllies; i++) {
        ss << "(" << _allies[i]->getX() << ", " << _allies[i]->getY() << ")";
        ss << "   ";
    }
    ss << "]";
    ss << "\nenemies: [";
    for (int i = 0; i < _numEnemies; i++) {
        ss << "(" << _enemies[i]->getX() << ", " << _enemies[i]->getY() << ")";
        ss << "   ";
    }
    ss << "]";
    return ss.str();
}

