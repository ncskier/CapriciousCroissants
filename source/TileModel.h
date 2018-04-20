//
//  TileModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __TILE_MODEL_H__
#define __TILE_MODEL_H__

#include <cugl/cugl.h>

/** Tile Frame Sprite numbers */
#define TILE_IMG_APPEAR_START    16
#define TILE_IMG_APPEAR_END      23
#define TILE_IMG_NORMAL          0
#define TILE_IMG_DISAPPEAR_START 0
#define TILE_IMG_DISAPPEAR_END   15
#define TILE_IMG_APPEAR_TIME    0.3f
#define TILE_IMG_DISAPPEAR_TIME 0.6f

/** Number of rows and cols in film strip */
#define TILE_IMG_ROWS 3
#define TILE_IMG_COLS 8
#define TILE_IMG_SIZE 24

/** Tile texture keys */
#define TILE_TEXTURE_KEY_NULL "tileNULL_strip"
#define TILE_TEXTURE_KEY_0 "tile0_strip"
#define TILE_TEXTURE_KEY_1 "tile1_strip"
#define TILE_TEXTURE_KEY_2 "tile2_strip"
#define TILE_TEXTURE_KEY_3 "tile3_strip"
#define TILE_TEXTURE_KEY_4 "tile4_strip"
#define TILE_TEXTURE_KEY_5 "tile5_strip"
#define TILE_TEXTURE_KEY_6 "tile0_strip"
#define TILE_TEXTURE_KEY_7 "tile0_strip"
#define TILE_TEXTURE_KEY_8 "tile0_strip"
#define TILE_TEXTURE_KEY_9 "tile0_strip"


#pragma mark -
#pragma mark Tile Model

/**
 * TileModel
 *
 * Stores match information and AnimationNode for drawing.
 */
class TileModel {
protected:
    /** Match information */
    int _color;
    
    /** Reference to image in SceneGraph for animation */
    std::shared_ptr<cugl::AnimationNode> _sprite;
    
    /** Sets the film strip with [bounds] */
    void setSprite(const cugl::Rect bounds, const std::shared_ptr<cugl::AssetManager>& assets);

    
public:
#pragma mark -
#pragma mark Constructors/Destructors
    /** Creates a new tile with color 0 */
    TileModel(void) : _color(0) { }
    
    /** Destroys tile, releasing all resources */
    ~TileModel(void) { dispose(); }
    
    /** Disposes all resources and assets of this tile */
    void dispose();
    
    /** Initialize a new tile with color 0 */
    virtual bool init() { return true; }
    
    /** Initialize a new tile with [color] */
    virtual bool init(int color);
    
    /** Initialize a new tile with [color] and dimensions [dimen] with anchor (0,0) */
    virtual bool init(int color, cugl::Rect bounds, std::shared_ptr<cugl::AssetManager>& assets);
    
#pragma mark -
#pragma mark Static Constructors
    /** Returns newly allocated tile with color 0 */
    static std::shared_ptr<TileModel> alloc() {
        std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
        return (result->init() ? result : nullptr);
    }
    
    /** Returns newly allocated tile with [color] */
    static std::shared_ptr<TileModel> alloc(int color) {
        std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
        return (result->init(color) ? result : nullptr);
    }
    
    /** Returns newly allocated tile with [color] and dimensions [dimen] with anchor (0,0) */
    static std::shared_ptr<TileModel> alloc(int color, cugl::Rect bounds, std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
        return (result->init(color, bounds, assets) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Accessors/Mutators
    /** Return match information */
    int getColor() const { return _color; }
    
    /** Return whether tile is null tile */
    bool isNull() const { return (_color == -1); }
    
    /** Set match information */
    void setColor(int color) { _color = color; }
    
#pragma mark -
#pragma mark Animation
    /** Returns a reference to the film strip */
    std::shared_ptr<cugl::AnimationNode>& getSprite() { return _sprite; }
    
    /** Set sprite [bounds] */
    void setSpriteBounds(cugl::Rect bounds);
    
    /** Sets the film strip */
    void setSprite(const std::shared_ptr<cugl::AnimationNode>& sprite) { _sprite = sprite; }

};

#endif /* __TILE_MODEL_H__ */
