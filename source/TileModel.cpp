//
//  TileModel.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "TileModel.h"

#pragma mark -
#pragma mark Constructors/Destructors

/** Initialize a new tile with random color */
bool TileModel::init(int numColors) {
    srand((int)time(NULL));
    _color = rand() % numColors;
    return true;
}

/** Disposes all resources and assets of this tile */
void TileModel::dispose() {
    _sprite = nullptr;
}
