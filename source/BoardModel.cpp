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
#include <vector>

#include "BoardModel.h"
#include <cugl/base/CUBase.h>

using namespace cugl;


#pragma mark -
#pragma mark Constructors/Destructors

// Initialize board attributes and call generateNewBoard to generate _tiles, _allies, and _enemies
BoardModel::BoardModel() :
_height(5),
_width(5),
_numColors(5),
_numAllies(1),
_numEnemies(4),
_selectedTile(-1),
_placeAllies(false),
_boardPadding(45.0f),
_tilePadding(0.0f),
_tilePaddingX(0.0f),
_tilePaddingY(0.0f),
offsetRow(false),
offsetCol(false),
offset(0.0f) {
}

/**
 * Initializes the board
 *
 * @param width     The board width (num tiles)
 * @param height    The board height (num tiles)
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool BoardModel::init(int width, int height) {
    return init(width, height, _numColors, _numAllies, _numEnemies, _placeAllies);
}

bool BoardModel::init(std::shared_ptr<cugl::JsonValue> &json, std::shared_ptr<AssetManager>& assets, Size dimen) {
    CULog("Init JSON");
    
    // Set asset manager
    _assets = assets;
    
    // Get Board Node info (width, height, colors, seed)
    int width = json->get("size")->get("width")->asInt();
    int height = json->get("size")->get("height")->asInt();
    CULog("(%d, %d)", width, height);
    int colors = json->get("colors")->asInt();
    CULog("colors: %d", colors);
    
    // Setup Board Node
    if (!setupBoardNode(width, height, colors, dimen)) {
        return false;
    }
    
    // Setup Tiles from json
    int seed = (int)time(NULL);
    if (json->get("seed") != nullptr) {
        seed = json->get("seed")->asInt();
    }
    CULog("seed: %d", seed);
    if (!generateTiles(seed)) {
        return false;
    }
    
    // Setup Allies from json
    
    // Setup Enemies from json
    
    return true;
}

bool BoardModel::init(int width, int height, int seed, int colors, std::shared_ptr<AssetManager>& assets, Size dimen) {
    CULog("New init");
    // Set asset manager
    _assets = assets;
    
    // Setup Board Node
    if (!setupBoardNode(width, height, colors, dimen)) {
        return false;
    }
    
    // Generate tiles
    if (!generateTiles(seed)) {
        return false;
    }
    
    // Add allies
    _numAllies = 1;
    int x;
    int y;
    x = 2;
    y = 3;
    std::shared_ptr<PlayerPawnModel> ally1 = PlayerPawnModel::alloc(x, y, calculateDrawBounds(x, y), _assets);
    _allies.push_back(ally1);
    _addedAllies.insert(ally1);
    
    // Add enemies
    _numEnemies = 5;
    bool smart;
    EnemyPawnModel::Direction direction;
    // Dumb enemy (1,4) SOUTH
    x = 1;
    y = 4;
    smart = false;
    direction = EnemyPawnModel::Direction::SOUTH;
    std::shared_ptr<EnemyPawnModel> enemy1 = EnemyPawnModel::alloc(x, y, direction, smart, calculateDrawBounds(x, y), _assets);
    _enemies.push_back(enemy1);
    _addedEnemies.insert(enemy1);
    // Dumb enemy (4,1) WEST
    x = 4;
    y = 1;
    smart = false;
    direction = EnemyPawnModel::Direction::WEST;
    std::shared_ptr<EnemyPawnModel> enemy2 = EnemyPawnModel::alloc(x, y, direction, smart, calculateDrawBounds(x, y), _assets);
    _enemies.push_back(enemy2);
    _addedEnemies.insert(enemy2);
    // Smart enemy (0,4)
    x = 0;
    y = 4;
    smart = true;
    std::shared_ptr<EnemyPawnModel> enemy3 = EnemyPawnModel::alloc(x, y, direction, smart, calculateDrawBounds(x, y), _assets);
    _enemies.push_back(enemy3);
    _addedEnemies.insert(enemy3);
    // Smart enemy (3,4)
    x = 3;
    y = 4;
    smart = true;
    std::shared_ptr<EnemyPawnModel> enemy4 = EnemyPawnModel::alloc(x, y, direction, smart, calculateDrawBounds(x, y), _assets);
    _enemies.push_back(enemy4);
    _addedEnemies.insert(enemy4);
    // Smart enemy (2,1)
    x = 2;
    y = 1;
    smart = true;
    std::shared_ptr<EnemyPawnModel> enemy5 = EnemyPawnModel::alloc(x, y, direction, smart, calculateDrawBounds(x, y), _assets);
    _enemies.push_back(enemy5);
    _addedEnemies.insert(enemy5);
    
    
    return true;
}

bool BoardModel::init(int width, int height, int colors, int allies, int enemies, bool placePawn) {
    CULog("Old init");
    _height = height;
    _width = width;
    _numColors = colors;
    _numAllies = allies;
    _numEnemies = enemies;
    _placeAllies = placePawn;
    
    // Set cell size
    float cellLength = getCellLength();
    _cellSize = Size(cellLength, cellLength*0.85f);
    // Set tile padding
    _tilePaddingX = -cellLength*0.04f;
    
    srand((int)time(NULL));
    generateNewBoard();
    while (checkForMatches());
    srand((int)time(NULL));
    return true;
}

bool BoardModel::init(int width, int height, int colors, int allies, int enemies, bool placePawn, std::shared_ptr<cugl::AssetManager>& assets, Size dimen) {
    _assets = assets;
    
    // Setup Board Node
    gameWidth = dimen.width;
    gameHeight = dimen.height;
    _node = Node::alloc();
    float gameLength = (dimen.width > dimen.height) ? dimen.height : dimen.width;
    _node->setContentSize(gameLength, gameLength);
//    _node->setContentSize(dimen);
    _node->setAnchor(Vec2::ANCHOR_CENTER);
    _node->setPosition(dimen.width*0.5f, dimen.height*0.5f);
//    _node->setAnchor(Vec2::ZERO);
    
    // Initialize everything else
    return init(width, height, colors, allies, enemies, placePawn);
}

// Destroy any values needed to be deleted for this class
void BoardModel::dispose() {
}


#pragma mark -
#pragma mark Initialization Helpers
/** Setup board node & display properties */
bool BoardModel::setupBoardNode(int height, int width, int colors, Size dimen) {
    // Setup Board properties
    _height = height;
    _width = width;
    _numColors = colors;
    
    // Setup Board Node
    gameWidth = dimen.width;
    gameHeight = dimen.height;
    _node = Node::alloc();
    float gameLength = (dimen.width > dimen.height) ? dimen.height : dimen.width;
    _node->setContentSize(gameLength, gameLength);
    _node->setAnchor(Vec2::ANCHOR_CENTER);
    _node->setPosition(dimen.width*0.5f, dimen.height*0.5f);
    
    // Set cell size
    float cellLength = getCellLength();
    _cellSize = Size(cellLength, cellLength*0.85f);
    // Set tile padding
    _tilePaddingX = -cellLength*0.04f;
    
    return true;
}

/** Generate tiles
 *  Assume board properties are already setup
 */
bool BoardModel::generateTiles(int seed) {
    srand(seed);
    // Setup Tiles
    _tiles.reserve(_width * _height);
    int color;
    for (int i = 0; i < _height*_width; i++) {
        color = randomColor();
        Rect bounds = calculateDrawBounds(xOfIndex(i), yOfIndex(i));
        std::shared_ptr<TileModel> tile = TileModel::alloc(color, bounds, _assets);
        _tiles.push_back(tile);
        _addedTiles.insert(tile);
    }
    while (checkForMatches(false));
    
    return true;
}


#pragma mark -
#pragma mark Index Transformation Functions

/**
 * Convert (x, y) coordinate to array index. (0, 0) is the bottom left corner
 * Example for sideSize = 3
 *  6 7 8     (0,2) (1,2) (2,2)
 *  3 4 5     (0,1) (1,1) (2,1)
 *  0 1 2     (0,0) (1,0) (2,0)
 */
int BoardModel::indexOfCoordinate(int x, int y) const {
	return x + (y*_width);
}

// Convert array index to x
int BoardModel::xOfIndex(int i) const {
    return i % _width;
}

// Convert array index to y
int BoardModel::yOfIndex(int i) const {
    return i / _width;
}


#pragma mark -
#pragma mark Accessors/Mutators
// Get random color
int BoardModel::randomColor() {
    if (_colorLookup.empty()) {
        resetRandom();
    }
    int i = rand() % _colorLookup.size();
    int color = _colorLookup[i];
    _colorLookup.erase(_colorLookup.begin()+i);
    return color;
}

// Reset color lookup
void BoardModel::resetRandom() {
    _colorLookup.clear();
    for (int i = 0; i < _numColors; i++) {
        _colorLookup.push_back(i);
    }
}

// Returns the value at the give (x, y) coordinate
std::shared_ptr<TileModel>& BoardModel::getTile(int x, int y) {
	return _tiles[indexOfCoordinate(x, y)];
}

// Returns the ally pawn at index i of _allies
std::shared_ptr<PlayerPawnModel> BoardModel::getAlly(int i) {
	return _allies[i];
}

// Returns the ally pawn at (x, y)
std::shared_ptr<PlayerPawnModel> BoardModel::getAlly(int x, int y) {
    for (int i = 0; i < _numAllies; i++) {
        std::shared_ptr<PlayerPawnModel>& ally = _allies[i];
        if (ally->getX() == x && ally->getY() == y) {
            return ally;
        }
    }
    return nullptr;
}

// Returns the enemy pawn at index i of _enemies
std::shared_ptr<EnemyPawnModel> BoardModel::getEnemy(int i) {
	return _enemies[i];
}

// Returns the enemy pawn at (x, y)
std::shared_ptr<EnemyPawnModel> BoardModel::getEnemy(int x, int y) {
    for (int i = 0; i < _numEnemies; i++) {
        std::shared_ptr<EnemyPawnModel>& enemy = _enemies[i];
        if (enemy->getX() == x && enemy->getY() == y) {
            return enemy;
        }
    }
    return nullptr;
}

// Set the value at the given (x, y) coordinate
void BoardModel::setTile(int x, int y, std::shared_ptr<TileModel> t) {
	_tiles[indexOfCoordinate(x, y)] = t;
}

// Place ally at index i of _allies on location (x, y)
void BoardModel::placeAlly(int x, int y, int i) {
	_allies[i]->setXY(x, y);
}
	
// Place enemy at index i of _enemies on location (x, y)
void BoardModel::placeEnemy(int x, int y, int i) {
	_enemies[i]->setXY(x, y);
}

void BoardModel::moveEnemy(int dx, int dy, int enemyIdx) {
    _enemies[enemyIdx]->step();
}

// Remove ally at index i
void BoardModel::removeAlly(int i) {
//    _node->removeChild(_allies[i]->getSprite());
    _removedAllies.insert(_allies[i]);
    _allies.erase(_allies.begin() + i);
    _numAllies--;
}

// Remove enemy at index i
void BoardModel::removeEnemy(int i) {
//    _node->removeChild(_enemies[i]->getSprite());
    _removedEnemies.insert(_enemies[i]);
    _enemies.erase(_enemies.begin() + i);
    _numEnemies--;
}


#pragma mark -
#pragma mark Logic

// Check if any matches exist on the board, if so then remove them and check for pawn locations for damage/removal
bool BoardModel::checkForMatches(bool removeEnemies) {
	std::set<int> replaceTiles;
    
	// Check for matches
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			// Check Row
			if (x >= 2) {
				if (_tiles[indexOfCoordinate(x,y)]->getColor() == _tiles[indexOfCoordinate(x - 1, y)]->getColor()
					&& _tiles[indexOfCoordinate(x - 1, y)]->getColor() == _tiles[indexOfCoordinate(x - 2, y)]->getColor()) {
					replaceTiles.insert(indexOfCoordinate(x, y));
					replaceTiles.insert(indexOfCoordinate(x - 1, y));
					replaceTiles.insert(indexOfCoordinate(x - 2, y));
				}
			}
			// Check Column
			if (y >= 2) {
				if (_tiles[indexOfCoordinate(x, y)]->getColor() == _tiles[indexOfCoordinate(x, y - 1)]->getColor()
					&& _tiles[indexOfCoordinate(x, y - 1)]->getColor() == _tiles[indexOfCoordinate(x, y - 2)]->getColor()) {
					replaceTiles.insert(indexOfCoordinate(x, y));
					replaceTiles.insert(indexOfCoordinate(x, y - 1));
					replaceTiles.insert(indexOfCoordinate(x, y - 2));
				}
			}
		}
	}

	bool matchExists = !replaceTiles.empty();

	// Replace Tiles
	std::set<int>::iterator iter;
	for (iter = replaceTiles.begin(); iter != replaceTiles.end(); ++iter) {
		// Replace tile
		replaceTile(*iter);
		// Remove enemies
        if (removeEnemies && !_enemies.empty()) {
            for (int i = 0; i < _enemies.size(); i++) {
                if (indexOfCoordinate(_enemies[i]->getX(), _enemies[i]->getY()) == *iter) {
                    removeEnemy(i);
                    i--;
                }
            }
        }
	}

	return matchExists;
}

// Private function that allows for a tile to be replaced based on it's array index value in _tiles
void BoardModel::replaceTile(int tileLocation) {
//    _node->removeChild(_tiles[tileLocation]->getSprite());
    _removedTiles.insert(_tiles[tileLocation]);
    // New random color
    int color = randomColor();
//    int color = rand() % _numColors;
    Rect bounds = calculateDrawBounds(xOfIndex(tileLocation), yOfIndex(tileLocation));
    std::shared_ptr<TileModel> tile = TileModel::alloc(color, bounds, _assets);
    _tiles[tileLocation] = tile;
//    _node->addChild(tile->getSprite());
    _addedTiles.insert(tile);
}

// Generates a new set of tiles for _tiles that verifies that the board does not have any matches existing
void BoardModel::generateNewBoard() {
	// Setup Tiles
    _tiles.reserve(_width * _height);
    srand((int)time(NULL));
	int color;
    for (int i = 0; i < _height*_width; i++) {
        color = randomColor();
//        color = rand() % _numColors;        // random number in range [0, _numColors-1]
        Rect bounds = calculateDrawBounds(xOfIndex(i), yOfIndex(i));
        std::shared_ptr<TileModel> tile = TileModel::alloc(color, bounds, _assets);
//        _node->addChild(tile->getSprite());
        _tiles.push_back(tile);
        _addedTiles.insert(tile);
    }
    
    // Replace any matches
    while (checkForMatches());

    // Setup Allies
    int x;
    int y;
    for (int i = 0; i < _numAllies; i++) {
        x = rand() % _width;
        y = rand() % _height;
        while (true) {
            bool match = false;
            for (int j = 0; j < i; j++) {
                std::shared_ptr<PlayerPawnModel> temp = _allies[j];
                if (temp->getX() == x && temp->getY() == y) {
                    x = rand() % _width;
                    y = rand() % _height;
                    match = true;
                }
            }
            if (!match) {
                break;
            }
        }
        std::shared_ptr<PlayerPawnModel> ally = PlayerPawnModel::alloc(x, y, calculateDrawBounds(x, y), _assets);
//        _node->addChild(ally->getSprite());
        _allies.push_back(ally);
        _addedAllies.insert(ally);
    }

    //Setup Enemies
    for (int i = 0; i < _numEnemies; i++) {
        x = rand() % _width;
        y = rand() % _height;
        while (true) {
            bool match = false;
            for (int j = 0; j < _numAllies; j++) {
                std::shared_ptr<PlayerPawnModel> temp = _allies[j];
                if (temp->getX() == x && temp->getY() == y) {
                    x = rand() % _width;
                    y = rand() % _height;
                    match = true;
                }
            }
            if (i > 0) {
                for (int j = 0; j < i; j++) {
                    std::shared_ptr<EnemyPawnModel> temp = _enemies[j];
                    if (temp->getX() == x && temp->getY() == y) {
                        x = rand() % _width;
                        y = rand() % _height;
                        match = true;
                    }
                }
            }

            if (!match) {
                break;
            }
        }
        std::shared_ptr<EnemyPawnModel> enemy = EnemyPawnModel::alloc(x, y, calculateDrawBounds(x, y), _assets);
        enemy->setRandomDirection();
//        _node->addChild(enemy->getSprite());
        _enemies.push_back(enemy);
        _addedEnemies.insert(enemy);
    }
}

// Slide pawns in row or column [k] by [offset]
void BoardModel::slidePawns(bool row, int k, int offset) {
    // Slide Allies
    for (int i = 0; i < _numAllies; i++) {
        std::shared_ptr<PlayerPawnModel> pawn = _allies[i];
        if (pawn->getX() != -1 && pawn->getY() != -1) {
            if (row) {
                // Row
                if (k == pawn->getY()) {
                    float x = (pawn->getX() + offset) % _width;
                    while (x < 0) {
                        x += _width;
                    }
                    _allies[i]->setX(x);
//                    _allies[i]->setSpriteBounds(calculateDrawBounds(x, k));
                }
            }
            else {
                // Column
                if (k == pawn->getX()) {
                    float y = (pawn->getY() + offset) % _height;
                    while (y < 0) {
                        y += _height;
                    }
                    _allies[i]->setY(y);
//                    _allies[i]->setSpriteBounds(calculateDrawBounds(k, y));
                }
            }
        }
    }

    // Slide Enemies
    for (int i = 0; i < _numEnemies; i++) {
        std::shared_ptr<EnemyPawnModel> pawn = _enemies[i];
        if (pawn->getX() != -1 && pawn->getY() != -1) {
            if (row) {
                // Row
                if (k == pawn->getY()) {
                    float x = ((int)pawn->getX() + offset) % _width;
                    while (x < 0) {
                        x += _width;
                    }
                    _enemies[i]->setX(x);
//                    _enemies[i]->setSpriteBounds(calculateDrawBounds(x, k));
                }
            }
            else {
                // Column
                if (k == pawn->getX()) {
                    float y = (pawn->getY() + offset) % _height;
                    while (y < 0) {
                        y += _height;
                    }
                    _enemies[i]->setY(y);
//                    _enemies[i]->setSpriteBounds(calculateDrawBounds(k, y));
                }
            }
        }
    }
}

//Slide row or column by [offset]
void BoardModel::slide(bool row, int k, int offset) {
	// Copy
	int sideSize = row ? _width : _height;
    std::shared_ptr<TileModel>* line = new std::shared_ptr<TileModel>[sideSize];
	for (int i = 0; i < sideSize; i++) {
		int x = row ? i : k;
		int y = row ? k : i;
		line[i] = getTile(x, y);
	}

	// Slide/write row
	for (int i = 0; i < sideSize; i++) {
		int x = row ? i : k;
		int y = row ? k : i;
		int j = (i - offset) % sideSize;
		while (j < 0) {
			j += sideSize;
		}
		setTile(x, y, line[j]);
	}

	// Slide pawns
    slidePawns(row, k, offset);
}

//Offset view of row (not model)
void BoardModel::setOffsetRow(float value) {
	offsetRow = true;
	offset = value;
}

//Offset view of col (not model)
void BoardModel::setOffsetCol(float value) {
	offsetCol = true;
	offset = value;
}

//Offset reset
void BoardModel::offsetReset() {
	offsetRow = false;
	offsetCol = false;
	offset = 0.0f;
}

// Deselect _selectedTile
void BoardModel::deselectTile() {
    _selectedTile = -1;
    offsetReset();
}

//Slide row [y] by [offset]
void BoardModel::slideRow(int y, int offset) {
	slide(true, y, offset);
}

//Slide column [x] by [offset]
void BoardModel::slideCol(int x, int offset) {
	slide(false, x, offset);
}

// Slide row/col by [offset]
void BoardModel::slide(int offset) {
    if (offsetRow) {
        slideRow(yOfIndex(_selectedTile), offset);
    } else {
        slideCol(xOfIndex(_selectedTile), offset);
    }
}

/**
 * Select tile at screen position [position]
 *
 * @param position on screen
 *
 * @return true if there is a tile, false otherwise
 */
bool BoardModel::selectTileAtPosition(Vec2 position) {
    int x;
    int y;
    position = position + Vec2(0.0f, -getCellLength()*0.15f*2.0f);
    std::tie(x, y) = screenToGrid(position);
    if (x < 0 || _width <= x) {
        return false;
    }
    if (y < 0 || _height <= y) {
        return false;
    }
    _selectedTile = indexOfCoordinate(x, y);
    return true;
}


#pragma mark -
#pragma mark Drawing/Animation

/** Update nodes */
void BoardModel::updateNodes(bool position, bool z) {
    // Tiles
    for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
            if (position)
                _tiles[indexOfCoordinate(x, y)]->setSpriteBounds(calculateDrawBounds(x, y));
            if (z)
                _tiles[indexOfCoordinate(x, y)]->getSprite()->setZOrder(calculateDrawZ(x, y, true));
        }
    }
    
    // Allies
    for (std::vector<std::shared_ptr<PlayerPawnModel>>::iterator it = _allies.begin(); it != _allies.end(); ++it) {
        if (position)
            (*it)->setSpriteBounds(calculateDrawBounds((*it)->getX(), (*it)->getY()));
        if (z)
            (*it)->getSprite()->setZOrder(calculateDrawZ((*it)->getX(), (*it)->getY(), false));
    }
    
    // Enemies
    for (std::vector<std::shared_ptr<EnemyPawnModel>>::iterator it = _enemies.begin(); it != _enemies.end(); ++it) {
        if (position)
            (*it)->setSpriteBounds(calculateDrawBounds((*it)->getX(), (*it)->getY()));
        if (z)
            (*it)->getSprite()->setZOrder(calculateDrawZ((*it)->getX(), (*it)->getY(), false));
    }
    
    // Resort z order
    _node->sortZOrder();
}

float BoardModel::getCellLength() {
    float cellWidth = (gameWidth - 2.0f*_boardPadding) / _width;
    float cellHeight = (gameHeight - 2.0f*_boardPadding) / _height;
    return fmin(cellWidth, cellHeight);
}

// Convert grid (x, y) to screen coordinates
cugl::Rect BoardModel::gridToScreen(int x, int y) {
    float xPos = _boardPadding + x*_cellSize.width;
    float yPos = _boardPadding + y*_cellSize.height;
    
    return Rect(xPos, yPos, _cellSize.width, _cellSize.height);
}

// Convert screen coordinates to grid (x, y)
std::tuple<int, int> BoardModel::screenToGrid(Vec2 position) {
    int x = (int)floor( (position.x - _boardPadding/2.0f) / _cellSize.width );
    int y = (int)floor( (position.y - _boardPadding/2.0f) / _cellSize.height );
    
    return {x, y};
}

// Convert screen length to grid length
int BoardModel::lengthToCells(float length, bool row) {
    float cellLength = row ? _cellSize.width : _cellSize.height;
    return (int)round( length / cellLength );
}

// Apply padding, offset, and wrap to return tile bounds
Rect BoardModel::calculateDrawBounds(int gridX, int gridY) {
    Rect bounds = gridToScreen(gridX, gridY);
    
    // Apply Padding to Bounds
    float x = bounds.getMinX() + _tilePaddingX/2.0f;
    float y = bounds.getMinY() + _tilePaddingY/2.0f;
    float width = bounds.size.width - _tilePaddingX;
    float height = bounds.size.height - _tilePaddingY;
    float length = std::max(width, height);
    bounds.set(x, y, length, length);
    
    // Calculate Offset
    float xOffset = (offsetRow && gridY == yOfIndex(_selectedTile)) ? offset : 0.0f;
    float yOffset = (offsetCol && gridX == xOfIndex(_selectedTile)) ? offset : 0.0f;
    
    // Calculate Wrap (wrapping tiles around the board as they are moved)
    float xWrap = 0.0f;
    float yWrap = 0.0f;
    float boardWidth = _width*_cellSize.width;
    float boardHeight = _height*_cellSize.height;
    if (bounds.getMidX() + xOffset <= _boardPadding)
        xWrap = boardWidth;
    if (bounds.getMidX() + xOffset >= boardWidth + _boardPadding)
        xWrap = -boardWidth;
    if (bounds.getMidY() + yOffset <= _boardPadding)
        yWrap = boardHeight;
    if (bounds.getMidY() + yOffset >= boardHeight + _boardPadding)
        yWrap = -boardHeight;
    
    // Calculate new bounds
    bounds.set(
               x + xOffset + xWrap,
               y + yOffset + yWrap,
               bounds.size.width,
               bounds.size.height);
    
    // Calculate selected tile
    if (_selectedTile != -1 && gridX == xOfIndex(_selectedTile) && gridY == yOfIndex(_selectedTile)) {
        float padding = 0.01*_cellSize.width;
        Vec2 offset = Vec2(0.0f, -0.07*getCellLength());
        bounds.set(
                   bounds.getMinX() + offset.x,
                   bounds.getMinY() + offset.y,
                   bounds.size.width - padding,
                   bounds.size.height - padding);
    }
    
    return bounds;
}

/**
 * Calculate z-axis coordinate given (x,y) cell in grid.
 *   Tiles at 10s
 *   Pawns at  Tile number + 5
 */
int BoardModel::calculateDrawZ(int x, int y, bool tile) {
    int row = y;
//    if (offsetCol && _selectedTile != -1 && x == xOfIndex(_selectedTile)) {
//        row = (row + lengthToCells(offset)) % _height;
//        while (row < 0) {
//            row += _height;
//        }
//    }
    row = std::get<1>( screenToGrid(calculateDrawBounds(x, y).origin) );
    if (tile) {
        // Start from 10 with increments of 10
        return 10 + 10*(_height-row-1);
    } else {
        // Start from 10*height + 5 with increments of 10
        return (10*_height + 5) + 10*(_height-row-1);
    }
}

/**
* Returns a string representation of the board for debugging purposes.
*
* @return a string representation of this vector for debuggging purposes.
*/
std::string BoardModel::toString() const {
    std::stringstream ss;
    for (int j = _height-1; j >= 0; j--) {
        for (int i = 0; i < _width; i++) {
            ss << " ";
            ss << _tiles[indexOfCoordinate(i, j)]->getColor();
        }
        ss << "\n";
    }
    if (!_allies.empty()) {
        ss << "allies: [";
        for (int i = 0; i < _numAllies; i++) {
            ss << "(" << _allies[i]->getX() << ", " << _allies[i]->getY() << ")";
            ss << "   ";
        }
        ss << "]";
    }
    if (!_enemies.empty()) {
        ss << "\nenemies: [";
        for (int i = 0; i < _numEnemies; i++) {
            ss << "(" << _enemies[i]->getX() << ", " << _enemies[i]->getY() << ")";
            ss << "   ";
        }
        ss << "]";
    }
    return ss.str();
}

