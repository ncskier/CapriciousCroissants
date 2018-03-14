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
_enemies(nullptr),
_allies(nullptr),
_tiles(nullptr),
_placeAllies(false),
_boardPadding(10.0f),
_tilePadding(5.0f),
offsetRow(false),
offsetCol(false),
offset(0.0f) {
    colorLookup = { Color4::CORNFLOWER, Color4::RED, Color4::CYAN, Color4::MAGENTA, Color4::BLUE, Color4::GREEN, Color4::YELLOW, Color4::WHITE };
    srand((int)time(NULL));
    generateNewBoard();
    while (checkForMatches());
    srand((int)time(NULL));
}

BoardModel::BoardModel(int width, int height, int colors, int allies, int enemies, bool placePawn) :
_height(height),
_width(width),
_numColors(colors),
_numAllies(allies),
_numEnemies(enemies),
_selectedTile(-1),
_enemies(nullptr),
_allies(nullptr),
_tiles(nullptr),
_placeAllies(placePawn),
_boardPadding(10.0f),
_tilePadding(5.0f),
offsetRow(false),
offsetCol(false),
offset(0.0f) {
    colorLookup = { Color4::CORNFLOWER, Color4::RED, Color4::CYAN, Color4::MAGENTA, Color4::BLUE, Color4::GREEN, Color4::YELLOW, Color4::WHITE };
	srand((int)time(NULL));
	generateNewBoard();
	while (checkForMatches());
	srand((int)time(NULL));
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
    _width = width;
    _height = height;
    return true;
}

// Destroy any values needed to be deleted for this class
void BoardModel::dispose() {
	delete[] _tiles;
	delete[] _allies;
	delete[] _enemies;

	_tiles = nullptr;
	_allies = nullptr;
	_enemies = nullptr;
}

//Convert (x, y) coordinate to array index. (0, 0) is the upper left corner
// Example for sideSize = 3
//  0 1 2     (0,0) (1,0) (2,0)
//  3 4 5     (0,1) (1,1) (2,1)
//  6 7 8     (0,2) (1,2) (2,2) (3) (4)
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
TileModel BoardModel::getTile(int x, int y) const {
	return _tiles[indexOfCoordinate(x, y)];
}

// Returns the ally pawn at index i of _allies
PlayerPawnModel BoardModel::getAlly(int i) const {
	return _allies[i];
}

// Returns the ally pawn at index i of _allies
PlayerPawnModel* BoardModel::getAllyPtr(int i) {
	return &_allies[i];
}


// Returns the enemy pawn at index i of _enemies
PlayerPawnModel BoardModel::getEnemy(int i) {
	return _enemies[i];
}

// Returns the enemy pawn at index i of _enemies
PlayerPawnModel* BoardModel::getEnemyPtr(int i) {
    return &_enemies[i];
}

// Returns the allies
PlayerPawnModel BoardModel::getAllies()  {
	return *_allies;
}

// Returns the enemies
PlayerPawnModel BoardModel::getEnemies()  {
	return *_enemies;
}


// Set the value at the given (x, y) coordinate
void BoardModel::setTile(int x, int y, TileModel t) {
	_tiles[indexOfCoordinate(x, y)] = t;
}

// Place ally at index i of _allies on location (x, y)
void BoardModel::placeAlly(int x, int y, int i) {
	_allies[i].x = x;
	_allies[i].y = y;
}
	
// Place enemy at index i of _enemies on location (x, y)
void BoardModel::placeEnemy(int x, int y, int i) {
	_enemies[i].x = x;
	_enemies[i].y = y;
}

void BoardModel::moveEnemy(int dx, int dy, int enemyIdx) {
    _enemies[enemyIdx].x += dx;
    _enemies[enemyIdx].y += dy;
}

// Remove ally at index i
void BoardModel::removeAlly(int i) {
	_allies[i].x = -1;
	_allies[i].y = -1;
}

// Remove enemy at index i
void BoardModel::removeEnemy(int i) {
	_enemies[i].x = -1;
	_enemies[i].y = -1;
}

// Check if any matches exist on the board, if so then remove them and check for pawn locations for damage/removal
bool BoardModel::checkForMatches() {
	std::set<int> replaceTiles;

	// Check for matches
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			// Check Row
			if (x >= 2) {
				if (_tiles[indexOfCoordinate(x,y)].getColor() == _tiles[indexOfCoordinate(x - 1, y)].getColor()
					&& _tiles[indexOfCoordinate(x - 1, y)].getColor() == _tiles[indexOfCoordinate(x - 2, y)].getColor()) {
					replaceTiles.insert(indexOfCoordinate(x, y));
					replaceTiles.insert(indexOfCoordinate(x - 1, y));
					replaceTiles.insert(indexOfCoordinate(x - 2, y));
				}
			}
			// Check Column
			if (y >= 2) {
				if (_tiles[indexOfCoordinate(x, y)].getColor() == _tiles[indexOfCoordinate(x, y - 1)].getColor()
					&& _tiles[indexOfCoordinate(x, y - 1)].getColor() == _tiles[indexOfCoordinate(x, y - 2)].getColor()) {
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
	for (iter = replaceTiles.begin(); iter != replaceTiles.end(); iter++) {
		// Replace tile
		replaceTile(*iter);
		// Remove enemies
		if (_enemies != nullptr) {
			for (int i = 0; i < _numEnemies; i++) {
				if (indexOfCoordinate(_enemies[i].x, _enemies[i].y) == *iter) { 
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
				if (_tiles[indexOfCoordinate(x, y)].getColor() == _tiles[indexOfCoordinate(x - 1, y)].getColor()
					&& _tiles[indexOfCoordinate(x - 1, y)].getColor() == _tiles[indexOfCoordinate(x - 2, y)].getColor()) {
					totalReplaceTiles.insert(indexOfCoordinate(x, y));
					totalReplaceTiles.insert(indexOfCoordinate(x - 1, y));
					totalReplaceTiles.insert(indexOfCoordinate(x - 2, y));
				}
			}
			// Check Column
			if (y >= 2) {
				if (_tiles[indexOfCoordinate(x, y)].getColor() == _tiles[indexOfCoordinate(x, y - 1)].getColor()
					&& _tiles[indexOfCoordinate(x, y - 1)].getColor() == _tiles[indexOfCoordinate(x, y - 2)].getColor()) {
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
		if (_enemies != nullptr) {
			for (int i = 0; i < _numEnemies; i++) {
				if (indexOfCoordinate(_enemies[i].x, _enemies[i].y) == *totalIter) {
					removeEnemy(i);
				}
			}
		}
	}

	animationCounter = 60;

	
}



// Private function that allows for a tile to be replaced based on it's array index value in _tiles
void BoardModel::replaceTile(int tileLocation) {
	int color = rand() % _numColors;
	_tiles[tileLocation].setColor(color);
}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void BoardModel::generateNewBoard() {
	// Setup Tiles
    srand((int)time(NULL));
	_tiles = new TileModel[_height*_width];
	int color;
	for (int i = 0; i < _height*_width; i++) {
		color = rand() % _numColors;        // random number in range [0, _numColors-1]
		_tiles[i].setColor(color);
	}

	// Replace any matches
	while (checkForMatches());

	// Setup Allies
	_allies = new PlayerPawnModel[_numAllies];
	int x;
	int y;
	for (int i = 0; i < _numAllies; i++) {
		x = rand() % _width;
		y = rand() % _height;
		if (i > 0) {
			while (true) {
				bool match = false;
				for (int j = 0; j < i; j++) {
					PlayerPawnModel temp = _allies[j];
					if (temp.x == x && temp.y == y) {
						x = rand() % _width;
						y = rand() % _height;
						match = true;
					}
				}
				if (!match) {
					break;
				}
			}
		}
		
		_allies[i].x = x;
		_allies[i].y = y;
	}

	//Setup Enemies
	_enemies = new PlayerPawnModel[_numEnemies];
	for (int i = 0; i < _numEnemies; i++) {
		x = rand() % _width;
		y = rand() % _height;
		while (true) {
			bool match = false;
			for (int j = 0; j < _numAllies; j++) {
				PlayerPawnModel temp = _allies[j];
				if (temp.x == x && temp.y == y) {
					x = rand() % _width;
					y = rand() % _height;
					match = true;
				}
			}
			if (i > 0) {
				for (int j = 0; j < i; j++) {
					PlayerPawnModel temp = _enemies[j];
					if (temp.x == x && temp.y == y) {
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

		_enemies[i].x = x;
		_enemies[i].y = y;
        _enemies[i].randomDirection();
	}
}

// Slide pawns in row or column [k] by [offset]
void BoardModel::slidePawns(bool row, int k, int offset) {
    // Slide Allies
    for (int i = 0; i < _numAllies; i++) {
        PlayerPawnModel pawn = _allies[i];
        if (pawn.x != -1 && pawn.y != -1) {
            if (row) {
                // Row
                if (k == pawn.y) {
                    float x = ((int)pawn.x + offset) % _width;
                    while (x < 0) {
                        x += _width;
                    }
                    _allies[i].x = x;
                }
            }
            else {
                // Column
                if (k == pawn.x) {
                    float y = ((int)pawn.y + offset) % _height;
                    while (y < 0) {
                        y += _height;
                    }
                    _allies[i].y = y;
                }
            }
        }
    }

    // Slide Enemies
    for (int i = 0; i < _numEnemies; i++) {
        PlayerPawnModel pawn = _enemies[i];
        if (pawn.x != -1 && pawn.y != -1) {
            if (row) {
                // Row
                if (k == pawn.y) {
                    float x = ((int)pawn.x + offset) % _width;
                    while (x < 0) {
                        x += _width;
                    }
                    _enemies[i].x = x;
                }
            }
            else {
                // Column
                if (k == pawn.x) {
                    float y = ((int)pawn.y + offset) % _height;
                    while (y < 0) {
                        y += _height;
                    }
                    _enemies[i].y = y;
                }
            }
        }
    }
}

//Slide row or column by [offset]
void BoardModel::slide(bool row, int k, int offset) {
	// Copy
	int sideSize = row ? _width : _height;
	TileModel *line = new TileModel[sideSize];
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
}

//Offset view of col (not model)
void BoardModel::setOffsetCol(float value) {
	offsetCol = true;
	offset = value;
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

float BoardModel::getCellLength() {
    float tileWidth = ((gameWidth - 2.0f*_boardPadding) / _width) - _tilePadding/2.0f;
    float tileHeight = ((gameHeight - 2.0f*_boardPadding) / _height) - _tilePadding/2.0f;
    float tileLength = tileWidth > tileHeight ? tileHeight : tileWidth;
    return tileLength + _tilePadding;
}

// Convert grid (x, y) to screen coordinates
cugl::Rect BoardModel::gridToScreen(int x, int y) {
    float cellLength = getCellLength();
    
    float xPos = _boardPadding/2.0f + x*cellLength;
    float yPos = _boardPadding/2.0f + y*cellLength;
    
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

// Draws all of the tiles and pawns(in that order) 
void BoardModel::draw(const std::shared_ptr<SpriteBatch>& batch) {
//    float gameLength = (gameWidth > gameHeight) ? gameHeight : gameWidth;
    Rect bounds;
    batch->begin();
	if (animationCounter >= 0) {
		animationCounter++;
	}
	if (animationCounter == 60) {
		triggerResets();
	}
	if (animationCounter > 120) {
		totalReplaceTiles.clear();
		animationCounter = -1;
	}
    for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {  //USE FUNCTIONS, 90% OF THIS CODE IS REPEATED 4x, EDITING IT IS A NIGHTMARE
            // Offset
            float xOffset = (offsetRow && y == yOfIndex(_selectedTile)) ? offset : 0.0f;
            float yOffset = (offsetCol && x == xOfIndex(_selectedTile)) ? offset : 0.0f;
            // Wrap
            bounds = gridToScreen(x, y);
            float xWrap = 0.0f;
            float yWrap = 0.0f;
            if (bounds.getMidX()+xOffset <= 0)
                xWrap = getCellLength() * _width;
            if (bounds.getMidX()+xOffset > getCellLength() * _width)
                xWrap = -getCellLength() * _width;
            if (bounds.getMidY()+yOffset <= 0)
                yWrap = getCellLength() * _height;
            if (bounds.getMidY()+yOffset > getCellLength() * _height)
                yWrap = -getCellLength() * _height;
            // Bounds
            float xf = bounds.getMinX() + xOffset + _tilePadding/2.0f + xWrap;
            float yf = bounds.getMinY() + yOffset + _tilePadding/2.0f + yWrap;
            float width = bounds.size.width - _tilePadding;
            float height = bounds.size.height - _tilePadding;
			//Temporary for cool animation
			if (animationCounter >= 0) {
				int locat = indexOfCoordinate(x, y);
				if (totalReplaceTiles.find(locat) != totalReplaceTiles.end() && animationCounter < 100) {
					if (animationCounter < 60) { //Destroy
						if (animationCounter < 30) {
							bounds.set(xf + animationCounter * getCellLength() / 300, yf + animationCounter * getCellLength() / 300, width - animationCounter * getCellLength() / 150, height - animationCounter * getCellLength() / 150);
						}
						if (animationCounter < 54) {
							bounds.set(xf + getCellLength() / 20, yf + getCellLength() / 20, width - getCellLength() / 10, height - getCellLength() / 10);
						}
						else {
							bounds.set(xf + ((animationCounter-54) * getCellLength() / 42), yf + ((animationCounter - 54) * getCellLength() / 20), width - ((animationCounter - 54) * getCellLength() / 10), height - ((animationCounter - 54) * getCellLength() / 10));
						}
					}
					else { //Regenerate
						if (animationCounter > 66) {
							bounds.set(xf + getCellLength() / 20, yf + getCellLength() / 20, width - getCellLength() / 10, height - getCellLength() / 10);
						}
						else {
							bounds.set(xf + ((66 - animationCounter) * getCellLength() / 20), yf + ((66 - animationCounter) * getCellLength() / 20), width - ((66 - animationCounter) * getCellLength() / 10), height - ((66 - animationCounter) * getCellLength() / 10));
						}
					}
					batch->draw(tileTexture, Color4(colorLookup.at(_tiles[indexOfCoordinate(x, y)].getColor())).scale(.85, false), bounds);
				}
				else {
					bounds.set(xf, yf, width, height);
					batch->draw(tileTexture, colorLookup.at(_tiles[indexOfCoordinate(x, y)].getColor()), bounds);
				}
			}
			else {
				bounds.set(xf, yf, width, height);
				batch->draw(tileTexture, colorLookup.at(_tiles[indexOfCoordinate(x, y)].getColor()), bounds);
			}
			//end of cool animation
            //bounds.set(xf, yf, width, height);
        }
    }
    
    // Draw over selected tile
    if (_selectedTile != -1) {
        int x = xOfIndex(_selectedTile);
        int y = yOfIndex(_selectedTile);
        // Offset
        float xOffset = (offsetRow) ? offset : 0.0f;
        float yOffset = (offsetCol) ? offset : 0.0f;
        // Wrap
        bounds = gridToScreen(x, y);
        float xWrap = 0.0f;
        float yWrap = 0.0f;
        if (bounds.getMidX() + xOffset <= 0)
        xWrap = getCellLength() * _width;
        if (bounds.getMidX() + xOffset > getCellLength() * _width)
        xWrap = -getCellLength() * _width;
        if (bounds.getMidY() + yOffset <= 0)
        yWrap = getCellLength() * _height;
        if (bounds.getMidY() + yOffset > getCellLength() * _height)
        yWrap = -getCellLength() * _height;
        float xf = bounds.getMinX() + xOffset + xWrap;
        float yf = bounds.getMinY() + yOffset + yWrap;
        float width = bounds.size.width;
        float height = bounds.size.height;
        bounds.set(xf, yf, width, height);
        batch->draw(tileTexture, Color4f(0.0f, 0.0f, 0.0f, 0.2f), bounds);
    }

    // Draw Pawns
    for (int i = 0; i < _numAllies; i++) {
        PlayerPawnModel ally = _allies[i];
        if (ally.x != -1 && ally.y != -1) {
            // Offset
            float xOffset = (offsetRow && ally.y == yOfIndex(_selectedTile)) ? offset : 0.0f;
            float yOffset = (offsetCol && ally.x == xOfIndex(_selectedTile)) ? offset : 0.0f;
            // Wrap
            bounds = gridToScreen(ally.x, ally.y);
            float xWrap = 0.0f;
            float yWrap = 0.0f;
			if (bounds.getMidX() + xOffset <= 0)
				xWrap = getCellLength() * _width;
			if (bounds.getMidX() + xOffset > getCellLength() * _width)
				xWrap = -getCellLength() * _width;
			if (bounds.getMidY() + yOffset <= 0)
				yWrap = getCellLength() * _height;
			if (bounds.getMidY() + yOffset > getCellLength() * _height)
				yWrap = -getCellLength() * _height;
            // Bounds
            float xf = bounds.getMinX() + xOffset + 5.0f*_tilePadding + xWrap;
            float yf = bounds.getMinY() + yOffset + 5.0f*_tilePadding + yWrap;
            float width = bounds.size.width - 10.0f*_tilePadding;
//            float height = bounds.size.height - 10.0f*_tilePadding;
            float height = playerTexture->getSize().height / playerTexture->getSize().width * width;
            bounds.set(xf, yf, width, height);
            batch->draw(playerTexture, bounds);
//            batch->draw(playerTexture, Color4::GRAY, bounds);
        }
    }
    
    // Draw Enemies
    for (int i = 0; i < _numEnemies; i++) {
         PlayerPawnModel enemy = _enemies[i];
        if (enemy.x != -1 && enemy.y != -1) {
            // Offset
            float xOffset = (offsetRow && enemy.y == yOfIndex(_selectedTile)) ? offset : 0.0f;
            float yOffset = (offsetCol && enemy.x == xOfIndex(_selectedTile)) ? offset : 0.0f;
            // Wrap
            bounds = gridToScreen(enemy.x, enemy.y);
            float xWrap = 0.0f;
            float yWrap = 0.0f;
			if (bounds.getMidX() + xOffset <= 0)
				xWrap = getCellLength() * _width;
			if (bounds.getMidX() + xOffset > getCellLength() * _width)
				xWrap = -getCellLength() * _width;
			if (bounds.getMidY() + yOffset <= 0)
				yWrap = getCellLength() * _height;
			if (bounds.getMidY() + yOffset > getCellLength() * _height)
				yWrap = -getCellLength() * _height;
            // Bounds
            float xf = bounds.getMinX() + xOffset + 5.0f*_tilePadding + xWrap;
            float yf = bounds.getMinY() + yOffset + 5.0f*_tilePadding + yWrap;
            float width = bounds.size.width - 10.0f*_tilePadding;
            float height = bounds.size.height - 10.0f*_tilePadding;
            bounds.set(xf, yf, width, height);
            batch->draw(tileTexture, Color4::BLACK, bounds);
            
            // Direction Indicator
            float padding = _tilePadding;
            bounds = gridToScreen(enemy.x, enemy.y);
            width = bounds.size.width/5.0f;
            height = bounds.size.height/5.0f;
            xf = bounds.getMidX() - width/2.0f;
            yf = bounds.getMidY() - height/2.0f;
            if (enemy.dx == 1)
                xf = bounds.getMaxX() - width - 5.0f*_tilePadding - padding;
            if (enemy.dx == -1)
                xf = bounds.getMinX() + 5.0f*_tilePadding + padding;
            if (enemy.dy == 1)
                yf = bounds.getMaxY() - height - 5.0f*_tilePadding - padding;
            if (enemy.dy == -1)
                yf = bounds.getMinY() + 5.0f*_tilePadding + padding;
            bounds.set(xf+xOffset+xWrap, yf+yOffset+yWrap, width, height);
            batch->draw(tileTexture, Color4::RED, bounds);
        }
    }

    batch->end();
}

/**
* Returns a string representation of the board for debugging purposes.
*
* @return a string representation of this vector for debuggging purposes.
*/
std::string BoardModel::toString() const {
    std::stringstream ss;
    for (int j = 0; j < _height; j++) {
        for (int i = 0; i < _width; i++) {
            ss << " ";
            ss << _tiles[indexOfCoordinate(i, j)].getColor();
        }
        ss << "\n";
    }
    ss << "allies: [";
    for (int i = 0; i < _numAllies; i++) {
        ss << "(" << _allies[i].x << ", " << _allies[i].y << ")";
        ss << "   ";
    }
    ss << "]";
    ss << "\nenemies: [";
    for (int i = 0; i < _numEnemies; i++) {
        ss << "(" << _enemies[i].x << ", " << _enemies[i].y << ")";
        ss << "   ";
    }
    ss << "]";
    return ss.str();
}

