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
    //Size of one side of the square board
    int _sideSize;
    
    //Number of colors of tiles available to the board
    int _numColors;
    
    //Number of pawns
    int _numPawns;

	std::vector<cugl::Color4> colorLookup;

	//For now just treat tiles as an array of ints, where an int is the color of the tile
    int *_tiles;
//    std::shared_ptr<int[]> _tiles;

	//Treat pawns as an array of a vector 2 for locations of the pawns
    cugl::Vec2 *_pawns;
//    std::shared_ptr<cugl::Vec2[]> _pawns;

	//Replaces a tile at a location in the array with a new value
	void replaceTile(int tileLocation);
    
    //Convert (x, y) coordinate to array index. (0, 0) is the upper left corner
    int indexOfCoordinate(int x, int y) const;
    
    //Slide row or column by [offset]
    void slide(bool row, int k, int offset);
    
    // Slide pawns in row or column [k] by [offset]
    void slidePawns(bool row, int k, int offset);
	
public:
	TileBoard();
	~TileBoard();

	int gameHeight;
	int gameWidth;
    
    int offsetRowIdx;
    int offsetColIdx;
    float offsetRowValue;
    float offsetColValue;
	
	std::shared_ptr<cugl::Texture> tileTexture;
    
    //Returns the value at the give (x, y) coordinate
    int get(int x, int y) const;
    
    //Set the value at the given (x, y) coordinate
    void set(int x, int y, int value);
    
    //Returns the number of pawns
    int getNumPawns() const { return _numPawns; }
    
    //Returns the number of colors
    int getNumColors() const { return _numColors; }
    
    //Returns the side size
    int getSideSize() const { return _sideSize; }
	
    // Place pawn at index i on location (x, y)
	void placePawn(int x, int y, int i);

    // Remove pawn at index i
    void removePawn(int i);

	//Return true if a match is found (and replace those matches, damaging pawns on matches), otherwise false
	bool checkForMatches();

	static std::shared_ptr<TileBoard> alloc();

	//Generates a new board into the _tiles variable
	void generateNewBoard();
    
    //Offset view of row (not model)
    void offsetRow(int idx, float value);
    
    //Offset view of col (not model)
    void offsetCol(int idx, float value);
    
    //Offset reset
    void offsetReset();
    
    //Slide row [y] by [offset]
    void slideRow(int y, int offset);
    
    //Slide column [x] by [offset]
    void slideCol(int x, int offset);

	//Draws tiles and pawns
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

#endif
