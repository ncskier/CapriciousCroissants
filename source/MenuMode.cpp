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
bool MenuMode::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController>& input) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }
    
    // Initialize
    _assets = assets;
    _dimen = dimen;
    
    // Initialize Input Handler
    _input = input;
    _input->init(getCamera());
    _input->clear();
    
    // Initialize View
    _worldNode = Node::allocWithBounds(_dimen);
    _worldNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldNode->setPosition(Vec2::ZERO);
    _worldNode->doLayout(); // This rearranges the children to fit the screen
    addChild(_worldNode);
    // Background color
    Application::get()->setClearColor(Color4(192,192,192,255));
    
    // Load levels
    _selectedLevel = 0;
    _softOffset = 0.0f;
    _hardOffset = 0.0f;
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
    for (auto i = 0; i < _menuButtons.size(); i++) {
        _menuButtons[i]->deactivate();
    }
    removeAllChildren();
    _assets = nullptr;
    _worldNode = nullptr;
    _levelsJson = nullptr;
    _input = nullptr;
    _active = false;
    _menuTiles.clear();
    _menuButtons.clear();
    _selectedLevel = 0;
    _softOffset = 0.0f;
    _hardOffset = 0.0f;
    _minOffset = 0.0f;
    _maxOffset = 0.0f;
    _originY = 0.0f;
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
    _originY = _dimen.height*0.1f;
    _maxOffset = 2.0f*_originY - _dimen.height + _menuTileSize.height*_levelsJson->size();
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
    std::shared_ptr<Font> font = _assets->get<Font>("script");
    std::shared_ptr<Label> levelLabel = Label::alloc(ss.str(), font);
    levelLabel->setBackground(Color4::BLACK);
    levelLabel->setForeground(Color4::WHITE);
    std::shared_ptr<Button> levelButton = Button::alloc(levelLabel);
    levelButton->setAnchor(Vec2::ANCHOR_CENTER);
    levelButton->setName(ss.str());
    
    // Initialize node
    std::shared_ptr<PolygonNode> menuTile = PolygonNode::allocWithTexture(_assets->get<Texture>(MENU_TILE_KEY_0));
    menuTile->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    menuTile->setContentSize(_menuTileSize);
    menuTile->setPosition(menuTilePosition(levelIdx));
    menuTile->setName(ss.str());
    
    // Set Button Position
    float frameLength = _menuTileSize.height*0.5f;
    levelButton->setPosition(_menuTileSize.width*0.5f, _menuTileSize.height*0.5f);
    levelButton->setContentSize(frameLength, frameLength);
    levelLabel->setContentSize(frameLength, frameLength);

    // Set Button Callback
    levelButton->setListener([=](const std::string& name, bool down) {
        if (!down) {
            int i = std::stoi(name);
            this->_selectedLevelJson = _levelsJson->get(i)->asString();
            this->setActive(false);
        }
    });
    levelButton->activate(100+levelIdx);
    menuTile->addChild(levelButton);
    _menuButtons.push_back(levelButton);
    
    return menuTile;
}

/** Calculate menu tile position given the level index */
cugl::Vec2 MenuMode::menuTilePosition(int levelIdx) {
    float positionY = _originY + _menuTileSize.height*levelIdx - _softOffset;
    return Vec2(0.0f, positionY);
}

/** Apply offset cap function to difference between offset min/max and movement offset */
float MenuMode::applyOffsetCapFunction(float diff) {
    float converter = _menuTileSize.height*0.1f;        // Lower value means cap will converge faster
    float x = diff / converter;
    float y = std::log(std::pow(x+1, 2));               // Lower power will converge faster
    return y * converter;
}


#pragma mark -
#pragma mark Input Handling
/**
 * The method called to update the game mode.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MenuMode::update(float timestep) {
    // Handle Input
    InputController::MoveEvent moveEvent = _input->getMoveEvent();
    if (moveEvent != InputController::MoveEvent::NONE) {
        // Calculate Menu Tile offsets
        Vec2 moveOffset = _input->getMoveOffset();
        if (moveEvent == InputController::MoveEvent::START) {
            _hardOffset = _softOffset;
            _input->recordMove();
        } else if (moveEvent != InputController::MoveEvent::END) {
            _softOffset = _hardOffset - moveOffset.y;
            // Check drag bounds (top & bottom)
            if (_softOffset > _maxOffset) {
                float diff = std::abs(_softOffset-_maxOffset);
                _softOffset = _maxOffset + applyOffsetCapFunction(diff);
            } else if (_softOffset < _minOffset) {
                float diff = std::abs(_softOffset-_minOffset);
                _softOffset = _minOffset - applyOffsetCapFunction(diff);
            }
        } else {
            _hardOffset -= moveOffset.y;
            // Check draw bounds (top & bottom)
            if (_hardOffset > _maxOffset) { _hardOffset = _maxOffset; }
            if (_hardOffset < _minOffset) { _hardOffset = _minOffset; }
            _input->clear();
        }
    } else {
        if (_softOffset != _hardOffset) {
            float diff = std::abs(_softOffset - _hardOffset);
            float velocity = diff*10.0f;
            float dx = velocity * timestep;
            if (_softOffset < _hardOffset) {
                _softOffset = _softOffset + dx;
                if (_softOffset > _hardOffset) { _softOffset = _hardOffset; }
            } else {
                _softOffset = _softOffset - dx;
                if (_softOffset < _hardOffset) { _softOffset = _hardOffset; }
            }
        }
    }
    // Move Menu Tiles
    for (auto i = 0; i < _menuTiles.size(); i++) {
        _menuTiles[i]->setPosition(menuTilePosition(i));
    }
}
