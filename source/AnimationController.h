//
//  AnimationController.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 5/16/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef AnimationController_hpp
#define AnimationController_hpp

#include <cugl/cugl.h>

class AnimationController {
private:
    /** Reference to AnimationController singleton */
    static AnimationController* _gController;
    
    /** Reference to ActionManager */
    std::shared_ptr<cugl::ActionManager> _actions;
    
    /***************************************************** THIS CLASS IS NOT USED *************************************************************/
    
#pragma mark -
#pragma mark Constructors
    /** Constructor */
    AnimationController();
    
    /** Destructor */
    ~AnimationController() { dispose(); }
    
    /** Dispose of all (non-static) resources */
    void dispose();
    
    /** Initialize AnimationController */
    bool init();
    
#pragma mark -
#pragma mark Helper Methods
    
    
public:
#pragma mark -
#pragma mark Static Accessors
    /**
     * Returns the singleton instance of the Animation Controller.
     *
     * If the Animation Controller has not been started, then this method will return
     * nullptr.
     *
     * @return the singleton instance of the Animation Controller.
     */
    static AnimationController* get() { return _gController; }
    
    /** Start the AnimationController */
    static void start();
    
    /**
     * Stops the singleton Animation Controller, releasing all resources.
     *
     * Once this method is called, the method get() will return nullptr.
     * Calling the method multiple times (without calling stop) will have
     * no effect.
     */
    static void stop();
    
#pragma mark -
#pragma mark Non-Static Accessors/Mutators
    /** Return ActionManager */
    std::shared_ptr<cugl::ActionManager> getActionManager() { return _actions; }
};

#endif /* AnimationController_hpp */
