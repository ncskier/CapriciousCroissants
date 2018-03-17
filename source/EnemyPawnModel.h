//
//  EnemyPawnModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/17/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __ENEMY_PAWN_MODEL_H__
#define __ENEMY_PAWN_MODEL_H__

#include <cugl/cugl.h>

/** Tile Frame Sprite numbers */
#define ENEMY_IMG_NORMAL 0

/** Number of rows and cols in film strip */
#define ENEMY_IMG_ROWS 5
#define ENEMY_IMG_COLS 5
#define ENEMY_IMG_SIZE 25


#pragma mark -
#pragma mark Enemy Pawn Model

/**
 * EnemyPawnModel
 *
 * Stores enemy information and AnimationNode for drawing.
 */
class EnemyPawnModel {
public:
    /*
     * Direction Enemey is facing
     *   N
     * W + E
     *   S
     */
    enum Direction : unsigned int {
        NORTH = 0,
        SOUTH = 1,
        EAST = 2,
        WEST = 3
    };
protected:
    int _x;
    int _y;
    Direction _direction;
    
    /** Reference to image in SceneGraph for animation */
    std::shared_ptr<cugl::AnimationNode> _sprite;
    
public:
#pragma mark -
#pragma mark Constructors/Destructors
    /** Creates a new enemy at (0, 0) facing NORTH */
    EnemyPawnModel(void) : _x(0), _y(0), _direction(Direction::NORTH) { }
    
    /** Destroys enemy, releasing all resources */
    ~EnemyPawnModel(void) { dispose(); }
    
    /** Disposes all resources and assets of this enemy */
    void dispose();
    
    /** Initialize a new enemy at (0, 0) facing a random direction */
    virtual bool init() { return init(0, 0); }
    
    /** Initialize a new enemy at (x, y) facing a random direction */
    virtual bool init(int x, int y);
    
    /** Initialize a new enemy at (x, y) facing [direction] */
    virtual bool init(int x, int y, Direction direction);
    
    
#pragma mark -
#pragma mark Static Constructors
    /** Returns newly allocated enemy at (0, 0) facing NORTH */
    static std::shared_ptr<EnemyPawnModel> alloc() {
        std::shared_ptr<EnemyPawnModel> result = std::make_shared<EnemyPawnModel>();
        return (result->init() ? result : nullptr);
    }
    
    /** Returns newly allocated enemy at (x, y) facing NORTH */
    static std::shared_ptr<EnemyPawnModel> alloc(int x, int y) {
        std::shared_ptr<EnemyPawnModel> result = std::make_shared<EnemyPawnModel>();
        return (result->init(x, y) ? result : nullptr);
    }
    
    /** Returns newly allocated enemy at (x, y) facing [direction] */
    static std::shared_ptr<EnemyPawnModel> alloc(int x, int y, Direction direction) {
        std::shared_ptr<EnemyPawnModel> result = std::make_shared<EnemyPawnModel>();
        return (result->init(x, y, direction) ? result : nullptr);
    }
    
    
#pragma mark -
#pragma mark Accessors/Mutators
    /** Returns x coordinate */
    int getX() const { return _x; }
    
    /** Returns y coordinate */
    int getY() const { return _y; }
    
    /** Returns direction */
    Direction getDirection() const { return _direction; }
    
    /** Set x */
    void setX(int x) { _x = x; }
    
    /** Set y */
    void setY(int y) { _y = y; }
    
    /** Set (x, y) */
    void setXY(int x, int y) {
        _x = x;
        _y = y;
    }
    
    /** Set direction */
    void setDirection(Direction direction) { _direction = direction; }
    
    
#pragma mark -
#pragma mark Animation
    /** Returns a reference to the film strip */
    std::shared_ptr<cugl::AnimationNode>& getSprite() { return _sprite; }
    
    /** Sets the film strip */
    void setSprite(const std::shared_ptr<cugl::AnimationNode>& sprite) { _sprite = sprite; }
    
};

#endif /* __ENEMY_PAWN_MODEL_H__ */

