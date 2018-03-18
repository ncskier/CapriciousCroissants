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
#define TILE_IMG_APPEAR 0
#define TILE_IMG_NORMAL 7
#define TILE_IMG_DISAPPEAR 24

/** Number of rows and cols in film strip */
#define TILE_IMG_ROWS 1
#define TILE_IMG_COLS 1
#define TILE_IMG_SIZE 1


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
    
    /** Initialize a new tile with random color */
    virtual bool init(int numColors);
    
#pragma mark -
#pragma mark Static Constructors
    /** Returns newly allocated tile with color 0 */
    static std::shared_ptr<TileModel> alloc() {
        std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
        return (result->init() ? result : nullptr);
    }
    
    /** Returns newly allocated tile with random color */
    static std::shared_ptr<TileModel> alloc(int numColors) {
        std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
        return (result->init(numColors) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Accessors/Mutators
    /** Return match information */
    int getColor() const { return _color; }
    
    /** Set match information */
    void setColor(int color) { _color = color; }
    
#pragma mark -
#pragma mark Animation
    /** Returns a reference to the film strip */
    std::shared_ptr<cugl::AnimationNode>& getSprite() { return _sprite; }
    
    /** Sets the film strip */
    void setSprite(const std::shared_ptr<cugl::AnimationNode>& sprite) { _sprite = sprite; }

};

#endif /* __TILE_MODEL_H__ */
