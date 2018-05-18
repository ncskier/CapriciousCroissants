//
//  AnimationController.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 5/16/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "AnimationController.h"

using namespace cugl;

/** Reference to AnimationController singleton */
AnimationController* AnimationController::_gController = nullptr;


#pragma mark -
#pragma mark Constructors
/** Constructor */
AnimationController::AnimationController() {
}

/** Dispose of all (non-static) resources */
void AnimationController::dispose() {
    _actions = nullptr;
}

/** Initialize AnimationController */
bool AnimationController::init() {
    _actions = ActionManager::alloc();
    return true;
}


#pragma mark -
#pragma mark Helper Methods


#pragma mark -
#pragma mark Static Accessors
/** Start the AnimationController */
void AnimationController::start() {
    if (_gController != nullptr) {
        return;
    }
    _gController = new AnimationController();
    if (!_gController->init()) {
        delete _gController;
        _gController = nullptr;
        CUAssertLog(false,"Animation Controller failed to initialize");
    }
}

/**
 * Stops the singleton Animation Controller, releasing all resources.
 *
 * Once this method is called, the method get() will return nullptr.
 * Calling the method multiple times (without calling stop) will have
 * no effect.
 */
void AnimationController::stop() {
    if (_gController == nullptr) {
        return;
    }
    delete _gController;
    _gController = nullptr;
}


#pragma mark -
#pragma mark Non-Static Accessors/Mutators


