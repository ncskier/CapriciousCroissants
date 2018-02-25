//
// TileBoard.cpp
// Cpp file for the board that contains pawns/tile data


#include "TileBoard.h"
#include <cugl/base/CUBase.h>

using namespace cugl;

// Initialize _sideSize and call generateNewBoard to generate _tiles and empty _pawns
TileBoard::TileBoard() {

}

// Allocates this board for a shared pointer
std::shared_ptr<TileBoard> TileBoard::alloc() {
	std::shared_ptr<TileBoard> result = std::make_shared<TileBoard>();
	return result;
}


// Destroy any values needed to be deleted for this class
TileBoard::~TileBoard() {
	_tiles = nullptr;
	_pawns = nullptr;
}

// Place a pawn for the desired player(array index) at the desired location
void TileBoard::placePawn(Vec2 *desiredLocation, int desiredPlayer) {

}

// Check if any matches exist on the board, if so then remove them and check for pawn locations for damage/removal
boolean TileBoard::checkForMatches() {
	return false;
}

// Private function that allows for a tile to be replaced based on it's array index value in _tiles
void TileBoard::replaceTile(int tileLocation) {

}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void TileBoard::generateNewBoard() {

}

// Draws all of the tiles and pawns(in that order) 
void TileBoard::draw() {

}