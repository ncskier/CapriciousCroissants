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

#define MENU_TILE_KEY_0 "menu_tile0"
#define MENU_TILE_KEY_1 "menu_tile1"
#define MENU_TILE_KEY_2 "menu_tile2"


class MenuMode : public cugl::Scene {
protected:
    /** The asset manager for the menu. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // TODO: VIEW
    cugl::Size _dimen;
    std::shared_ptr<cugl::Node> _worldNode;
    std::vector<std::shared_ptr<cugl::Node>> _menuTiles;
    cugl::Size _menuTileSize;
    
    // TODO: MODEL
    std::shared_ptr<cugl::JsonValue> _levelsJson;
    std::string _selectedLevelJson;
    int _selectedLevel = 0;
    float _softOffset = 0.0f;
    float _hardOffset = 0.0f;
    InputController _input;
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    
#pragma mark -
#pragma mark Helper Functions
    /** Load levels from json */
    void loadLevelsFromJson(const std::string& filePath);
    
    /** Create level node */
    std::shared_ptr<cugl::Node> createLevelNode(int levelIdx);
    
    /** Calculate menu tile position given the level index */
    cugl::Vec2 menuTilePosition(int levelIdx);
    
    
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
