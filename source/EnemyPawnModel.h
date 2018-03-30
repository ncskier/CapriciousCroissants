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
#define ENEMY_IMG_NORTH 2
#define ENEMY_IMG_SOUTH 1
#define ENEMY_IMG_EAST  3
#define ENEMY_IMG_WEST  0

/** Number of rows and cols in film strip */
#define ENEMY_IMG_ROWS 1
#define ENEMY_IMG_COLS 4
#define ENEMY_IMG_SIZE 4

/** Movement animation speed (tiles/sec) */
#define ENEMY_IMG_SPEED 2.0f

/** Enemy Texture Key */
#define ENEMY_TEXTURE_KEY_0 "enemy0_strip"
#define ENEMY_TEXTURE_KEY_1 "enemy1_strip"


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
	int _ai;

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
    
    /** Initialize a new player pawn at (x, y) tile with [tileBounds] facing NORTH */
    virtual bool init(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets);
    
    /** Initialize a new player pawn at (x, y) tile with [tileBounds] facing [direction] */
    virtual bool init(int x, int y, Direction direction, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets);
    
    /** Initialize a new player pawn at (x, y) tile with [tileBounds] facing [direction] with [smart] AI */
    virtual bool init(int x, int y, Direction direction, bool smart, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets);

    
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
    
    /** Returns newly allocated player pawn at (x, y) tile with [tileBounds] facing NORTH */
    static std::shared_ptr<EnemyPawnModel> alloc(int x, int y, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<EnemyPawnModel> result = std::make_shared<EnemyPawnModel>();
        return (result->init(x, y, tileBounds, assets) ? result : nullptr);
    }
    
    /** Returns newly allocated player pawn at (x, y) tile with [tileBounds] facing [direction] */
    static std::shared_ptr<EnemyPawnModel> alloc(int x, int y, Direction direction, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<EnemyPawnModel> result = std::make_shared<EnemyPawnModel>();
        return (result->init(x, y, direction, tileBounds, assets) ? result : nullptr);
    }
    
    /** Returns newly allocated player pawn at (x, y) tile with [tileBounds] facing [direction] with [smart] AI */
    static std::shared_ptr<EnemyPawnModel> alloc(int x, int y, Direction direction, bool smart, cugl::Rect tileBounds, std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<EnemyPawnModel> result = std::make_shared<EnemyPawnModel>();
        return (result->init(x, y, direction, smart, tileBounds, assets) ? result : nullptr);
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
	void setDirection(int direction) {
		_direction = (Direction)(direction);
		updateSpriteDirection();
	}
    
    /** Step one unit forward in the direction the enemy is facing */
    void step();
    
    /** Turn 180 degrees around */
    void turnAround();
    
    /** Move 1 space forward and turn around if blocked */
    void move(int boardWidth, int boardHeight);
    
    /** Set random direction */
    void setRandomDirection();

	/**Set AI**/
	void setAI();

	int getAI() const { return _ai; }
    
    
#pragma mark -
#pragma mark Animation
    /** Returns a reference to the film strip */
    std::shared_ptr<cugl::AnimationNode>& getSprite() { return _sprite; }
    
    /** Set sprite bounds from tile [tileBounds] */
    void setSpriteBounds(cugl::Rect tileBounds);
    
    /** Set sprite according to direction */
    void updateSpriteDirection();
    
    /** Sets the film strip */
    void setSprite(const std::shared_ptr<cugl::AnimationNode>& sprite) { _sprite = sprite; }
    
};

#endif /* __ENEMY_PAWN_MODEL_H__ */

