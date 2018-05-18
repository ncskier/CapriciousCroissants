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
bool PlayerPawnModel::init(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets, bool isMika) {
    _x = x;
    _y = y;
    _isMika = isMika;
    // Create sprite
    if (isMika) {
        // Mika
        std::shared_ptr<Texture> texture = assets->get<Texture>(PLAYER_TEXTURE_KEY_0);
        _sprite = AnimationNode::alloc(texture, PLAYER_IMG_ROWS, PLAYER_IMG_COLS, PLAYER_IMG_SIZE);
        _sprite->setFrame(PLAYER_IMG_NORMAL);
        // End sprite
        std::shared_ptr<Texture> endTexture = assets->get<Texture>(PLAYER_TEXTURE_END_KEY);
        _endSprite = AnimationNode::alloc(endTexture, PLAYER_END_ROWS, PLAYER_END_COLS, PLAYER_END_SIZE);
        _endSprite->setFrame(PLAYER_END_WIN_START);
        _endSprite->setVisible(false);
    } else {
        // Ally
        std::shared_ptr<Texture> texture = assets->get<Texture>(ALLY_TEXTURE_KEY_IDLE);
        _sprite = AnimationNode::alloc(texture, ALLY_IDLE_IMG_ROWS, ALLY_IDLE_IMG_COLS, ALLY_IDLE_IMG_SIZE);
        _sprite->setFrame(ALLY_IDLE_IMG_START);
        // End sprite
        std::shared_ptr<Texture> endTexture = assets->get<Texture>(ALLY_TEXTURE_KEY_DEATH);
        _endSprite = AnimationNode::alloc(endTexture, ALLY_DEATH_IMG_ROWS, ALLY_DEATH_IMG_COLS, ALLY_DEATH_IMG_SIZE);
        _endSprite->setFrame(ALLY_DEATH_IMG_START);
        _endSprite->setVisible(false);
    }
    _sprite->setAnchor(Vec2::ANCHOR_CENTER);
    setSpriteBounds(tileBounds);
    return true;
}

/** Initialize a new player pawn at (x, y) tile with [tileBounds] */
bool PlayerPawnModel::init(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets, bool isMika, std::string name) {
	ownName = name;
	return init(x, y, tileBounds, assets, isMika);
}

/** Disposes all resources and assets of this enemy */
void PlayerPawnModel::dispose() {
    _sprite = nullptr;
    _endSprite = nullptr;
}


#pragma mark -
#pragma mark Animation

/** Set sprite bounds from tile [tileBounds] */
void PlayerPawnModel::setSpriteBounds(cugl::Rect tileBounds) {
    Size nodeSize = _sprite->cugl::Node::getSize();
    // Keeps aspect ratio, but matches [tileBounds] height
    float height = tileBounds.size.height;
    float width = nodeSize.width / nodeSize.height * height;
    if (!_isMika) {
        float scale = 2.0f;
        height *= scale;
        width *= scale;
    }
    float positionX = tileBounds.getMidX();
    float positionY = tileBounds.getMidY() + tileBounds.size.height*0.3f;
    if (!_isMika) {
        positionX = tileBounds.getMidX() + width*0.04f;
        positionY = tileBounds.getMidY() + tileBounds.size.height*0.3f;
    }
    _sprite->setPosition(positionX, positionY);
    _sprite->setContentSize(width, height);
    
    // Update end sprite
    if (_isMika) {
        float endScale = 1.19f;
        _endSprite->setContentSize(width*endScale, height*endScale);
    } else {
        _endSprite->setContentSize(width, height);
    }
    _endSprite->setPosition(positionX, positionY);
}

/** Set sprite lose size */
void PlayerPawnModel::setSpriteLose() {
    if (_isMika) {
        float scale = 1.04f;
        _endSprite->setContentSize(_sprite->getContentSize().width*scale, _sprite->getContentSize().height*scale);
        _endSprite->setPosition(_sprite->getPosition());
    }
}



