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
#include "GameData.h"

/** In-Game Menu */
#define PLAY_MENU_KEY_TOGGLE_OUT "play_menu_toggle_out"
#define PLAY_MENU_KEY_TOGGLE_IN  "play_menu_toggle_in"
#define PLAY_MENU_KEY_SOUND      "play_menu_sound"
#define PLAY_MENU_KEY_RESTART    "play_menu_restart"
#define PLAY_MENU_KEY_EXIT       "play_menu_exit"
#define PLAY_MENU_KEY_BACKGROUND "play_menu_background"
#define PLAY_MENU_LISTENER_TOGGLE_OUT 50
#define PLAY_MENU_LISTENER_TOGGLE_IN  51
#define PLAY_MENU_LISTENER_SOUND      52
#define PLAY_MENU_LISTENER_RESTART    53
#define PLAY_MENU_LISTENER_EXIT       54
/** WinLose Screen */
#define WIN_LOSE_BACKGROUND_WIN  "winlose-background-win"
#define WIN_LOSE_BACKGROUND_LOSE "winlose-background-lose"
#define WIN_LOSE_MIKA_WIN        "winlose-mika-win"
#define WIN_LOSE_MIKA_LOSE       "winlose-mika-lose"
#define WIN_LOSE_CONTINUE        "winlose-continue"
#define WIN_LOSE_RETRY           "winlose-retry"
#define WIN_LOSE_LEVELS_WIN      "winlose-levels-win"
#define WIN_LOSE_LEVELS_LOSE     "winlose-levels-lose"
#define WIN_LOSE_LISTENER_CONTINUE 60
#define WIN_LOSE_LISTENER_RETRY    61
#define WIN_LOSE_LISTENER_LEVELS   62


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
    float winTimer = 0.0f;
    bool winAnimationDisappear = true;
    bool winAnimationAppear = true;
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
    
    /** Level json */
    int _level;
    
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    std::shared_ptr<InputController> _input;
    PlayerController _playerController;
    BoardController _boardController;
    EnemyController _enemyController;

	//VIEW
	//Buttons
	std::shared_ptr<cugl::Button> _resetButton;
    std::shared_ptr<cugl::Button> _soundButton;
    std::shared_ptr<cugl::Button> _exitButton;
    std::shared_ptr<cugl::AnimationNode> _soundSprite;
    std::shared_ptr<cugl::PolygonNode> _menuNode;
    cugl::Size _dimen;
    
    /** WinLose Menu */
    bool _winloseActive;
    std::shared_ptr<cugl::Node> _winloseNode;
    std::shared_ptr<cugl::Button> _winloseContinueButton;
    std::shared_ptr<cugl::Button> _winloseRetryButton;
    std::shared_ptr<cugl::Button> _winloseLevelsButton;
    
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
    void setupLevelFromJson(cugl::Size dimen);
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<InputController>& input, int level);
    
    
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
#pragma mark Helper Functions
    /** Initialize in-game menu */
    void initMenu();
    
    
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
    
    /** Update interrupting win animation if player has won */
    void updateWinAnimation(float dt);
    
    /** Reset level/PlayMode */
    void reset();
    
    /** Exits the game */
    void exit();
    
    /** Toggle sound */
    void toggleSound();
    
#pragma mark -
#pragma mark WinLose Menu
    /** Initialize WinLose Menu */
    void initWinLose();
    
    /** Go to the next level. */
    void nextLevel();
    
    /** Go to the level select screen. */
    void levelMenu();
    
    /** Retry the level. */
    void retryLevel();
    
    /** Return the level */
    int getLevel() { return _level; }
};

#endif /* __Play_Mode_H__ */
