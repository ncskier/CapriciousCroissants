//
//  BoardModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Board_Model_H__
#define __Board_Model_H__

#include <cugl/cugl.h>
#include "TileModel.h"
#include "PlayerPawnModel.h"

/** Class of the board model*/
class BoardModel {
protected:
	// Size of vertical (column) side of the square board
	int _height;

	// Size of horizontal (row) side of the square board
	int _width;

	// Number of colors of tiles available to the board
	int _numColors;

	// Number of allies
	int _numAllies;

	// Number of enemies
	int _numEnemies;

	// True: Player places allies at beggining of game
	// False: Allies are already placed on board initially
	bool _placeAllies;
    
    // Tile selected by move
    int _selectedTile;

    // Colors to display
	std::vector<cugl::Color4> colorLookup;

	// Array of all tiles on the board, array index can be translated to coordinates
	TileModel *_tiles;

	// Array of all allies on the board
	PlayerPawnModel *_allies;

	// Array of all enemies on the board
	PlayerPawnModel *_enemies;

	// Replaces a tile at a location in the array with a new value
	void replaceTile(int tileLocation);

	// Convert (x, y) coordinate to array index. (0, 0) is the upper left corner
	int indexOfCoordinate(int x, int y) const;
    
    // Convert array index to x
    int columnOfIndex(int i) const;
    
    // Convert array index to y
    int rowOfIndex(int i) const;

	// Slide row or column by [offset]
	void slide(bool row, int k, int offset);

	// Slide pawns in row or column [k] by [offset]
	void slidePawns(bool row, int k, int offset);

public:
	BoardModel();
    
    ~BoardModel() { dispose(); }
    
    void dispose();
    
    /**
     * Initializes the board
     *
     * @param width     The board width (num tiles)
     * @param height    The board height (num tiles)
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(int width, int height);
    
    // Allocates this board for a shared pointer
    static std::shared_ptr<BoardModel> alloc(int width, int height) {
        std::shared_ptr<BoardModel> board = std::make_shared<BoardModel>();
        return (board->init(width, height) ? board : nullptr);
    }

	int gameHeight;
	int gameWidth;

	bool offsetRow;
	bool offsetCol;
    float offset;

	std::shared_ptr<cugl::Texture> tileTexture;

	// Returns the value at the give (x, y) coordinate
	TileModel getTile(int x, int y) const;

	// Returns the ally pawn at index i of _allies
	PlayerPawnModel getAlly(int i) const;

	// Returns the enemy pawn at index i of _enemies
	PlayerPawnModel getEnemy(int i) const;

	// Set the tile at the given (x, y) coordinate
	void setTile(int x, int y, TileModel t);

	// Returns the number of allies
	int getNumAllies() const { return _numAllies; }

	//Returns the number of enemies
	int getNumEnemies() const { return _numEnemies; }

	//Returns the number of colors
	int getNumColors() const { return _numColors; }

	//Returns the side height
	int getHeight() const { return _height; }

	//Returns the side width
	int getWidth() const { return _width; }

	// Place ally at index i of _allies on location (x, y)
	void placeAlly(int x, int y, int i);

	// Place enemy at index i of _enemies on location (x, y)
	void placeEnemy(int x, int y, int i);

	// Remove ally at index i
	void removeAlly(int i);

	// Remove enemy at index i
	void removeEnemy(int i);

	// Return true if a match is found (and replace those matches, damaging pawns on matches), otherwise false
	bool checkForMatches();

	// Generates a new board into the _tiles variable
	void generateNewBoard();

	// Offset view of row (not model)
	void setOffsetRow(float value);

	// Offset view of col (not model)
	void setOffsetCol(float value);

	// Offset reset
	void offsetReset();
    
    // Deselect _selectedTile
    void deselectTile();

	// Slide row [y] by [offset]
	void slideRow(int y, int offset);

	// Slide column [x] by [offset]
	void slideCol(int x, int offset);
    
    /**
     * Select tile at screen position [position]
     *
     * @param position on screen
     *
     * @return true if there is a tile, false otherwise
     */
    bool selectTileAtPosition(cugl::Vec2 position);
    
    // Convert grid (x, y) to screen coordinates
//    cugl::Vec2 gridToScreen(int x, int y);
    
    // Convert screen coordinates to grid (x, y)
//    std::tuple<int, int> screenToGrid(cugl::Vec2 position);

	// Draws tiles and pawns
	void draw(const std::shared_ptr<cugl::SpriteBatch>& batch);

	/**
	* Returns a string representation of the board for debugging purposes.
	*
	* @return a string representation of this vector for debuggging purposes.
	*/
	std::string toString() const;

	/** Cast from TileBoard to a string. */
	operator std::string() const { return toString(); }

};

#endif /* __Board_Model_H__ */
