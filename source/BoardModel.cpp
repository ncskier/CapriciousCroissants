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
TileModel BoardModel::get(int x, int y) const {
	return _tiles[indexOfCoordinate(x, y)];
}

//Set the value at the given (x, y) coordinate
void BoardModel::set(int x, int y, TileModel t) {
	_tiles[indexOfCoordinate(x, y)] = t;
}

// Place ally at index i of _allies on location (x, y)
void BoardModel::placeAlly(int x, int y, int i) {
	_allies[i].x = x;
	_allies[i].y = y;
}

// Place enemy at index i of _enemies on location (x, y)
void BoardModel::placeAlly(int x, int y, int i) {
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
			row[j] = get(j, i);
			col[j] = get(i, j);
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
		// Remove pawn
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
	Color4 color = colorLookup.at(rand() % _numColors);
	_tiles[tileLocation].setColor(color);
}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void BoardModel::generateNewBoard() {
	// Setup Tiles
	_tiles = new int[_height*_width];
	int color;
	for (int i = 0; i < _height*_width; i++) {
		color = rand() % _numColors;        // random number in range [0, _numColors-1]
		_tiles[i] = color;
	}

	// Replace any matches
	while (checkForMatches());

	// Setup Pawns
	_pawns = new Vec2[_numPawns];
	int x;
	int y;
	for (int i = 0; i < _numPawns; i++) {
		x = rand() % _sideSize;
		y = rand() % _sideSize;
		_pawns[i].set(x, y);
	}
}




