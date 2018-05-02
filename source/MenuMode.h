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

#define MENU_TILE_KEY_0 "menu_tile0_strip"
//#define MENU_TILE_KEY_1 "menu_tile1"
//#define MENU_TILE_KEY_2 "menu_tile2"
#define MENU_DOT_KEY "menu_dot"

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
    std::vector<std::shared_ptr<cugl::Button>> _menuButtons;
    cugl::Size _menuTileSize;
    std::shared_ptr<cugl::Node> _mikaNode;
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<InputController>& input);

    
#pragma mark -
#pragma mark Helper Functions
    /** Load levels from json */
    void loadLevelsFromJson(const std::string& filePath);
    
    /** Create level node */
    std::shared_ptr<cugl::Node> createLevelNode(int levelIdx);
    
    /** Create mika node */
    std::shared_ptr<cugl::Node> createMikaNode();
    
    /** Update mika node */
    void updateMikaNode();
    
    /** Update selected level */
    void updateSelectedLevel();
    
    /** Calculate menu tile position given the level index */
    cugl::Vec2 menuTilePosition(int levelIdx);
    
    /** Set menu tile size */
    void setMenuTileSize();
    
    /** Apply offset cap function to difference between offset min/max and movement offset */
    float applyOffsetCapFunction(float diff);
    
    /** Return level spritesheet frame given the level index [levelIdx] */
    int menuTileFrame(int levelIdx);
    
    /** Return horizontal position of level dot as fraction of level width */
    float levelFractionX(int levelIdx);
    
    /** Return true if touch selected the level */
    bool touchSelectedLevel(cugl::Vec2 touchPosition);
    
    
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
};

#endif /* __Menu_Mode_H__ */
