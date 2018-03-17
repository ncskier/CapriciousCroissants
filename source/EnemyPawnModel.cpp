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
    srand((int)time(NULL));
    _direction = (Direction)(rand() % 4);
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
