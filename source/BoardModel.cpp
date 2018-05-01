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

/** Initializes the board */
bool BoardModel::init(std::shared_ptr<cugl::JsonValue> &json, std::shared_ptr<AssetManager>& assets, Size dimen, std::shared_ptr<EntityManager>& entityManager, std::shared_ptr<ActionManager>& actions) {
    CULog("Init JSON");
    
    // Set asset manager
    _assets = assets;
	_entityManager = entityManager;
    
    // Get Board Node info (width, height, colors, seed)
    int width = json->get("size")->get("width")->asInt();
    int height = json->get("size")->get("height")->asInt();
    int colors = json->get("colors")->asInt();
    
    // Setup Board Node
    if (!setupBoardNode(width, height, colors, dimen)) {
        return false;
    }
    
    // Setup Tiles from json
    int seed = (int)time(NULL);
    if (json->get("seed") != nullptr) {
        seed = json->get("seed")->asInt();
    }
    CULog("seed: %i", seed);
    if (!generateTiles(seed)) {
        return false;
    }
    
    // Setup Allies from json
    if (!setupAlliesFromJson(json)) {
        return false;
    }
    
    // Setup Enemies from json
    if (!setupEnemiesFromJson(json, actions)) {
        return false;
    }
    
    return true;
}

// Destroy any values needed to be deleted for this class
void BoardModel::dispose() {
    CULog("dispose BoardModel");
    _node->removeAllChildren();
    _node = nullptr;
    _assets = nullptr;
	_entityManager = nullptr;
    _tiles.clear();
    _allies.clear();
    enemiesEntityIds.clear();
    _addedTiles.clear();
    _addedAllies.clear();
    _addedEnemies.clear();
    _removedTiles.clear();
    _removedAllies.clear();
    _removedEnemies.clear();
}


#pragma mark -
#pragma mark Initialization Helpers
/** Setup board node & display properties */
bool BoardModel::setupBoardNode(int width, int height, int colors, Size dimen) {
    // Setup Board properties
    _width = width;
    _height = height;
    _numColors = colors;
    
    // Setup Board Node
    gameWidth = dimen.width;
    gameHeight = dimen.height;
    float cellLength = getCellLength();
    float boardWidth = cellLength * _width;
    float boardHeight = cellLength * _height;
    _node = Node::alloc();
    _node->setContentSize(boardWidth, boardHeight);
    _node->setAnchor(Vec2::ANCHOR_CENTER);
    _node->setPosition(dimen.width*0.5f, dimen.height*0.5f);
    CULog("board dimen: (%f, %f)", boardWidth, boardHeight);
    CULog("game dimen: (%f, %f)", dimen.width, dimen.height);
    
    // Set cell size
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

/** Change (x,y) to NULL tile */
void BoardModel::setNullTile(int x, int y) {
    Rect bounds = calculateDrawBounds(x, y);
    std::shared_ptr<TileModel> tile = TileModel::alloc(-1, bounds, _assets);
    int index = indexOfCoordinate(x, y);
    _removedTiles.insert(_tiles[index]);
    _tiles[indexOfCoordinate(x, y)] = tile;
    _addedTiles.insert(tile);
}

/** Setup allies from Json */
bool BoardModel::setupAlliesFromJson(std::shared_ptr<cugl::JsonValue>& json) {
    // Vars for creating allies
    int x;
    int y;
    
    // Setup Mika
    std::shared_ptr<JsonValue> mikaJson = json->get("mika");
    x = mikaJson->get("x")->asInt();
    y = mikaJson->get("y")->asInt();
    std::shared_ptr<PlayerPawnModel> mika = PlayerPawnModel::alloc(x, y, calculateDrawBounds(x, y), _assets);
    _allies.push_back(mika);
    _addedAllies.insert(mika);
    // Set Mika's tile to NULL tile
    setNullTile(x, y);
    
    // Setup Allies
    std::shared_ptr<JsonValue> alliesJson = json->get("allies");
    for (auto i = 0; i < alliesJson->size(); i++) {
        std::shared_ptr<JsonValue> allyJson = alliesJson->get(i);
        x = allyJson->get("x")->asInt();
        y = allyJson->get("y")->asInt();
        std::shared_ptr<PlayerPawnModel> ally = PlayerPawnModel::alloc(x, y, calculateDrawBounds(x, y), _assets);
        ally->getSprite()->setColor(Color4::BLUE);
        _allies.push_back(ally);
        _addedAllies.insert(ally);
        // Set ally's tile to NULL tile
        setNullTile(x, y);
    }
    
    // Set num allies
    _numAllies = (int)_allies.size();
    
    return true;
}

/** Setup enemies from Json */
bool BoardModel::setupEnemiesFromJson(std::shared_ptr<cugl::JsonValue>& json, std::shared_ptr<ActionManager>& actions) {
    
    // Setup Enemies
    std::shared_ptr<JsonValue> enemiesJson = json->get("enemies");

    for (auto i = 0; i < enemiesJson->size(); i++) {
		size_t enemyId = _entityManager->createEntity();
        std::shared_ptr<JsonValue> enemyJson = enemiesJson->get(i);
        
        // Parse Components
        std::shared_ptr<JsonValue> enemyComponentsJson = enemyJson->get("components");
        for (auto j = 0; j < enemyComponentsJson->size(); j++) {
            std::shared_ptr<JsonValue> componentJson = enemyComponentsJson->get(j);
            if ("location" == componentJson->key()) {
				LocationComponent loc;

				loc.x = componentJson->get("x")->asInt();
				loc.y = componentJson->get("y")->asInt();
				if (componentJson->has("direction")) {
					loc.dir = (LocationComponent::direction)componentJson->get("direction")->asInt();
				}

				_entityManager->addComponent<LocationComponent>(enemyId, loc);
			} else if ("dumbMovement" == componentJson->key()) {
				DumbMovementComponent move;

				move.movementDistance = componentJson->get("movementDistance")->asInt();

				_entityManager->addComponent<DumbMovementComponent>(enemyId, move);
			} else if ("smartMovement" == componentJson->key()) {
				SmartMovementComponent move;

				move.movementDistance = componentJson->get("movementDistance")->asInt();

				_entityManager->addComponent<SmartMovementComponent>(enemyId, move);
			} else if ("idle" == componentJson->key()) {
				IdleComponent idle;
				idle.textureKey = componentJson->get("textureKeys")->asString();
				idle.textureRows = componentJson->get("textureRows")->asIntArray();
				idle.textureColumns = componentJson->get("textureColumns")->asIntArray();
				idle.textureSize = componentJson->get("textureSize")->asIntArray();
				idle.speed = componentJson->get("textureSpeed")->asIntArray();
				idle.sprite = AnimationNode::alloc(_assets->get<Texture>(idle.textureKey), idle.textureRows[0], idle.textureColumns[0], idle.textureSize[0]);
				idle.sprite->setAnchor(Vec2::ZERO);
				idle._actions = actions;


				_entityManager->addComponent<IdleComponent>(enemyId, idle);

			} else if ("attackMelee" == componentJson->key()) {
				MeleeAttackComponent melee;

				_entityManager->addComponent<MeleeAttackComponent>(enemyId, melee);

			} else if ("attackRanged" == componentJson->key()) {
				RangeOrthoAttackComponent ranged;
				ranged.horizontal = true;
				ranged.vertical = true;


				_entityManager->addComponent<RangeOrthoAttackComponent>(enemyId, ranged);

			}


			else if ("rooting" == componentJson->key()) {
				RootingComponent rooting;

				_entityManager->addComponent<RootingComponent>(enemyId, rooting);
			}
			else if ("immobileMovement" == componentJson->key()) {
				ImmobileMovementComponent move;

				_entityManager->addComponent<ImmobileMovementComponent>(enemyId, move);
			}

        }

		IdleComponent idle = _entityManager->getComponent<IdleComponent>(enemyId); //Now we know everything is setup so we can configure the sprite
		LocationComponent loc = _entityManager->getComponent<LocationComponent>(enemyId);
		Rect tileBounds = calculateDrawBounds(loc.x, loc.y);

		//* setSpriteBounds from old enemyPawnModel, this really should change to be done better */
		float width = tileBounds.size.width * 1.2f;
		float height = tileBounds.size.height * 1.2f;
		float positionX = tileBounds.getMinX() + (tileBounds.size.width - width) / 2.0f;
		float positionY = tileBounds.getMinY() + (tileBounds.size.height - height) / 2.0f + tileBounds.size.height*0.15f / 2.0f + tileBounds.size.height*0.4f;
		if (_entityManager->hasComponent<SmartMovementComponent>(enemyId)) {
			width = tileBounds.size.width * 0.9f;
			height = tileBounds.size.height * 0.9f;
			positionX = tileBounds.getMinX() + (tileBounds.size.width - width) / 2.0f;
			positionY = tileBounds.getMinY() + (tileBounds.size.height - height) / 2.0f + tileBounds.size.height*0.15f / 2.0f + tileBounds.size.height*0.2f;
		}

		idle.sprite->setPosition(positionX, positionY);
		idle.sprite->setContentSize(width, height);
		switch (loc.dir) {
			case LocationComponent::UP:
				idle.sprite->setFrame(ENEMY_FRAME_UP);
				break;
			case LocationComponent::DOWN:
				idle.sprite->setFrame(ENEMY_FRAME_DOWN);
				break;
			case LocationComponent::LEFT:
				idle.sprite->setFrame(ENEMY_FRAME_LEFT);
				break;
			case LocationComponent::RIGHT:
				idle.sprite->setFrame(ENEMY_FRAME_RIGHT);
				break;
		}
		//Shouldn't need to save as sprite is a shared_ptr

		//* FIX THE ABOVE TO A CORRECT WAY OF DOING THIS, THIS IS TERRIBLE */

        
        // Create enemy
        enemiesEntityIds.push_back(enemyId);
        _addedEnemies.insert(enemyId);
		_entityManager->registerEntity(enemyId);
    }
    
    // Set num enemies
    _numEnemies = (int)enemiesEntityIds.size();
    
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
size_t BoardModel::getEnemy(int i) {
	return enemiesEntityIds[i];
}

// Returns the enemy pawn at (x, y)
size_t BoardModel::getEnemy(int x, int y) {
    for (int i = 0; i < _numEnemies; i++) {
        size_t enemy = enemiesEntityIds[i];
		LocationComponent loc = _entityManager->getComponent<LocationComponent>(enemy);

        if (loc.x == x && loc.y == y) {
            return enemy;
        }
    }
    return 0;
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
	size_t enemy = enemiesEntityIds[i];
	LocationComponent loc = _entityManager->getComponent<LocationComponent>(enemy);
	loc.x = x;
	loc.y = y;
	_entityManager->addComponent<LocationComponent>(enemy, loc);
}

// Remove ally at index i
void BoardModel::removeAlly(int i) {
    // Replace NULL Tile
    replaceTile(indexOfCoordinate(_allies[i]->getX(), _allies[i]->getY()));
    
    // Remove Ally
    _removedAllies.insert(_allies[i]);
    _allies.erase(_allies.begin() + i);
    _numAllies--;
}

// Remove enemy at index i
void BoardModel::removeEnemy(int i) {
//    _node->removeChild(_enemies[i]->getSprite());
    _removedEnemies.insert(enemiesEntityIds[i]);
    enemiesEntityIds.erase(enemiesEntityIds.begin() + i);
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
                int x0 = x;
                int x1 = x-1;
                int x2 = x-2;
				if (_tiles[indexOfCoordinate(x0,y)]->getColor() == _tiles[indexOfCoordinate(x1, y)]->getColor()
					&& _tiles[indexOfCoordinate(x1, y)]->getColor() == _tiles[indexOfCoordinate(x2, y)]->getColor()) {
                    if (!_tiles[indexOfCoordinate(x0, y)]->isNull()) {
                        replaceTiles.insert(indexOfCoordinate(x, y));
                        replaceTiles.insert(indexOfCoordinate(x1, y));
                        replaceTiles.insert(indexOfCoordinate(x2, y));
                    }
				}
			}
			// Check Column
			if (y >= 2) {
                int y0 = y;
                int y1 = y-1;
                int y2 = y-2;
				if (_tiles[indexOfCoordinate(x, y0)]->getColor() == _tiles[indexOfCoordinate(x, y1)]->getColor()
					&& _tiles[indexOfCoordinate(x, y1)]->getColor() == _tiles[indexOfCoordinate(x, y2)]->getColor()) {
                    if (!_tiles[indexOfCoordinate(x, y0)]->isNull()) {
                        replaceTiles.insert(indexOfCoordinate(x, y0));
                        replaceTiles.insert(indexOfCoordinate(x, y1));
                        replaceTiles.insert(indexOfCoordinate(x, y2));
                    }
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
        if (removeEnemies && !enemiesEntityIds.empty()) {
            for (int i = 0; i < enemiesEntityIds.size(); i++) {
				LocationComponent loc = _entityManager->getComponent<LocationComponent>(enemiesEntityIds[i]);
                if (indexOfCoordinate(loc.x, loc.y) == *iter) {
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
    _tiles[tileLocation]->x = xOfIndex(tileLocation);
    _tiles[tileLocation]->y = yOfIndex(tileLocation);
    _removedTiles.insert(_tiles[tileLocation]);
    // New random color
    int color = randomColor();
    Rect bounds = calculateDrawBounds(xOfIndex(tileLocation), yOfIndex(tileLocation));
    std::shared_ptr<TileModel> tile = TileModel::alloc(color, bounds, _assets);
    _tiles[tileLocation] = tile;
    _addedTiles.insert(tile);
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
        size_t enemyId = enemiesEntityIds[i];
		LocationComponent loc = _entityManager->getComponent<LocationComponent>(enemyId);
        if (loc.x != -1 && loc.y!= -1) {
            if (row) {
                // Row
                if (k == loc.y) {
                    float x = ((int)loc.x + offset) % _width;
                    while (x < 0) {
                        x += _width;
                    }
					loc.x = x;
                }
            }
            else {
                // Column
                if (k == loc.x) {
                    float y = (loc.y + offset) % _height;
                    while (y < 0) {
                        y += _height;
                    }
					loc.y = y;
                }
            }

			_entityManager->addComponent<LocationComponent>(enemyId, loc);
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
    
    // Free memory allocated for line
    delete[] line;
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
    position = position + Vec2(0.0f, -getCellLength()*0.01f);
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
            if (z) {
                _tiles[indexOfCoordinate(x, y)]->getSprite()->setZOrder(calculateDrawZ(x, y, true));
                if (_tiles[indexOfCoordinate(x, y)]->getDeathSprite()) {
                    _tiles[indexOfCoordinate(x, y)]->getDeathSprite()->setZOrder(calculateDrawZ(x, y, false) + 1);
                }
            }
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
    for (std::vector<size_t>::iterator it = enemiesEntityIds.begin(); it != enemiesEntityIds.end(); ++it) {
		LocationComponent loc = _entityManager->getComponent<LocationComponent>((*it));
		IdleComponent idle = _entityManager->getComponent<IdleComponent>((*it));

		if (position) {
			Rect tileBounds = calculateDrawBounds(loc.x, loc.y);
            float width = tileBounds.size.width;
            float height = tileBounds.size.height;
            float positionX = tileBounds.getMinX() + (tileBounds.size.width - width) / 2.0f;
            float positionY = tileBounds.getMinY() + (tileBounds.size.height - height) / 2.0f + tileBounds.size.height*0.3f;
            idle.sprite->setPosition(positionX, positionY);
            idle.sprite->setContentSize(width, height);
		}
        if (z)
            idle.sprite->setZOrder(calculateDrawZ(loc.x, loc.y, false));
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
    float xPos = x * _cellSize.width;
    float yPos = y * _cellSize.height;
    
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
//    float x = bounds.getMinX() - _tilePaddingX/2.0f + _tilePaddingY/2.0f;
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
    if (bounds.getMidX() + xOffset <= 0)
        xWrap = boardWidth;
    if (bounds.getMidX() + xOffset >= boardWidth)
        xWrap = -boardWidth;
    if (bounds.getMidY() + yOffset <= _boardPadding*0.4f)
        yWrap = boardHeight;
    if (bounds.getMidY() + yOffset >= boardHeight + _boardPadding*0.4f)
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
    if (!enemiesEntityIds.empty()) {
        ss << "\nenemies: [";
        for (int i = 0; i < _numEnemies; i++) {
			LocationComponent loc = _entityManager->getComponent<LocationComponent>(enemiesEntityIds[i]);

            ss << "(" << loc.x << ", " << loc.y << ")";
            ss << "   ";
        }
        ss << "]";
    }
    return ss.str();
}

