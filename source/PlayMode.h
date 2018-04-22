//
//  PlayMode.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Play_Mode_H__
#define __Play_Mode_H__

#include <cugl/cugl.h>
#include "InputController.h"
#include "PlayerController.h"
#include "BoardController.h"
#include "EnemyController.h"
#include "BoardModel.h"
#include "EntityManager.h"
#include "MainSystems.h"

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class PlayMode : public cugl::Scene {
public:
    /**
     * Enum defining the different states of the gameplay loop
     */
    enum State : unsigned int {
        // Player turn
        PLAYER = 0,
        // Board turn
        BOARD  = 1,
        // Enemy turn
        ENEMY  = 2
    };

    
protected:
	bool done = false;
    int doneCtr = 30;
	bool win = false;
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
	std::shared_ptr<cugl::Label> _text;
    
    /** The action manager for this game mode. */
    std::shared_ptr<cugl::ActionManager> _actions;
    
    /** World node for scene graph */
    std::shared_ptr<cugl::Node> _worldNode;
    
    /** Touch Node */
    std::shared_ptr<cugl::AnimationNode> _touchNode;
    std::shared_ptr<cugl::Animate> _touchAction;
    
    /** Mika animation state variables */
    bool _beginAttack = false;
    bool _attacking = false;
    
    /** Game Board */
    std::shared_ptr<BoardModel> _board;
    
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    InputController _input;
    PlayerController _playerController;
    BoardController _boardController;
    EnemyController _enemyController;

	//VIEW
	//Buttons
	std::shared_ptr<cugl::Button> _resetButton;
    
	//Entity Manager Instance
	std::shared_ptr<EntityManager> _entityManager;
    
    /** Current state of the game */
    State _state;
    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    /** Whether or not mode is active */
    bool _active;
    

    
#pragma mark Internal Object Management
    /** Load level from json */
    void setupLevelFromJson(const std::string& filePath, cugl::Size dimen);
    
    /** Add level sprites to scene graph */
    void setupLevelSceneGraph();
    
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    PlayMode();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~PlayMode() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::string& levelJson);
    
    
#pragma mark -
#pragma mark State Access
    /**
     * Returns true if the gameplay controller is currently active
     *
     * @return true if the gameplay controller is currently active
     */
    bool isActive( ) const { return _active; }
    
    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug( ) const { return _debug; }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; }
    
    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }
    
    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value) { _complete = value; }
    
    
#pragma mark -
#pragma mark Gameplay Handling
    /** Update animations */
    void updateAnimations();
    
    /** Update touch node */
    void updateTouchNode();
    
    /** Update Mika animations */
    void updateMikaAnimations();
    
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float dt);
    
    /** Update for player turn */
    void updatePlayerTurn(float dt);
    
    /** Update for board turn */
    void updateBoardTurn(float dt);
    
    /** Update for enemy turn */
    void updateEnemyTurn(float dt);
    
    /** Update interrupting animations (action manager is already updated every iteration) */
    void updateInterruptingAnimations(std::set<std::string>& interruptingActions);
    
    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();
};

#endif /* __Play_Mode_H__ */
