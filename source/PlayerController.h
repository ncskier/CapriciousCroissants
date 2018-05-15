//
//  PlayerController.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Player_Controller_H__
#define __Player_Controller_H__

#include <cugl/cugl.h>
#include <set>
#include "InputController.h"
#include "BoardModel.h"
#include "TileModel.h"
#include "EntityManager.h"


class PlayerController {
protected:
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    std::shared_ptr<InputController> _input;
    
    // MODEL
    /** The action manager. */
    std::shared_ptr<cugl::ActionManager> _actions;
    /** Set of interrupting animations */
    std::set<std::string> _interruptingActions;
    /** Game board */
    std::shared_ptr<BoardModel> _board;
	/** Entity Manager */
	std::shared_ptr<EntityManager> _entityManager;
    /** NUmber of player moves */
    int _numberMoves = 0;
    
    /** Whether we have completed the player's turn */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;

	bool drawX = false;
	bool drawY = false;
    
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    PlayerController();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~PlayerController() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, and starts the player turn
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param board    The game board
     * @param input    The input controller
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(std::shared_ptr<cugl::ActionManager>& actions, const std::shared_ptr<BoardModel>& board, std::shared_ptr<InputController>& input, std::shared_ptr<EntityManager>& manager);
    
    
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
     * Sets whether the player turn is completed.
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value) { _complete = value; }
    
    /** Returns interrupting actions */
    std::set<std::string>& getInterruptingActions() { return _interruptingActions; }
    
    /** Returns the number of player moves */
    int getNumberMoves() { return _numberMoves; }
    
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Calculate real offset from input offset
     *
     * @return {row, offsetValue} where [row] is true if the row is offset and false if the column is offset
     */
    std::tuple<bool, float> calculateOffset(cugl::Vec2 inputOffset);
    
    /**
     * The method called to update the player turn.
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

#endif /* __Player_Controller_H__ */
