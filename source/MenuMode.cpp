//
//  MenuMode.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "MenuMode.h"
#include <sstream>

using namespace cugl;

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024


#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
MenuMode::MenuMode() : Scene() {
}

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
bool MenuMode::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }
    
    // Initialize
    CULog("Initialize Menu Mode");
    _assets = assets;
    _dimen = dimen;
    
    // Initialize Input Handler
    _input.init(getCamera());
    
    // Initialize View
    _worldNode = Node::allocWithBounds(_dimen);
    _worldNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldNode->setPosition(Vec2::ZERO);
    _worldNode->doLayout(); // This rearranges the children to fit the screen
    addChild(_worldNode);
    // Background color
    Application::get()->setClearColor(Color4(192,192,192,255));
    
    // Load levels
    Size tileSize = _assets->get<Texture>(MENU_TILE_KEY_1)->getSize();
    float width = _dimen.width;
    // TODO: Change the height when placeholder art is changed
    float height = (tileSize.height / tileSize.width) * width * 0.5f;
//    float height = (tileSize.height / tileSize.width) * width;
    _menuTileSize = Size(width, height);
    loadLevelsFromJson("json/levelList.json");
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuMode::dispose() {
    removeAllChildren();
    _assets = nullptr;
    _worldNode = nullptr;
    _levelsJson = nullptr;
    _input.dispose();
}


#pragma mark -
#pragma mark Helper Functions

/** Load levels from json */
void MenuMode::loadLevelsFromJson(const std::string& filePath) {
    // Load json
    std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(filePath);
    std::shared_ptr<JsonValue> json = reader->readJson();
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
        return;
    }
    
    // Load levels
    _levelsJson = json->get("levels");
    CULog("levels: %d", (int)_levelsJson->size());
    for (auto i = 0; i < _levelsJson->size(); i++) {
        std::shared_ptr<Node> menuTile = createLevelNode(i);
        _worldNode->addChild(menuTile);
        _menuTiles.push_back(menuTile);
    }
    
}

/** Create level node */
std::shared_ptr<Node> MenuMode::createLevelNode(int levelIdx) {
    // Initialize Node
    std::stringstream ss;
    ss << levelIdx;
//    std::shared_ptr<Font> font = _assets->get<Font>("script");
//    std::shared_ptr<Label> levelLabel = Label::alloc(ss.str(), font);
//    levelLabel->setBackground(Color4::BLACK);
//    levelLabel->setForeground(Color4::WHITE);
//    std::shared_ptr<Button> levelButton = Button::alloc(levelLabel);
//    levelButton->setName(ss.str());
    
    // Initialize node
    std::shared_ptr<PolygonNode> menuTile = PolygonNode::allocWithTexture(_assets->get<Texture>(MENU_TILE_KEY_0));
    menuTile->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    menuTile->setContentSize(_menuTileSize);
    menuTile->setPosition(menuTilePosition(levelIdx));
    menuTile->setName(ss.str());
    return menuTile;
    
//    // Set Button Position
//    int levelsWidth = 5;
//    int levelsHeight = 10;
//    float frameWidth = _dimen.width * 5.0f/6.0f;
//    float frameHeight = _dimen.height * 5.0f/6.0f;
//    float x = (levelIdx % levelsWidth) * (frameWidth / levelsWidth) - (frameWidth / 2.0f);
//    float y = (levelsHeight - levelIdx / levelsWidth) * (frameHeight / levelsHeight) - (frameHeight / 2.0f);
//    levelButton->setPosition(x, y);
//    levelButton->setContentSize(frameWidth/levelsWidth*5.0f/6.0f, frameHeight/levelsHeight*5.0f/6.0f);
//    levelLabel->setContentSize(frameWidth/levelsWidth*5.0f/6.0f, frameHeight/levelsHeight*5.0f/6.0f);
//
//    // Set Button Callback
//    levelButton->setListener([=](const std::string& name, bool down) {
//        if (!down) {
//            int i = std::stoi(name);
//            this->_selectedLevelJson = _levelsJson->get(i)->asString();
//            this->setActive(false);
//        }
//    });
//    levelButton->activate(100+levelIdx);
//
//    return levelButton;
}

/** Calculate menu tile position given the level index */
cugl::Vec2 MenuMode::menuTilePosition(int levelIdx) {
    float originY = _dimen.height*0.1f;
    float positionY = originY + _menuTileSize.height*levelIdx - _softOffset;
    
    return Vec2(0.0f, positionY);
}


#pragma mark -
#pragma mark Input Handling
/**
 * The method called to update the game mode.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MenuMode::update(float timestep) {
    InputController::MoveEvent moveEvent = _input.getMoveEvent();
    if (moveEvent != InputController::MoveEvent::NONE) {
        Vec2 moveOffset = _input.getMoveOffset();
        if (moveEvent != InputController::MoveEvent::END) {
            _softOffset = _hardOffset - moveOffset.y;
        } else {
            _hardOffset -= moveOffset.y;
            _softOffset = _hardOffset;
            _input.clear();
        }
        // Move Menu Tiles
        for (auto i = 0; i < _menuTiles.size(); i++) {
            _menuTiles[i]->setPosition(menuTilePosition(i));
        }
    }
}
