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
#include "InputController.h"
#include "BoardModel.h"


class PlayerController {
protected:
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    std::shared_ptr<InputController> _input;
    
    // MODEL
    std::shared_ptr<BoardModel> _board;
    
    /** Whether we have completed the player's turn */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    
    
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
    bool init(const std::shared_ptr<BoardModel>& board, const std::shared_ptr<InputController>& input);
    
    
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
    
    
#pragma mark -
#pragma mark Gameplay Handling
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
