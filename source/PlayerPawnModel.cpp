//
//  PlayerPawnModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "PlayerPawnModel.h"

using namespace cugl;


#pragma mark -
#pragma mark Constructors/Destructors

/** Initialize a new player pawn at (x, y) */
bool PlayerPawnModel::init(int x, int y) {
    _x = x;
    _y = y;
    return true;
}

/** Initialize a new player pawn at (x, y) tile with [tileBounds] */
bool PlayerPawnModel::init(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets) {
    _x = x;
    _y = y;
    // Create sprite
    std::shared_ptr<Texture> texture = assets->get<Texture>(PLAYER_TEXTURE_KEY_0);
    _sprite = AnimationNode::alloc(texture, PLAYER_IMG_ROWS, PLAYER_IMG_COLS, PLAYER_IMG_SIZE);
    _sprite->setAnchor(Vec2::ZERO);
    setSpriteBounds(tileBounds);
    return true;
}

/** Disposes all resources and assets of this enemy */
void PlayerPawnModel::dispose() {
    _sprite = nullptr;
}


#pragma mark -
#pragma mark Animation

/** Set sprite bounds from tile [tileBounds] */
void PlayerPawnModel::setSpriteBounds(cugl::Rect tileBounds) {
    Size nodeSize = _sprite->cugl::Node::getSize();
    float width = nodeSize.width / nodeSize.height * tileBounds.size.height;
    float height = tileBounds.size.height;
    float positionX = tileBounds.getMinX() + (tileBounds.size.width-width)/2.0f;
    float positionY = tileBounds.getMinY() + tileBounds.size.height*0.3f;
    _sprite->setPosition(positionX, positionY);
    _sprite->setContentSize(width, height);
}

