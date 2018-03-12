//
//  InputController.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Input_Controller_H__
#define __Input_Controller_H__

#include <cugl/cugl.h>


/**
 * This class represents player input.
 *
 * This input handler uses the CUGL input API.  It uses the polling API for
 * keyboard, but the callback API for touch.  This demonstrates a mix of ways
 * to handle input, and the reason for hiding it behind an abstraction like
 * this class.
 *
 * Unlike CUGL input devices, this class is not a singleton.  It must be
 * allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this
 * controller as a field without using pointers. We simply add the class to the
 * header file of its owner, and delay initialization (via the method init())
 * until later. This is one of the main reasons we like to avoid initialization
 * in the constructor.
 */
class InputController {
public:
    /*
     * Events that can occur for on move in the game
     */
    enum MoveEvent : unsigned int {
        // No attempt at a move (no touch event)
        NONE = 0,
        // A move started (or tried to start)
        START = 1,
        // A move continued
        MOVING = 2,
        // A move ended
        END = 3
    };

private:
    /** Whether or not this input is active */
    bool _active;
    /** Scale */
//    cugl::Size _scale;
    std::shared_ptr<cugl::Camera> _camera;
    
    // TOUCH SUPPORT
    /** The initial touch location for the current gesture */
    cugl::Vec2 _initTouch;
    /** The touch id of the touch that starts the move */
    cugl::TouchID _touchID;
    /** Move event type */
    MoveEvent _moveEvent;
    
protected:
    // INPUT RESULTS
    /** The current touch location. */
    cugl::Vec2 _touchPosition;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    InputController(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    bool init(std::shared_ptr<cugl::Camera>);
    
#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }
    
    /**
     * Processes the currently cached inputs.
     *
     * This method is used to to poll the current input state.  This will poll the
     * keyboad and accelerometer.
     *
     * This method also gathers the delta difference in the touches. Depending on
     * the OS, we may see multiple updates of the same touch in a single animation
     * frame, so we need to accumulate all of the data together.
     */
    void  update(float dt);
    
    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
    /**
     * The move has been recorded and should now move to the MOVING state.
     */
    void recordMove();
    
#pragma mark -
#pragma mark Input Results
    /**
     * Returns the touch location
     *
     * @return the touch location
     */
    cugl::Vec2 getTouchPosition() const { return _camera->screenToWorldCoords(_touchPosition); }
    
    /**
     * Returns the move offset from the initial position
     *
     * @return the move offset
     */
    cugl::Vec2 getMoveOffset() const { return (_camera->screenToWorldCoords(_touchPosition)-_camera->screenToWorldCoords(_initTouch)); }
    
    /**
     * Returns _moveEvent
     *
     * @return _moveEvent
     */
    MoveEvent getMoveEvent() const { return _moveEvent; }
    
    /**
     * Set _moveEvent
     */
    void setMoveEvent(MoveEvent moveEvent) { _moveEvent = moveEvent; }
    
    
#pragma mark -
#pragma mark Touch Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for the movement of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
    
};

#endif /* __Input_Controller_H__ */
