//
// TileBoard.cpp
// Cpp file for the board that contains pawns/tile data

#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <set>

#include "TileBoard.h"
#include <cugl/base/CUBase.h>

using namespace cugl;

// Initialize _sideSize and call generateNewBoard to generate _tiles and empty _pawns
TileBoard::TileBoard() :
_sideSize(5),
_numColors(5),
_numPawns(4),
offsetRowIdx(-1),
offsetColIdx(-1),
offsetRowValue(0.0f),
offsetColValue(0.0f) {
	colorLookup = {Color4::WHITE, Color4::RED, Color4::BLACK, Color4::MAGENTA, Color4::BLUE};
    srand((int)time(NULL));
    generateNewBoard();
    while (checkForMatches());
	srand((int)time(NULL));
}

// Allocates this board for a shared pointer
std::shared_ptr<TileBoard> TileBoard::alloc() {
	std::shared_ptr<TileBoard> result = std::make_shared<TileBoard>();
	return result;
}


// Destroy any values needed to be deleted for this class
TileBoard::~TileBoard() {
    delete[] _tiles;
    delete[] _pawns;
    
    _tiles = nullptr;
	_pawns = nullptr;
}

//Convert (x, y) coordinate to array index. (0, 0) is the upper left corner
//Example for sideSize = 3
//  0 1 2     (0,0) (1,0) (2,0)
//  3 4 5     (0,1) (1,1) (2,1)
//  6 7 8     (0,2) (1,2) (2,2) (3) (4)
int TileBoard::indexOfCoordinate(int x, int y) const {
    return x + (y*_sideSize);
}

//Returns the value at the give (x, y) coordinate
int TileBoard::get(int x, int y) const {
    return _tiles[ indexOfCoordinate(x, y) ];
}

//Set the value at the given (x, y) coordinate
void TileBoard::set(int x, int y, int value) {
    _tiles[ indexOfCoordinate(x, y) ] = value;
}

// Place pawn at index i on location (x, y)
void TileBoard::placePawn(int x, int y, int i) {
    _pawns[i].set(x, y);
}

// Remove pawn at index i
void TileBoard::removePawn(int i) {
    _pawns[i].set(-1, -1);
}

// Check if any matches exist on the board, if so then remove them and check for pawn locations for damage/removal
bool TileBoard::checkForMatches() {
    std::set<int> replaceTiles;
    int *row = new int[_sideSize];
    int *col = new int[_sideSize];
    
    // Check for matches
    for (int i = 0; i < _sideSize; i++) {
        for (int j = 0; j < _sideSize; j++) {
            row[j] = get(j, i);
            col[j] = get(i, j);
            if (j >= 2) {
                // Check Row
                if (row[j] == row[j-1] && row[j-1] == row[j-2]) {
                    replaceTiles.insert(indexOfCoordinate(j, i));
                    replaceTiles.insert(indexOfCoordinate(j-1, i));
                    replaceTiles.insert(indexOfCoordinate(j-2, i));
                }
                // Check Column
                if (col[j] == col[j-1] && col[j-1] == col[j-2]) {
                    replaceTiles.insert(indexOfCoordinate(i, j));
                    replaceTiles.insert(indexOfCoordinate(i, j-1));
                    replaceTiles.insert(indexOfCoordinate(i, j-2));
                }
            }
        }
    }
    
    bool matchExists = !replaceTiles.empty();
    delete [] row;
    delete [] col;
    
    // Replace Tiles
    std::set<int>::iterator iter;
    for (iter = replaceTiles.begin(); iter != replaceTiles.end(); iter++) {
        // Replace tile
        replaceTile(*iter);
        // Remove pawn
        for (int i = 0; i < _numPawns; i++) {
            if (indexOfCoordinate(_pawns[i].x, _pawns[i].y) == *iter) {
                removePawn(i);
            }
        }
    }
    
    return matchExists;
}

// Private function that allows for a tile to be replaced based on it's array index value in _tiles
void TileBoard::replaceTile(int tileLocation) {
    _tiles[tileLocation] = rand() % _numColors;
}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void TileBoard::generateNewBoard() {
    // Setup Tiles
    _tiles = new int[_sideSize*_sideSize];
    int color;
    for (int i = 0; i < _sideSize*_sideSize; i++) {
        color = rand() % _numColors;        // random number in range [0, _numColors-1]
        _tiles[i] = color;
    }
    
    // Setup Pawns
    _pawns = new Vec2[_numPawns];
}

//Slide row or column by [offset]
void TileBoard::slide(bool row, int k, int offset) {
    // Copy
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
}

//Offset view of row (not model)
void TileBoard::offsetRow(int idx, float value) {
    offsetRowIdx = idx;
    offsetRowValue = value;
}

//Offset view of col (not model)
void TileBoard::offsetCol(int idx, float value) {
    offsetColIdx = idx;
    offsetColValue = value;
}

//Offset reset
void TileBoard::offsetReset() {
    offsetRowIdx = -1;
    offsetColIdx = -1;
    offsetRowValue = 0.0f;
    offsetColValue = 0.0f;
}

//Slide row [y] by [offset]
void TileBoard::slideRow(int y, int offset) {
    slide(true, y, offset);
}

//Slide column [x] by [offset]
void TileBoard::slideCol(int x, int offset) {
    slide(false, x, offset);
}

// Draws all of the tiles and pawns(in that order) 
void TileBoard::draw(const std::shared_ptr<SpriteBatch>& batch) {
	batch->begin();
	
	float tileWidth = (gameWidth - 20) / 5;
	float tileHeight = (gameHeight- 20) / 5;
	float usedSize = tileWidth > tileHeight ? tileHeight : tileWidth;
	Rect bounds = Rect(0, 0, usedSize, usedSize);

	for (int x = 0; x < _sideSize; x++) {
		float xPos = usedSize * x + (x * 5);
		for (int y = 0; y < _sideSize; y++) {
			float yPos = usedSize * y + (y * 5);
			bounds.set(xPos, yPos, usedSize, usedSize);
			batch->draw(tileTexture, colorLookup.at(_tiles[indexOfCoordinate(x, y)]), bounds);
		}
	}

	batch->end();
    // TODO: draw()
}

/**
 * Returns a string representation of the board for debugging purposes.
 *
 * @return a string representation of this vector for debuggging purposes.
 */
std::string TileBoard::toString() const {
    std::stringstream ss;
    for (int j = 0; j < _sideSize; j++) {
        for (int i = 0; i < _sideSize; i++) {
            ss << " ";
            ss << _tiles[ indexOfCoordinate(i, j) ];
        }
        ss << "\n";
    }
    ss << "[";
    for (int i = 0; i < _numPawns; i++) {
        ss << _pawns[i].toString();
        ss << "   ";
    }
    return ss.str();
}
