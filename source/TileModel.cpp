//
//  TileModel.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "TileModel.h"

using namespace cugl;


#pragma mark -
#pragma mark Constructors/Destructors

/** Initialize a new tile with [color] */
bool TileModel::init(int color) {
    _color = color;
    return true;
}

/** Initialize a new tile with [color] and dimensions [dimen] with anchor (0,0) */
bool TileModel::init(int color, cugl::Rect bounds, std::shared_ptr<cugl::AssetManager>& assets) {
    _color = color;
    setSprite(bounds, assets);
    return true;
}

/** Disposes all resources and assets of this tile */
void TileModel::dispose() {
    _sprite = nullptr;
}

#pragma mark -
#pragma mark Accessors/Mutators

/** Sets the film strip */
void TileModel::setSprite(const Rect bounds, const std::shared_ptr<cugl::AssetManager>& assets) {
    // Get Texture
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Texture> deathTexture;
    Color4 color = Color4::WHITE;
    if (_color == -1) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_NULL);
    } else if (_color == 0) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_0);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_0);
    } else if (_color == 1) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_1);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_1);
    } else if (_color == 2) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_2);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_2);
//        color = Color4::RED;
    } else if (_color == 3) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_3);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_3);
//        color = Color4::CYAN;
    } else if (_color == 4) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_4);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_4);
//        color = Color4::GREEN;
    } else if (_color == 5) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_5);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_5);
//        color = Color4::YELLOW;
    } else if (_color == 6) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_6);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_6);
        color = Color4::BLUE;
    } else if (_color == 7) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_7);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_7);
        color = Color4::MAGENTA;
    } else if (_color == 8) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_8);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_8);
        color = Color4::CORNFLOWER;
    } else if (_color == 9) {
        texture = assets->get<Texture>(TILE_TEXTURE_KEY_9);
        deathTexture = assets->get<Texture>(TILE_TEXTURE_KEY_DEATH_9);
        color = Color4::ORANGE;
    }
    
    // Create Animation Node
    _sprite = AnimationNode::alloc(texture, TILE_IMG_ROWS, TILE_IMG_COLS, TILE_IMG_SIZE);
    _sprite->setFrame(TILE_IMG_NORMAL);
    _sprite->setColor(color);
    _sprite->setAnchor(Vec2::ZERO);
    
    // Create Death Animation Node
    if (deathTexture) {
        _deathSprite = AnimationNode::alloc(deathTexture, TILE_DEATH_ROWS, TILE_DEATH_COLS, TILE_DEATH_SIZE);
        _deathSprite->setFrame(TILE_DEATH_NORMAL);
        _deathSprite->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
        _deathSprite->setVisible(false);
    }
    
    // Set Sprite Bounds
    setSpriteBounds(bounds);
}


#pragma mark -
#pragma mark Animation

/** Set sprite [bounds] */
void TileModel::setSpriteBounds(cugl::Rect bounds) {
    // Animation Sprite
    _sprite->setPosition(bounds.origin);
    _sprite->setContentSize(bounds.size);
    
    // Death Sprite
    if (_deathSprite) {
        float width = _sprite->getWidth() * 0.8f;
        float height = _deathSprite->getHeight() * width / _deathSprite->getWidth();
        _deathSprite->setContentSize(width, height);
        _deathSprite->setPosition(bounds.getMidX(), bounds.getMinY() + bounds.size.height*0.3f);
    }
}
