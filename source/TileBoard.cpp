//
// TileBoard.cpp
// Cpp file for the board that contains pawns/tile data

#include <sstream>
#include <iostream>
#include <ctime>

#include "TileBoard.h"
#include <cugl/base/CUBase.h>

using namespace cugl;

// Initialize _sideSize and call generateNewBoard to generate _tiles and empty _pawns
TileBoard::TileBoard() :
_sideSize(5),
_numColors(5),
_numPawns(4) {
    generateNewBoard();
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
    std::vector<int> toBeReplaced;

    for (int y = 0; y < _sideSize; y++){
        for (int x = 0; x < _sideSize; x++){
            // Check if tile was already removed
            if (get(x,y) != -1) {
                //Search upwards
                if (y > 1) {
                    // At least 3 in a row?
                    if (get(x, y-1) == get(x,y) && get(x, y-2) == get(x,y)){
                        int pointer = y;
                        bool pointIsMatch = true;
                        while(pointIsMatch && pointer >= 0){
                            //Add pointed tile to be replaced if not there already
                            if(std::find(toBeReplaced.begin(), toBeReplaced.end(), indexOfCoordinate(x, pointer)) == toBeReplaced.end()) {
                                toBeReplaced.push_back(indexOfCoordinate(x, pointer));
                            }
                            pointer--; //Move to next tile
                            //Check if still match
                            if (pointer >= 0 && get(x, pointer) == get(x,y)){
                                pointIsMatch = true;
                            }
                            else{
                                pointIsMatch = false;
                            }
                        }
                    }
                }
                // Search rightwards
                if (x < _sideSize - 2) {
                    // At least 3 in a row?
                    if (get(x+1, y) == get(x,y) && get(x+2, y) == get(x,y)){
                        int pointer = x;
                        bool pointIsMatch = true;
                        while(pointIsMatch && pointer < _sideSize){
                            //Add pointed tile to be replaced if not there already
                            if(std::find(toBeReplaced.begin(), toBeReplaced.end(), indexOfCoordinate(pointer, y)) == toBeReplaced.end()) {
                                toBeReplaced.push_back(indexOfCoordinate(pointer, y));
                            }
                            pointer++; //Move to next tile
                            //Check if still match
                            if (pointer < _sideSize && get(pointer, y) == get(x,y)){
                                pointIsMatch = true;
                            }
                            else{
                                pointIsMatch = false;
                            }
                        }
                    }
                }
                // Search downwards
                if (y < _sideSize - 2){
                    // At least 3 in a row?
                    if (get(x, y+1) == get(x,y) && get(x, y+2) == get(x,y)){
                        int pointer = y;
                        bool pointIsMatch = true;
                        while(pointIsMatch && pointer < _sideSize){
                            //Add pointed tile to be replaced if not there already
                            if(std::find(toBeReplaced.begin(), toBeReplaced.end(), indexOfCoordinate(x, pointer)) == toBeReplaced.end()) {
                                toBeReplaced.push_back(indexOfCoordinate(x, pointer));
                            }
                            pointer++; //Move to next tile
                            //Check if still match
                            if (pointer < _sideSize && get(x, pointer) == get(x,y)){
                                pointIsMatch = true;
                            }
                            else{
                                pointIsMatch = false;
                            }
                        }
                    }
                }
                //search leftwards
                if(x > 1){
                    // At least 3 in a row?
                    if (get(x-1, y) == get(x,y) && get(x-2, y) == get(x,y)){
                        int pointer = x;
                        bool pointIsMatch = true;
                        while(pointIsMatch && pointer >= 0){
                            //Add pointed tile to be replaced if not there already
                            if(std::find(toBeReplaced.begin(), toBeReplaced.end(), indexOfCoordinate(pointer, y)) == toBeReplaced.end()) {
                                toBeReplaced.push_back(indexOfCoordinate(pointer, y));
                            }
                            pointer++; //Move to next tile
                            //Check if still match
                            if (pointer >= 0 && get(pointer, y) == get(x,y)){
                                pointIsMatch = true;
                            }
                            else{
                                pointIsMatch = false;
                            }
                        }
                    }
                }
            }
        }
    }

    if(!toBeReplaced.empty()) {
        //Remove pawns that are on tiles to be replaced
        for (int i = 0; i < _numPawns; i++) {
            if (std::find(toBeReplaced.begin(), toBeReplaced.end(),
                          indexOfCoordinate(_pawns[i].x, _pawns[i].y)) != toBeReplaced.end()) {
                removePawn(i);
            }
        }

        //Replace tiles that need to be replaced
        for (int j = 0; j < toBeReplaced.size(); j++){
            replaceTile(toBeReplaced[j]);
        }
        return true;
    }
    else{
        return false;
    }
}

// Private function that allows for a tile to be replaced based on it's array index value in _tiles
void TileBoard::replaceTile(int tileLocation) {
    srand((int)time(0));
    _tiles[tileLocation] = rand() % _numColors;
}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void TileBoard::generateNewBoard() {
    // Setup Tiles
    _tiles = new int[_sideSize*_sideSize];
    int color;
    srand((int)time(0));
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
    int line[_sideSize];
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

//Slide row [y] by [offset]
void TileBoard::slideRow(int y, int offset) {
    slide(true, y, offset);
}

//Slide column [x] by [offset]
void TileBoard::slideCol(int x, int offset) {
    slide(false, x, offset);
}

// Draws all of the tiles and pawns(in that order) 
void TileBoard::draw() {
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
