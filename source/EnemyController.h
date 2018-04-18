//
//  EnemyController.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Enemy_Controller_H__
#define __Enemy_Controller_H__

#include <cugl/cugl.h>
#include "BoardModel.h"
#include "PlayerPawnModel.h"
#include "EntityManager.h"


class EnemyController {
public:
    /**
     * Enum defining the different states of the gameplay loop
     */
    enum State : unsigned int {
        // Move enemies
        MOVE   = 0,
        // Enemies attack
        ATTACK = 1,
        // Check for lose
        CHECK  = 2,
    };
protected:
    // CONTROLLERS
    
    // MODEL
    
    
    /** Game board */
    std::shared_ptr<BoardModel> _board;
	
	/** Entity Manager */
	std::shared_ptr<EntityManager> _entityManager;

    /** Whether we have completed the enemy turn */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    
    /** Internal state */
    State _state;
    
    
public:
	/** The action manager. */
	std::shared_ptr<cugl::ActionManager> _actions;
	/** Set of interrupting animations */
	std::set<std::string> _interruptingActions;


	bool lose = false;
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    EnemyController();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~EnemyController() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, and starts the enemy turn
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param board    The game board
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(std::shared_ptr<cugl::ActionManager>& actions, const std::shared_ptr<BoardModel>& board, std::shared_ptr<EntityManager>& manager);
    
    
#pragma mark -
#pragma mark State Access
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
     * Sets whether the enemy turn is completed.
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value) { _complete = value; }
    
    /** Returns interrupting actions */
    std::set<std::string>& getInterruptingActions() { return _interruptingActions; }

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the enemy turn.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();
};

#endif /* __Enemy_Controller_H__ */
