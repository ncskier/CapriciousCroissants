//
//  EnemyPawnModel.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/17/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "EnemyPawnModel.h"

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
