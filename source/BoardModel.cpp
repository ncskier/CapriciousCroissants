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
_numEnemies(1),
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
    colorLookup = { Color4::WHITE, Color4::RED, Color4::BLACK, Color4::MAGENTA, Color4::BLUE };
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

// Returns the enemy pawn at index i of _enemies
PlayerPawnModel BoardModel::getEnemy(int i) const {
	return _enemies[i];
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
	int *row = new int[_height];
	int *col = new int[_width];

	// Check for matches
	for (int i = 0; i < _height; i++) {
		for (int j = 0; j < _width; j++) {
			row[j] = getTile(j, i).getColor();
			col[j] = getTile(i, j).getColor();
			if (j >= 2) {
				// Check Row
				if (row[j] == row[j - 1] && row[j - 1] == row[j - 2]) {
					replaceTiles.insert(indexOfCoordinate(j, i));
					replaceTiles.insert(indexOfCoordinate(j - 1, i));
					replaceTiles.insert(indexOfCoordinate(j - 2, i));
				}
				// Check Column
				if (col[j] == col[j - 1] && col[j - 1] == col[j - 2]) {
					replaceTiles.insert(indexOfCoordinate(i, j));
					replaceTiles.insert(indexOfCoordinate(i, j - 1));
					replaceTiles.insert(indexOfCoordinate(i, j - 2));
				}
			}
		}
	}

	bool matchExists = !replaceTiles.empty();
	delete[] row;
	delete[] col;

	// Replace Tiles
	std::set<int>::iterator iter;
    CULog("replace tiles");
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

// Private function that allows for a tile to be replaced based on it's array index value in _tiles
void BoardModel::replaceTile(int tileLocation) {
	int color = rand() % _numColors;
	_tiles[tileLocation].setColor(color);
}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void BoardModel::generateNewBoard() {
	// Setup Tiles
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
		_allies[i].x = x;
		_allies[i].y = y;
	}

	//Setup Enemies
	_enemies = new PlayerPawnModel[_numEnemies];
	int a;
	int b;
	for (int i = 0; i < _numEnemies; i++) {
		a = rand() % _width;
		b = rand() % _height;
		_enemies[i].x = a;
		_enemies[i].y = b;
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
    CULog("(%d, %d)", x, y);
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
    Rect bounds;
    
    batch->begin();

    for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
            float xOffset = (offsetRow && y == yOfIndex(_selectedTile)) ? offset : 0.0f;
            float yOffset = (offsetCol && x == xOfIndex(_selectedTile)) ? offset : 0.0f;
            bounds = gridToScreen(x, y);
            bounds.set(
                       bounds.getMinX() + _tilePadding/2.0f + xOffset,
                       bounds.getMinY() + _tilePadding/2.0f + yOffset,
                       bounds.size.width - _tilePadding,
                       bounds.size.height - _tilePadding);
            batch->draw(tileTexture, colorLookup.at(_tiles[indexOfCoordinate(x, y)].getColor()), bounds);
        }
    }

    // Draw Pawns
    /*for (int i = 0; i < _numPawns; i++) {
        Vec2 pawn = _pawns[i];
        if (pawn.x != -1 && pawn.y != -1) {
            float xPos = usedSize * pawn.x + (pawn.x * _sideSize) + (usedSize / 4.0f);
            float yPos = usedSize * pawn.y + (pawn.y * _sideSize) + (usedSize / 4.0f);
            bounds.set(xPos, yPos, usedSize / 2.0f, usedSize / 2.0f);
            batch->draw(tileTexture, Color4::GRAY, bounds);
        }
    }*/
    //TODO ^^^

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
    ss << "[";
    for (int i = 0; i < _numAllies; i++) {
        ss << "(" << _allies[i].x << ", " << _allies[i].y << ")";
        ss << "   ";
    }
    ss << "]";
    return ss.str();
}

