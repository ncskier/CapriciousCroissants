//
//  PlayerPawnModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "PlayerPawnModel.h"

#pragma mark -
#pragma mark Constructors/Destructors

/** Initialize a new player pawn at (x, y) */
bool PlayerPawnModel::init(int x, int y) {
    _x = x;
    _y = y;
    return true;
}

/** Disposes all resources and assets of this enemy */
void PlayerPawnModel::dispose() {
    _sprite = nullptr;
}

