//
//  InputController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "InputController.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE

/** The key for the event handlers */
#define LISTENER_KEY        1


#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputController::InputController() :
_active(false),
//_scale(Size::ZERO),
_touchID(NULL),
_initTouch(Vec2::ZERO),
_prevTouch(Vec2::ZERO),
_touchPosition(Vec2::ZERO),
_moveEvent(InputController::MoveEvent::NONE) {
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
    CULog("dispose InputController");
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Mouse *touch = Input::get<Mouse>();
        touch->removePressListener(LISTENER_KEY);
        touch->removeReleaseListener(LISTENER_KEY);
        touch->removeDragListener(LISTENER_KEY);
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        clear();
        _camera = nullptr;
        _active = false;
    }
}

/**
 * Initializes the input control for the given drawing scale.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * @return true if the controller was initialized successfully
 */
bool InputController::init(std::shared_ptr<Camera> camera) {
    _camera = camera;
    bool success = true;
    
    // Only process keyboard on desktop
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Mouse>();
	Mouse *mouse = Input::get<Mouse>();
	mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
	mouse->addPressListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
		this->touchBeganCB(event, focus);
	});
	mouse->addReleaseListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
		this->touchEndedCB(event, focus);
	});
	mouse->addDragListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, const cugl::Vec2 previous, bool focus) {
		this->touchMovedCB(event, previous,  focus);
	});

	
#else
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY, [=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event, focus);
    });
    touch->addEndListener(LISTENER_KEY, [=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event, focus);
    });
    touch->addMotionListener(LISTENER_KEY, [=](const cugl::TouchEvent& event, const cugl::Vec2 previous, bool focus) {
        this->touchMovedCB(event, previous, focus);
    });
#endif
    _active = success;
    return success;
}


/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void InputController::update(float dt) {
    // TODO: Could have this clear itself when a move is done
    //       just need to make sure the PlayMode controller gets it
    //       before the information is cleared.
    if (_moveEvent != MoveEvent::NONE && _moveEvent != MoveEvent::END) {
        _touchDownTime += dt;
    }
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void InputController::clear() {
    _touchID = NULL;
    _touchPosition = Vec2::ZERO;
    _initTouch = Vec2::ZERO;
    _prevTouch = Vec2::ZERO;
    _moveEvent = MoveEvent::NONE;
    _touchDownTime = 0.0f;
    // Swipe info
    _swipeInitPos = Vec2::ZERO;
    _swipeInitTime = 0.0f;
    _swiping = false;
    _acceleration = 0.0f;
    _velocity = 0.0f;
}

/**
 * The move has been recorded and should now move to the MOVING state.
 */
void InputController::recordMove() {
    _moveEvent = MoveEvent::MOVING;
}

#pragma mark -
#pragma mark Input Results
/** Returns if touch event was a tap by time standards */
bool InputController::isTapTime() {
    float maxTapTime = 0.15f;
    return (_touchDownTime < maxTapTime);
}

#pragma mark -
#pragma mark Touch Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
    // Begin the move event
    if (_moveEvent == MoveEvent::NONE) {
        // Begin move event
        _moveEvent = MoveEvent::START;
        _initTouch = event.position;
        _touchPosition = event.position;
        _prevTouch = event.position;
        _prevTouchTime = _touchDownTime;
        _touchID = event.touch;
    }
}

/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
    if (_moveEvent == MoveEvent::START || _moveEvent == MoveEvent::MOVING) {
        if (event.touch == _touchID) {
            _touchPosition = event.position;
            _moveEvent = MoveEvent::END;
        }
    }
}

/**
 * Callback for the movement of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchMovedCB(const cugl::TouchEvent& event, const Vec2& previous, bool focus) {
    if (_moveEvent != MoveEvent::END) {
        if (event.touch == _touchID) {
            _touchPosition = event.position;
            // Acceleration calculations
            Vec2 diff = event.position;
            diff.subtract(_prevTouch);
            float dy = std::abs(diff.y);
            float dt = _touchDownTime - _prevTouchTime;
            if (dt > 0.001f) {
                dt *= 1000.0f;      // Convert to ms because velocities were too high
                float velocity = dy/dt;
                float acceleration = velocity/dt;
                // Mark if acceleration is greater than threashold
                if (acceleration >= _swipeAccelerationThreshold && _acceleration < _swipeAccelerationThreshold) {
                    _swiping = true;
                    _swipeInitPos = event.position;
                    _swipeInitTime = _touchDownTime;
                } else if (acceleration < _swipeAccelerationThreshold) {
                    _swiping  = false;
                }
                // Update previous touch info
                _prevTouch = event.position;
                _prevTouchTime = _touchDownTime;
                _velocity = velocity;
                _acceleration = acceleration;
            }
        }
    }
}

#pragma mark -
#pragma mark Mouse Callbacks
/**
* Callback for the beginning of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchBeganCB(const cugl::MouseEvent& event, bool focus) {
	// Begin the move event
	if (_moveEvent == MoveEvent::NONE && event.buttons.hasLeft()) {
		// Begin move event
		_moveEvent = MoveEvent::START;
		_initTouch = event.position;
		_touchPosition = event.position;
        _prevTouch = event.position;
        _prevTouchTime = _touchDownTime;
	}
}

/**
* Callback for the end of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchEndedCB(const cugl::MouseEvent& event, bool focus) {
	if (_moveEvent == MoveEvent::START || _moveEvent == MoveEvent::MOVING) {
		if (event.buttons.hasLeft()) {
			_touchPosition = event.position;
			_moveEvent = MoveEvent::END;
		}
	}
}

/**
* Callback for the movement of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchMovedCB(const cugl::MouseEvent& event, const Vec2& previous, bool focus) {
	if (_moveEvent != MoveEvent::END) {
		if (event.buttons.hasLeft()) {
			_touchPosition = event.position;
            // Acceleration calculations
            Vec2 diff = event.position;
            diff.subtract(_prevTouch);
            float dy = std::abs(diff.y);
            float dt = _touchDownTime - _prevTouchTime;
            if (dt > 0.001f) {
                dt *= 1000.0f;      // Convert to ms because velocities were too high
                float velocity = dy/dt;
                float acceleration = velocity/dt;
                // Mark if acceleration is greater than threashold
                if (acceleration >= _swipeAccelerationThreshold && _acceleration < _swipeAccelerationThreshold) {
                    _swiping = true;
                    _swipeInitPos = event.position;
                    _swipeInitTime = _touchDownTime;
                } else if (acceleration < _swipeAccelerationThreshold) {
                    _swiping  = false;
                }
                // Update previous touch info
                _prevTouch = event.position;
                _prevTouchTime = _touchDownTime;
                _velocity = velocity;
                _acceleration = acceleration;
            }
		}
	}
}
