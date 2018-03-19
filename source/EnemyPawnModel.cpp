//
//  EnemyPawnModel.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/17/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "EnemyPawnModel.h"

using namespace cugl;


#pragma mark -
#pragma mark Constructors/Destructors

/** Initialize a new enemy at (x, y) */
bool EnemyPawnModel::init(int x, int y) {
    _x = x;
    _y = y;
    setRandomDirection();
    return true;
}

/** Initialize a new enemy at (x, y) facing [direction] */
bool EnemyPawnModel::init(int x, int y, Direction direction) {
    _x = x;
    _y = y;
    _direction = direction;
    return true;
}

/** Initialize a new player pawn at (x, y) tile with [tileBounds] facing NORTH */
bool EnemyPawnModel::init(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets) {
    return init(x, y, Direction::NORTH, tileBounds, assets);
}

/** Initialize a new player pawn at (x, y) tile with [tileBounds] facing [direction] */
bool EnemyPawnModel::init(int x, int y, Direction direction, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets) {
    _x = x;
    _y = y;
    _direction = direction;
    // Create sprite
    std::shared_ptr<Texture> texture = assets->get<Texture>(ENEMY_TEXTURE_KEY_0);
    _sprite = AnimationNode::alloc(texture, ENEMY_IMG_ROWS, ENEMY_IMG_COLS, ENEMY_IMG_SIZE);
    _sprite->setAnchor(Vec2::ZERO);
    setSpriteBounds(tileBounds);
    return true;
}

/** Disposes all resources and assets of this enemy */
void EnemyPawnModel::dispose() {
    _sprite = nullptr;
}


#pragma mark -
#pragma mark Accessors/Mutators

/** Step one unit forward in the direction the enemy is facing */
void EnemyPawnModel::step() {
    if (_direction == Direction::NORTH) {
        // NORTH
        _y += 1;
    } else if (_direction == Direction::SOUTH) {
        // SOUTH
        _y -= 1;
    } else if (_direction == Direction::EAST) {
        // EAST
        _x += 1;
    } else {
        // WEST
        _x -= 1;
    }
}

/** Step one unit backward in the direction the enemy is facing */
void EnemyPawnModel::stepBack() {
    if (_direction == Direction::NORTH) {
        // NORTH
        _y -= 1;
    } else if (_direction == Direction::SOUTH) {
        // SOUTH
        _y += 1;
    } else if (_direction == Direction::EAST) {
        // EAST
        _x -= 1;
    } else {
        // WEST
        _x += 1;
    }
}

/** Turn 180 degrees around */
void EnemyPawnModel::turnAround() {
    if (_direction == Direction::NORTH) {
        // NORTH
        _direction = Direction::SOUTH;
    } else if (_direction == Direction::SOUTH) {
        // SOUTH
        _direction = Direction::NORTH;
    } else if (_direction == Direction::EAST) {
        // EAST
        _direction = Direction::WEST;
    } else {
        // WEST
        _direction = Direction::EAST;
    }
}

/** Set random direction */
void EnemyPawnModel::setRandomDirection() {
    srand((int)time(NULL));
    _direction = (Direction)(rand() % 4);
}


#pragma mark -
#pragma mark Animation

/** Set sprite bounds from tile [tileBounds] */
void EnemyPawnModel::setSpriteBounds(cugl::Rect tileBounds) {
    float width = tileBounds.size.width * 0.5f;
    float height = tileBounds.size.height * 0.5f;
    float positionX = tileBounds.getMinX() + (tileBounds.size.width-width)/2.0f;
    float positionY = tileBounds.getMinY() + (tileBounds.size.height-height)/2.0f;
    _sprite->setPosition(positionX, positionY);
    _sprite->setContentSize(width, height);
}
