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
#include "EnemyPawnModel.h"
#include <set>
#include <vector>


#pragma mark -
#pragma mark Board Model

/** Class of the board model*/
class BoardModel {
protected:
    //Temp variables for cool animation
    std::set<int>::iterator totalIter;
    std::set<int> totalReplaceTiles;
    int animationCounter = -1;

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
	std::vector<int> _colorLookup;
    // Reset color lookup
    void resetRandom();
    int randomColor();

	// Replaces a tile at a location in the array with a new value
	void replaceTile(int tileLocation);

	// Slide row or column by [offset]
	void slide(bool row, int k, int offset);

	// Slide pawns in row or column [k] by [offset]
	void slidePawns(bool row, int k, int offset);
    

#pragma mark -
#pragma mark Protected Variables
    
    /** Reference to node in SceneGraph */
    std::shared_ptr<cugl::Node> _node;
    
    /** The asset manager for play mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** Display settings */
    float _boardPadding;
    float _tilePadding;
    float _tilePaddingX;
    float _tilePaddingY;
    
    /** Cell size in the board */
    cugl::Size _cellSize;
    
    /** Size of vertical (column) side of the square board */
    int _height;
    
    /** Size of horizontal (row) side of the square board */
    int _width;
    
    // Number of colors of tiles available to the board
    int _numColors;
    
    /** Array of all tiles on the board, array index can be translated to coordinates and vice versa */
    std::vector<std::shared_ptr<TileModel>> _tiles;
    
    /** Vector storing all allies on the board */
    std::vector<std::shared_ptr<PlayerPawnModel>> _allies;
    
    /** Vector storing all enemies on the board */
    std::vector<std::shared_ptr<EnemyPawnModel>> _enemies;
    
    /** Set storing all tiles added to the board (store for animation) */
    std::set<std::shared_ptr<TileModel>> _addedTiles;
    
    /** Set storing all allies added to the board (store for animation) */
    std::set<std::shared_ptr<PlayerPawnModel>> _addedAllies;
    
    /** Set storing all enemies added to the board (store for animation) */
    std::set<std::shared_ptr<EnemyPawnModel>> _addedEnemies;
    
    /** Set storing all tiles removed from the board (store for animation) */
    std::set<std::shared_ptr<TileModel>> _removedTiles;
    
    /** Set storing all allies removed from the board (store for animation) */
    std::set<std::shared_ptr<PlayerPawnModel>> _removedAllies;
    
    /** Set storing all enemies removed from the board (store for animation) */
    std::set<std::shared_ptr<EnemyPawnModel>> _removedEnemies;
    
    
#pragma mark -
#pragma mark Initialization Helpers
    /** Setup board node & display properties */
    bool setupBoardNode(int width , int height, int colors, cugl::Size dimen);
    
    /**
     * Generate tiles
     * Assume board properties are already setup
     */
    bool generateTiles(int seed);
    
    /** Change (x,y) to NULL tile */
    void setNullTile(int x, int y);
    
    /** Setup allies from Json */
    bool setupAlliesFromJson(std::shared_ptr<cugl::JsonValue>& json);
    
    /** Setup enemies from Json */
    bool setupEnemiesFromJson(std::shared_ptr<cugl::JsonValue>& json);
    
    
#pragma mark -
#pragma mark Index Transformation Functions
    /**
     * Convert (x, y) coordinate to array index. (0, 0) is the bottom left corner
     * Example for sideSize = 3
     *  6 7 8     (0,2) (1,2) (2,2)
     *  3 4 5     (0,1) (1,1) (2,1)
     *  0 1 2     (0,0) (1,0) (2,0)
     */
    int indexOfCoordinate(int x, int y) const;
    
    // Convert array index to x
    int xOfIndex(int i) const;
    
    // Convert array index to y
    int yOfIndex(int i) const;
    
    

public:
#pragma mark -
#pragma mark Constructors/Destructors
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
    
    bool init(int width, int height, int seed, int colors, std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen);
    
    bool init(int width, int height, int colors, int allies, int enemies, bool placePawn);
    
    bool init(int width, int height, int colors, int allies, int enemies, bool placePawn, std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen);
    
    bool init(std::shared_ptr<cugl::JsonValue>& json, std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen);
    
#pragma mark -
#pragma mark Static Constructors
    /** Allocates board for a shared pointer */
    static std::shared_ptr<BoardModel> alloc(int width, int height) {
        std::shared_ptr<BoardModel> board = std::make_shared<BoardModel>();
        return (board->init(width, height) ? board : nullptr);
    }
    
    /** Allocates board for a shared pointer */
    static std::shared_ptr<BoardModel> alloc(int width, int height, int seed, int colors, std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen) {
        std::shared_ptr<BoardModel> board = std::make_shared<BoardModel>();
        return (board->init(width, height, seed, colors, assets, dimen) ? board : nullptr);
    }

    /** Allocates board for shared pointer */
	static std::shared_ptr<BoardModel> alloc(int width, int height, int colors, int allies, int enemies, bool placePawn) {
		std::shared_ptr<BoardModel> board = std::make_shared<BoardModel>();
		return (board->init(width, height, colors, allies, enemies, placePawn) ? board : nullptr);
	}
    
    /** Allocates board for shared pointer */
    static std::shared_ptr<BoardModel> alloc(int width, int height, int colors, int allies, int enemies, bool placePawn, std::shared_ptr<cugl::AssetManager> assets, cugl::Size dimen) {
        std::shared_ptr<BoardModel> board = std::make_shared<BoardModel>();
        return (board->init(width, height, colors, allies, enemies, placePawn, assets, dimen) ? board : nullptr);
    }
    
    /** Allocates board for shared pointer */
    static std::shared_ptr<BoardModel> alloc(std::shared_ptr<cugl::JsonValue>& json, std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen) {
        std::shared_ptr<BoardModel> board = std::make_shared<BoardModel>();
        return (board->init(json, assets, dimen) ? board : nullptr);
    }

    
#pragma mark -
#pragma mark Accessors/Mutators
	int gameHeight;
	int gameWidth;

	bool offsetRow;
	bool offsetCol;
    float offset;

	std::shared_ptr<cugl::Texture> tileTexture;
    std::shared_ptr<cugl::Texture> playerTexture;
    std::shared_ptr<cugl::Texture> tile1Texture;
    std::shared_ptr<cugl::Texture> tile2Texture;
    
	// Returns the value at the give (x, y) coordinate
    std::shared_ptr<TileModel>& getTile(int x, int y);

	// Returns the ally pawn at index i of _allies
    std::shared_ptr<PlayerPawnModel> getAlly(int i);
    
    // Returns the ally pawn at (x, y)
    std::shared_ptr<PlayerPawnModel> getAlly(int x, int y);

	// Returns the enemy pawn at index i of _enemies
    std::shared_ptr<EnemyPawnModel> getEnemy(int i);
    
    // Returns the enemy pawn at (x, y) or nullptr
    std::shared_ptr<EnemyPawnModel> getEnemy(int x, int y);

    // Returns the tiles
    std::vector<std::shared_ptr<TileModel>>& getTiles() { return _tiles; }
    
	// Returns the allies
    std::vector<std::shared_ptr<PlayerPawnModel>>& getAllies() { return _allies; }

	// Returns the enemies
    std::vector<std::shared_ptr<EnemyPawnModel>>& getEnemies() { return _enemies; }
    
    /** Return added/removed sets */
    std::set<std::shared_ptr<TileModel>>& getAddedTiles() { return _addedTiles; }
    std::set<std::shared_ptr<PlayerPawnModel>>& getAddedAllies() { return _addedAllies; }
    std::set<std::shared_ptr<EnemyPawnModel>>& getAddedEnemies() { return _addedEnemies; }
    std::set<std::shared_ptr<TileModel>>& getRemovedTiles() { return _removedTiles; }
    std::set<std::shared_ptr<PlayerPawnModel>>& getRemovedAllies() { return _removedAllies; }
    std::set<std::shared_ptr<EnemyPawnModel>>& getRemovedEnemies() { return _removedEnemies; }
    
    /** Clear added/removed sets */
    void clearAddedTiles() { _addedTiles.clear(); }
    void clearAddedAllies() { _addedAllies.clear(); }
    void clearAddedEnemies() { _addedEnemies.clear(); }
    void clearRemovedTiles() { _removedTiles.clear(); }
    void clearRemovedAllies() { _removedAllies.clear(); }
    void clearRemovedEnemies() { _removedEnemies.clear(); }

	// Set the tile at the given (x, y) coordinate
    void setTile(int x, int y, std::shared_ptr<TileModel> t);

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
    
    // Returns cell length
    float getCellLength();

	// Place ally at index i of _allies on location (x, y)
	void placeAlly(int x, int y, int i);

	// Place enemy at index i of _enemies on location (x, y)
	void placeEnemy(int x, int y, int i);

	// Moves an enemy e to a different location
	void moveEnemy(int x, int y, int enemyIdx);

	// Remove ally at index i
	void removeAlly(int i);

	// Remove enemy at index i
	void removeEnemy(int i);

#pragma mark -
#pragma mark Logic
	// Return true if a match is found (and replace those matches, damaging pawns on matches), otherwise false
	bool checkForMatches(bool removeEnemies=true);

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
    
    // Slide row/col by [offset]
    void slide(int offset);
    
#pragma mark -
#pragma mark Drawing/Animation
    /** Actions */
    // Tile
    std::shared_ptr<cugl::Animate> tileAddAction = cugl::Animate::alloc(TILE_IMG_APPEAR_START, TILE_IMG_APPEAR_END, TILE_IMG_APPEAR_TIME);
    std::shared_ptr<cugl::Animate> tileRemoveAction = cugl::Animate::alloc(TILE_IMG_DISAPPEAR_START, TILE_IMG_DISAPPEAR_END, TILE_IMG_DISAPPEAR_TIME);
    // Enemy
    std::shared_ptr<cugl::FadeIn> enemyAddAction = cugl::FadeIn::alloc(TILE_IMG_APPEAR_TIME);
    std::shared_ptr<cugl::FadeOut> enemyRemoveAction = cugl::FadeOut::alloc(TILE_IMG_DISAPPEAR_TIME);
    // Ally
    std::shared_ptr<cugl::FadeIn> allyAddAction = cugl::FadeIn::alloc(TILE_IMG_APPEAR_TIME);
    std::shared_ptr<cugl::FadeOut> allyRemoveAction = cugl::FadeOut::alloc(TILE_IMG_DISAPPEAR_TIME);
    // Mika
    std::shared_ptr<cugl::Animate> mikaIdleAction = cugl::Animate::alloc(PLAYER_IMG_IDLE_START, PLAYER_IMG_IDLE_END, PLAYER_IMG_IDLE_TIME);
    std::shared_ptr<cugl::Animate> mikaBeginAttackAction = cugl::Animate::alloc(PLAYER_IMG_BEGIN_ATTACK_START, PLAYER_IMG_BEGIN_ATTACK_END, PLAYER_IMG_BEGIN_ATTACK_TIME);
    std::shared_ptr<cugl::Animate> mikaAttackingAction = cugl::Animate::alloc(PLAYER_IMG_ATTACKING_START, PLAYER_IMG_ATTACKING_END, PLAYER_IMG_ATTACKING_TIME);
    
    /** Get board node */
    std::shared_ptr<cugl::Node>& getNode() { return _node; }
    
    /** Update nodes positions (can choose to only update position or z) */
    void updateNodes(bool position=true, bool z=true);
    
    /**
     * Select tile at screen position [position]
     *
     * @param position on screen
     *
     * @return true if there is a tile, false otherwise
     */
    bool selectTileAtPosition(cugl::Vec2 position);
    
//     Convert grid (x, y) to screen coordinates
    cugl::Rect gridToScreen(int x, int y);
    
    // Convert screen coordinates to grid (x, y)
    std::tuple<int, int> screenToGrid(cugl::Vec2 position);
    
    // Convert screen length to grid length
    int lengthToCells(float length, bool row=false);
    
    // Apply padding, offset, and wrap to return tile bounds
    cugl::Rect calculateDrawBounds(int x, int y);
    
    /**
     * Calculate z-axis coordinate given (x,y) cell in grid.
     *   Tiles start at 10s through 10*height
     *   Pawns at 10*height + 5 through 20*height + 5
     */
    int calculateDrawZ(int x, int y, bool tile);

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
