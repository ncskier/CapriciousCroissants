//
//  MenuMode.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Menu_Mode_H__
#define __Menu_Mode_H__

#include <cugl/cugl.h>
#include <vector>
#include "InputController.h"
#include "PlayerPawnModel.h"
#include "TileModel.h"
#include "GameData.h"

#define MENU_TILE_KEY_0 "menu_tile0_strip"
#define MENU_TILE_KEY_1 "menu_tile1_strip"
#define MENU_TILE_KEY_2 "menu_tile2_strip"
#define MENU_DOT_KEY "menu_dot"
#define MIKA_IDLE_KEY "mika_idle_action"
#define MIKA_TRANSITION_KEY "mika_transition_action"
#define MIKA_ATTACK_KEY "mika_attack_action"
#define MENU_STAR_KEY "winlose-high-star"
#define MENU_STAR_EMPTY_KEY "winlose-high-star-outline"
#define MENU_LABEL_NAME "menu-label"

#define MENU_TILE_ROWS 6
#define MENU_TILE_COLS 1
#define MENU_TILE_SIZE 6


class MenuMode : public cugl::Scene {
protected:
    /** The asset manager for the menu. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // TODO: VIEW
    cugl::Size _dimen;
    std::shared_ptr<cugl::Node> _worldNode;
    std::vector<std::shared_ptr<cugl::Node>> _menuTiles;
    std::vector<std::shared_ptr<cugl::Node>> _menuDots;
    std::vector<std::shared_ptr<cugl::Node>> _menuCapLowTiles;
    std::vector<std::shared_ptr<cugl::Node>> _menuCapHiTiles;
    cugl::Size _menuTileSize;
    cugl::Size _dotSize;
    cugl::Size _starSize;
    std::shared_ptr<cugl::Node> _mikaNode;
    std::shared_ptr<cugl::AnimationNode> _mikaSprite;
    /** The action manager for this game mode. */
    std::shared_ptr<cugl::ActionManager> _actions;
    std::shared_ptr<cugl::Animate> _mikaIdleAction = cugl::Animate::alloc(PLAYER_IMG_IDLE_START, PLAYER_IMG_IDLE_END, PLAYER_IMG_IDLE_TIME);
    std::shared_ptr<cugl::Animate> _mikaTransitionAction = cugl::Animate::alloc(PLAYER_IMG_BEGIN_ATTACK_START, PLAYER_IMG_BEGIN_ATTACK_END, PLAYER_IMG_BEGIN_ATTACK_TIME);
    std::shared_ptr<cugl::Animate> _mikaAttackAction = cugl::Animate::alloc(PLAYER_IMG_ATTACKING_START, PLAYER_IMG_ATTACKING_END, PLAYER_IMG_ATTACKING_TIME);
    
    // TODO: MODEL
    std::shared_ptr<cugl::JsonValue> _levelsJson;
    std::string _selectedLevelJson;
    int _selectedLevel = 0;
    float _softOffset = 0.0f;
    float _hardOffset = 0.0f;
    float _minOffset = 0.0f;
    float _maxOffset = 0.0f;
    float _originY = 0.0f;
    std::shared_ptr<InputController> _input;
    bool _introScroll = true;
    bool _scroll = false;
    float _velocity = 0.0f;
    bool _playSelected = false;
    bool _mikaAttack = false;
    // Cap
    int _lowCapTiles = 6;
    int _hiCapTiles = 8;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    MenuMode();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MenuMode() { dispose(); }
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<InputController>& input, int selectedLevel);
    
    
#pragma mark -
#pragma mark Helper Initialization
    /** Load levels from json */
    void loadLevelsFromJson(const std::string& filePath);
    
    /** Create level node */
    std::shared_ptr<cugl::Node> createLevelNode(int levelIdx, bool cap=false);
    
    /** Create mika node */
    std::shared_ptr<cugl::Node> createMikaNode();
    
    /** Set menu tile size */
    void setMenuTileSize();
    
#pragma mark -
#pragma mark Helper Coordinates
    /** Calculate menu tile position given the level index */
    cugl::Vec2 menuTilePosition(int levelIdx);
    
    /** Calculate dot position */
    cugl::Vec2 dotPosition(int levelIdx);
    
    /** Apply offset cap function to difference between offset min/max and movement offset */
    float applyOffsetCapFunction(float diff);
    
    /** Return true if touch selected the level */
    bool touchSelectedLevel(cugl::Vec2 touchPosition);
    
    /** Returns tapped level and -1 if no level tapped */
    int tappedLevel(cugl::Vec2 touchPosition);
    
    
#pragma mark -
#pragma mark Helper Visuals
    /** Return offset for level at [levelIdx] */
    float offsetForLevel(int levelIdx);
    
    /** Return level spritesheet frame given the level index [levelIdx] */
    int menuTileFrame(int levelIdx);
    
    /** Return horizontal position of level dot as fraction of level width */
    float levelFractionX(int levelIdx);
    
    
#pragma mark -
#pragma mark Menu Tiles
    /** Returns if menu tile at index [i] is on the screen (shouldn't or should hidden) */
    bool menuTileOnScreen(int i);
    
    
#pragma mark -
#pragma mark Helper Input Handling
    /** Update mika node */
    void updateMikaNode();
    
    /** Update mika animation */
    void updateMikaAnimation(float timestep, bool animate);
    
    /** Update selected level */
    void updateSelectedLevel();
    
#pragma mark -
#pragma mark Input Handling
    /**
     * The method called to update the game mode.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    
#pragma mark -
#pragma mark Accessors
    std::string& getSelectedLevelJson() { return _selectedLevelJson; }
    int getSelectedLevel() { return _selectedLevel; }
};

#endif /* __Menu_Mode_H__ */
