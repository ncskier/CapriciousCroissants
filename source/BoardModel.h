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
#include <set>
#include <vector>
#include "EntityManager.h"

#define ENEMY_FRAME_RIGHT  0
#define ENEMY_FRAME_DOWN  16
#define ENEMY_FRAME_UP    32
#define ENEMY_FRAME_LEFT 48
#define ENEMY_IDLE_TIME 1.0f
#define ENEMY_WALK_TIME 1.0f
#define ENEMY_ATTACK_TIME 1.0f

#pragma mark -
#pragma mark Board Model

/** Class of the board model*/
class BoardModel {
protected:
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

	/** The entity manager for play mode. */
	std::shared_ptr<EntityManager> _entityManager;
    
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
	std::vector<size_t> enemiesEntityIds;
    
    /** Set storing all tiles added to the board (store for animation) */
    std::set<std::shared_ptr<TileModel>> _addedTiles;
    
    /** Set storing all allies added to the board (store for animation) */
    std::set<std::shared_ptr<PlayerPawnModel>> _addedAllies;
    
    /** Set storing all enemies added to the board (store for animation) */
    std::set<size_t> _addedEnemies;
    
    /** Set storing all tiles removed from the board (store for animation) */
    std::set<std::shared_ptr<TileModel>> _removedTiles;
    
    /** Set storing all allies removed from the board (store for animation) */
    std::set<std::shared_ptr<PlayerPawnModel>> _removedAllies;
    
    /** Set storing all enemies removed from the board (store for animation) */
    std::set<size_t> _removedEnemies;
    
	/** Set storing all attacking enemies attacking on the board (store for animation) */
	std::set<size_t> _attackingEnemies;

    
#pragma mark -
#pragma mark Initialization Helpers
    /** Setup board node & display properties */
    bool setupBoardNode(int width , int height, int colors, cugl::Size dimen);
    
    /**
     * Generate tiles
     * Assume board properties are already setup
     */
    bool generateTiles(int seed);
	bool generateTiles(std::shared_ptr<cugl::JsonValue> &tileColors);
    
    /** Change (x,y) to NULL tile */
    void setNullTile(int x, int y);
    
    /** Setup allies from Json */
    bool setupAlliesFromJson(std::shared_ptr<cugl::JsonValue>& json);
    
    /** Setup enemies from Json */
    bool setupEnemiesFromJson(std::shared_ptr<cugl::JsonValue>& json, std::shared_ptr<cugl::ActionManager>& actions);
    
public:    
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
    
    /** Initialized the board */
    bool init(std::shared_ptr<cugl::JsonValue>& json, std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen, std::shared_ptr<EntityManager>& entityManager, std::shared_ptr<cugl::ActionManager>& actions);
    

	std::shared_ptr<cugl::AssetManager>& getAssets() { return _assets; }

#pragma mark -
#pragma mark Static Constructors
    /** Allocates board for shared pointer */
    static std::shared_ptr<BoardModel> alloc(std::shared_ptr<cugl::JsonValue>& json, std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen, std::shared_ptr<EntityManager>& entityManager, std::shared_ptr<cugl::ActionManager>& actions) {
        std::shared_ptr<BoardModel> board = std::make_shared<BoardModel>();
        return (board->init(json, assets, dimen, entityManager, actions) ? board : nullptr);
    }

    
#pragma mark -
#pragma mark Accessors/Mutators
	int gameHeight;
	int gameWidth;

	bool offsetRow;
	bool offsetCol;
    float offset;

	bool lose = false;
    
    // Max allies (number of starting allies)
    int maxAllies;

	bool requestedRow;

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
    size_t getEnemy(int i);
    
    // Returns the enemy pawn at (x, y) or 0
    size_t getEnemy(int x, int y);

    // Returns the tiles
    std::vector<std::shared_ptr<TileModel>>& getTiles() { return _tiles; }
    
	// Returns the allies
    std::vector<std::shared_ptr<PlayerPawnModel>>& getAllies() { return _allies; }

	// Returns the enemies
    std::vector<size_t>& getEnemies() { return enemiesEntityIds; }

	//Returns selected tile
	int getSelectedTile() { return _selectedTile; }
    
    /** Return added/removed sets */
    std::set<std::shared_ptr<TileModel>>& getAddedTiles() { return _addedTiles; }
    std::set<std::shared_ptr<PlayerPawnModel>>& getAddedAllies() { return _addedAllies; }
    std::set<size_t>& getAddedEnemies() { return _addedEnemies; }
    std::set<std::shared_ptr<TileModel>>& getRemovedTiles() { return _removedTiles; }
    std::set<std::shared_ptr<PlayerPawnModel>>& getRemovedAllies() { return _removedAllies; }
    std::set<size_t>& getRemovedEnemies() { return _removedEnemies; }
	std::set<size_t>& getAttackingEnemies() { return _attackingEnemies; }

    
    /** Clear added/removed sets */
    void clearAddedTiles() { _addedTiles.clear(); }
    void clearAddedAllies() { _addedAllies.clear(); }
    void clearAddedEnemies() { _addedEnemies.clear(); }
    void clearRemovedTiles() { _removedTiles.clear(); }
    void clearRemovedAllies() { _removedAllies.clear(); }
    void clearRemovedEnemies() { _removedEnemies.clear(); }
	void clearAttackingEnemies() { _attackingEnemies.clear(); }

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

	// Remove ally at index i
	void removeAlly(int i);

	// Remove enemy at index i
	void removeEnemy(int i);


	// Add attacking enemy at index i
	void insertAttackingEnemy(EntityId entity);

#pragma mark -
#pragma mark Logic
	// Return true if a match is found (and replace those matches, damaging pawns on matches), otherwise false
	bool checkForMatches(bool removeEnemies=true);

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
//    std::shared_ptr<cugl::Animate> tileRemoveAction = cugl::Animate::alloc(TILE_IMG_DISAPPEAR_START, TILE_IMG_DISAPPEAR_END, TILE_IMG_DISAPPEAR_TIME);
    std::shared_ptr<cugl::Animate> tileRemoveAction = cugl::Animate::alloc({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, {0.035f,0.035f,0.035f,0.035f,0.035f,0.035f,0.4f,0.025f,0.025f,0.025f,0.025f,0.025f,0.025f,0.025f,0.025f,0.025f});
    std::shared_ptr<cugl::Animate> tileDeathAction = cugl::Animate::alloc(TILE_DEATH_START, TILE_DEATH_END, TILE_DEATH_TIME);
    // Enemy
    std::shared_ptr<cugl::FadeIn> enemyAddAction = cugl::FadeIn::alloc(TILE_IMG_APPEAR_TIME);
    std::shared_ptr<cugl::FadeOut> enemyRemoveAction = cugl::FadeOut::alloc(TILE_IMG_DISAPPEAR_TIME);
	std::shared_ptr<cugl::Animate> enemyIdleLeftAction = cugl::Animate::alloc(ENEMY_FRAME_LEFT, ENEMY_FRAME_LEFT + 15, ENEMY_IDLE_TIME);
	std::shared_ptr<cugl::Animate> enemyIdleRightAction = cugl::Animate::alloc(ENEMY_FRAME_RIGHT, ENEMY_FRAME_RIGHT + 15, ENEMY_IDLE_TIME);
	std::shared_ptr<cugl::Animate> enemyIdleUpAction = cugl::Animate::alloc(ENEMY_FRAME_UP, ENEMY_FRAME_UP + 15, ENEMY_IDLE_TIME);
	std::shared_ptr<cugl::Animate> enemyIdleDownAction = cugl::Animate::alloc(ENEMY_FRAME_DOWN, ENEMY_FRAME_DOWN + 15, ENEMY_IDLE_TIME);
	std::shared_ptr<cugl::Animate> enemyMoveLeftAction = cugl::Animate::alloc(64 + ENEMY_FRAME_LEFT, 64 + ENEMY_FRAME_LEFT + 15, ENEMY_WALK_TIME);
	std::shared_ptr<cugl::Animate> enemyMoveRightAction = cugl::Animate::alloc(64 + ENEMY_FRAME_RIGHT, 64 + ENEMY_FRAME_RIGHT + 15, ENEMY_WALK_TIME);
	std::shared_ptr<cugl::Animate> enemyMoveUpAction = cugl::Animate::alloc(64 + ENEMY_FRAME_UP, 64 + ENEMY_FRAME_UP + 15, ENEMY_WALK_TIME);
	std::shared_ptr<cugl::Animate> enemyMoveDownAction = cugl::Animate::alloc(64 + ENEMY_FRAME_DOWN, 64 + ENEMY_FRAME_DOWN + 15, ENEMY_WALK_TIME);
	std::shared_ptr<cugl::Animate> enemyAttackLeftAction = cugl::Animate::alloc(128 + ENEMY_FRAME_LEFT, 128 + ENEMY_FRAME_LEFT + 15, ENEMY_ATTACK_TIME);
	std::shared_ptr<cugl::Animate> enemyAttackRightAction = cugl::Animate::alloc(128 + ENEMY_FRAME_RIGHT, 128 + ENEMY_FRAME_RIGHT + 15, ENEMY_ATTACK_TIME);
	std::shared_ptr<cugl::Animate> enemyAttackUpAction = cugl::Animate::alloc(128 + ENEMY_FRAME_UP, 128 + ENEMY_FRAME_UP + 15, ENEMY_ATTACK_TIME);
	std::shared_ptr<cugl::Animate> enemyAttackDownAction = cugl::Animate::alloc(128 + ENEMY_FRAME_DOWN, 128 + ENEMY_FRAME_DOWN + 15, ENEMY_ATTACK_TIME);
    // Ally
    std::shared_ptr<cugl::FadeIn> allyAddAction = cugl::FadeIn::alloc(TILE_IMG_APPEAR_TIME);
    std::shared_ptr<cugl::FadeOut> allyFadeOutAction = cugl::FadeOut::alloc(0.2f);
    std::shared_ptr<cugl::Animate> allyDeathAction = cugl::Animate::alloc(ALLY_DEATH_IMG_START, ALLY_DEATH_IMG_END, ALLY_DEATH_IMG_TIME);
	std::shared_ptr<cugl::Animate> allyIdleAction = cugl::Animate::alloc(ALLY_IDLE_IMG_START, ALLY_IDLE_IMG_END, ALLY_IDLE_IMG_TIME);
    std::shared_ptr<cugl::FadeOut> allyRemoveAction = cugl::FadeOut::alloc(TILE_IMG_DISAPPEAR_TIME);
    // Mika
    std::shared_ptr<cugl::Animate> mikaIdleAction = cugl::Animate::alloc(PLAYER_IMG_IDLE_START, PLAYER_IMG_IDLE_END, PLAYER_IMG_IDLE_TIME);
    std::shared_ptr<cugl::Animate> mikaBeginAttackAction = cugl::Animate::alloc(PLAYER_IMG_BEGIN_ATTACK_START, PLAYER_IMG_BEGIN_ATTACK_END, PLAYER_IMG_BEGIN_ATTACK_TIME);
    std::shared_ptr<cugl::Animate> mikaAttackingAction = cugl::Animate::alloc(PLAYER_IMG_ATTACKING_START, PLAYER_IMG_ATTACKING_END, PLAYER_IMG_ATTACKING_TIME);
    std::shared_ptr<cugl::Animate> mikaLoseAction = cugl::Animate::alloc(PLAYER_END_LOSE_START, PLAYER_END_LOSE_END, PLAYER_END_LOSE_TIME);
    std::shared_ptr<cugl::Animate> mikaWinAction = cugl::Animate::alloc(PLAYER_END_WIN_START, PLAYER_END_WIN_END, PLAYER_END_WIN_TIME);
	//Projectile
	std::shared_ptr<cugl::Animate> projectileAction = cugl::Animate::alloc(0, 10, 0.5);



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

	//     Convert grid (x, y) to screen coordinates
	cugl::Vec2 gridToScreenV(int x, int y);
    

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
