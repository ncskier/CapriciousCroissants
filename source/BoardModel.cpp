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
	_placeAllies(false),
	offsetRowIdx(-1),
	offsetColIdx(-1),
	offsetRowValue(0.0f),
	offsetColValue(0.0f) {
		colorLookup = { Color4::WHITE, Color4::RED, Color4::BLACK, Color4::MAGENTA, Color4::BLUE };
		srand((int)time(NULL));
		generateNewBoard();
		while (checkForMatches());
		srand((int)time(NULL));
}

// Allocates this board for a shared pointer
std::shared_ptr<BoardModel> BoardModel::alloc() {
	std::shared_ptr<BoardModel> result = std::make_shared<BoardModel>();
	return result;
}

// Destroy any values needed to be deleted for this class
BoardModel::~BoardModel() {
	delete[] _tiles;
	delete[] _allies;
	delete[] _enemies;

	_tiles = nullptr;
	_allies = nullptr;
	_enemies = nullptr;
}

//Convert (x, y) coordinate to array index. (0, 0) is the upper left corner
//Example for sideSize = 3
//  0 1 2     (0,0) (1,0) (2,0)
//  3 4 5     (0,1) (1,1) (2,1)
//  6 7 8     (0,2) (1,2) (2,2) (3) (4)
int BoardModel::indexOfCoordinate(int x, int y) const {
	return x + (y*_width);
}

//Returns the value at the give (x, y) coordinate
TileModel BoardModel::getTile(int x, int y) const {
	return _tiles[indexOfCoordinate(x, y)];
}

//Returns the value at the give (x, y) coordinate
PlayerPawnModel BoardModel::getAlly(int x, int y) const {
	return _allies[indexOfCoordinate(x, y)];
}

//Returns the value at the give (x, y) coordinate
PlayerPawnModel BoardModel::getEnemy(int x, int y) const {
	return _enemies[indexOfCoordinate(x, y)];
}

//Set the value at the given (x, y) coordinate
void BoardModel::set(int x, int y, TileModel t) {
	_tiles[indexOfCoordinate(x, y)] = t;
}

// Place ally at index i of _allies on location (x, y)
void BoardModel::placeAlly(int x, int y, int i) {
	/*_allies[i].x = x;
	_allies[i].y = y;*/
	//TODO^^
}

// Place enemy at index i of _enemies on location (x, y)
void BoardModel::placeEnemy(int x, int y, int i) {
	/*_enemies[i].x = x;
	_enemies[i].y = y;*/
	//TODO^^
}

// Remove ally at index i
void BoardModel::removeAlly(int i) {
	/*_allies[i].x = -1;
	_allies[i].y = -1;*/
	//TODO^^
}

// Remove enemy at index i
void BoardModel::removeEnemy(int i) {
	/*_enemies[i].x = -1;
	_enemies[i].y = -1;*/
	//TODO^^
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
	for (iter = replaceTiles.begin(); iter != replaceTiles.end(); iter++) {
		// Replace tile
		replaceTile(*iter);
		/*// Remove pawn
		if (_enemies != nullptr) {
			for (int i = 0; i < _numEnemies; i++) {
				if (indexOfCoordinate(_enemies[i].x, _enemies[i].y) == *iter) { 
					removeEnemy(i);
				}
			}
		}*/
		//TODO^^
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

	// Setup Pawns
	/*_pawns = new Vec2[_numPawns]; //Should this be a smart pointer?
	int x;
	int y;
	for (int i = 0; i < _numPawns; i++) {
		x = rand() % _sideSize;
		y = rand() % _sideSize;
		_pawns[i].set(x, y);
	}*/
	//TODO^^^^ Need to setup allies and enemies
}

// Slide pawns in row or column [k] by [offset]
void BoardModel::slidePawns(bool row, int k, int offset) {
	/*// Slide pawns
	for (int i = 0; i < _numPawns; i++) {
		Vec2 pawn = _pawns[i];
		if (pawn.x != -1 && pawn.y != -1) {
			if (row) {
				// Row
				if (k == pawn.y) {
					float x = ((int)pawn.x + offset) % _sideSize;
					while (x < 0) {
						x += _sideSize;
					}
					_pawns[i].x = x;
				}
			}
			else {
				// Column
				if (k == pawn.x) {
					float y = ((int)pawn.y + offset) % _sideSize;
					while (y < 0) {
						y += _sideSize;
					}
					_pawns[i].y = y;
				}
			}
		}
	}*/
	//TODO^^^^^^ Slide allies and enemies in row/column 
}

//Slide row or column by [offset]
void BoardModel::slide(bool row, int k, int offset) {
	/*// Copy
	std::unique_ptr<int[]> line(new int[_sideSize]);
	for (int i = 0; i < _sideSize; i++) {
		int x = row ? i : k;
		int y = row ? k : i;
		line[i] = get(x, y);
	}

	// Slide/write row
	for (int i = 0; i < _sideSize; i++) {
		int x = row ? i : k;
		int y = row ? k : i;
		int j = (i - offset) % _sideSize;
		while (j < 0) {
			j += _sideSize;
		}
		set(x, y, line[j]);
	}

	// Slide pawns
	slidePawns(row, k, offset);*/
	//TODO^^^^^^ Take into account differing height/width
}

//Offset view of row (not model)
void BoardModel::offsetRow(int idx, float value) {
	offsetRowIdx = idx;
	offsetRowValue = value;
}

//Offset view of col (not model)
void BoardModel::offsetCol(int idx, float value) {
	offsetColIdx = idx;
	offsetColValue = value;
}

//Offset reset
void BoardModel::offsetReset() {
	offsetRowIdx = -1;
	offsetColIdx = -1;
	offsetRowValue = 0.0f;
	offsetColValue = 0.0f;
}

//Slide row [y] by [offset]
void BoardModel::slideRow(int y, int offset) {
	slide(true, y, offset);
}

//Slide column [x] by [offset]
void BoardModel::slideCol(int x, int offset) {
	slide(false, x, offset);
}

// Draws all of the tiles and pawns(in that order) 
void BoardModel::draw(const std::shared_ptr<SpriteBatch>& batch) {
	batch->begin();

	float tileWidth = (gameWidth - 20) / 5;
	float tileHeight = (gameHeight - 20) / 5;
	float usedSize = tileWidth > tileHeight ? tileHeight : tileWidth;
	Rect bounds = Rect(0, 0, usedSize, usedSize);

	for (int x = 0; x < _width; x++) {
		float xPos = usedSize * x + (x * 5);
		for (int y = 0; y < _height; y++) {
			float yPos = usedSize * y + (y * 5);
			bounds.set(xPos, yPos, usedSize, usedSize);
			batch->draw(tileTexture, _tiles[indexOfCoordinate(x, y)].getColor(), bounds);
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
	/*std::stringstream ss;
	for (int j = 0; j < _height; j++) {
		for (int i = 0; i < _width; i++) {
			ss << " ";
			ss << _tiles[indexOfCoordinate(i, j)];
		}
		ss << "\n";
	}
	ss << "[";
	for (int i = 0; i < _numPawns; i++) {
		ss << _pawns[i].toString();
		ss << "   ";
	}
	return ss.str();*/
	//TODO^^^ Take into account allies and enemies
    return "";
}

