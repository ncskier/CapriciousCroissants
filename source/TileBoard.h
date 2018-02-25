//
// TileBoard.h
// Header file for the data structure that holds the information for the tiled board


#ifndef __TILEBOARD_H__
#define __TILEBOARD_H__
#include <cugl/cugl.h>

/**
* Class for the main board data structure of game
*
* Holds reference to the tiles and the pawns on the board
*
*/

class TileBoard {
protected:
	//For now just treat tiles as an array of ints, where an int is the color of the tile
	std::shared_ptr<int[]> _tiles;

	//Treat pawns as an array of a vector 2 for locations of the pawns
	std::shared_ptr<cugl::Vec2[]> _pawns;

	//Replaces a tile at a location in the array with a new value
	void replaceTile(int tileLocation);

	int _sideSize;
	
public:
	TileBoard();
	~TileBoard();
	
	//Places a pawn for a player, keeping the same vector so it can be altered by tileboard or boardScreen
	void placePawn(cugl::Vec2 *desiredLocation, int desiredPlayer);

	//Return true if a match is found (and replace those matches, damaging pawns on matches), otherwise false
	boolean checkForMatches();

	static std::shared_ptr<TileBoard> alloc();

	//Generates a new board into the _tiles variable
	void generateNewBoard(); 

	//Draws tiles and pawns
	void draw();
};

#endif