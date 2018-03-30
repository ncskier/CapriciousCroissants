//
//  PlayerPawnModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __PLAYER_PAWN_MODEL_H__
#define __PLAYER_PAWN_MODEL_H__

#include <cugl/cugl.h>

/** Tile Frame Sprite numbers */
#define PLAYER_IMG_NORMAL 0
/** Idle loop */
#define PLAYER_IMG_IDLE_START    0
#define PLAYER_IMG_IDLE_END      15
#define PLAYER_IMG_IDLE_TIME     0.7f
/** Begin Attack (Transition to Attack) */
#define PLAYER_IMG_BEGIN_ATTACK_START 16
#define PLAYER_IMG_BEGIN_ATTACK_END   31
#define PLAYER_IMG_BEGIN_ATTACK_TIME  0.7f
/** Attacking loop */
#define PLAYER_IMG_ATTACKING_START 32
#define PLAYER_IMG_ATTACKING_END   47
#define PLAYER_IMG_ATTACKING_TIME  0.7f

/** Number of rows and cols in film strip */
#define PLAYER_IMG_ROWS 3
#define PLAYER_IMG_COLS 16
#define PLAYER_IMG_SIZE 48

/** Player Texture Key */
#define PLAYER_TEXTURE_KEY_0 "mika_spritesheet"


#pragma mark -
#pragma mark Player Pawn Model

/**
 * PlayerPawnModel
 *
 * Stores player pawn information and AnimationNode for drawing.
 */
class PlayerPawnModel {
protected:
    int _x;
    int _y;
    
    /** Reference to image in SceneGraph for animation */
    std::shared_ptr<cugl::AnimationNode> _sprite;
    
public:
#pragma mark -
#pragma mark Constructors/Destructors
    /** Creates a new enemy at (0, 0) */
    PlayerPawnModel(void) : _x(0), _y(0) { }
    
    /** Destroys enemy, releasing all resources */
    ~PlayerPawnModel(void) { dispose(); }
    
    /** Disposes all resources and assets of this player pawn */
    void dispose();
    
    /** Initialize a new player pawn at (0, 0) */
    virtual bool init() { return init(0, 0); }
    
    /** Initialize a new player pawn at (x, y) */
    virtual bool init(int x, int y);
    
    /** Initialize a new player pawn at (x, y) tile with [tileBounds] */
    virtual bool init(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets);
    
    
#pragma mark -
#pragma mark Static Constructors
    /** Returns newly allocated player pawn at (0, 0) */
    static std::shared_ptr<PlayerPawnModel> alloc() {
        std::shared_ptr<PlayerPawnModel> result = std::make_shared<PlayerPawnModel>();
        return (result->init() ? result : nullptr);
    }
    
    /** Returns newly allocated player pawn at (x, y) */
    static std::shared_ptr<PlayerPawnModel> alloc(int x, int y) {
        std::shared_ptr<PlayerPawnModel> result = std::make_shared<PlayerPawnModel>();
        return (result->init(x, y) ? result : nullptr);
    }
    
    /** Returns newly allocated player pawn at (x, y) tile with [tileBounds] */
    static std::shared_ptr<PlayerPawnModel> alloc(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<PlayerPawnModel> result = std::make_shared<PlayerPawnModel>();
        return (result->init(x, y, tileBounds, assets) ? result : nullptr);
    }
    
    
#pragma mark -
#pragma mark Accessors/Mutators
    /** Returns x coordinate */
    int getX() const { return _x; }
    
    /** Returns y coordinate */
    int getY() const { return _y; }
    
    /** Set x */
    void setX(int x) { _x = x; }
    
    /** Set y */
    void setY(int y) { _y = y; }
    
    /** Set (x, y) */
    void setXY(int x, int y) {
        _x = x;
        _y = y;
    }
    
    
#pragma mark -
#pragma mark Animation
    /** Returns a reference to the film strip */
    std::shared_ptr<cugl::AnimationNode>& getSprite() { return _sprite; }
    
    /** Set sprite bounds from tile [tileBounds] */
    void setSpriteBounds(cugl::Rect tileBounds);
    
    /** Sets the film strip */
    void setSprite(const std::shared_ptr<cugl::AnimationNode>& sprite) { _sprite = sprite; }
    
};

#endif /* __PLAYER_PAWN_MODEL_H__ */


