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
#define MENU_MIKA_NAME "mika"


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
    _softOffset = offsetForLevel(_selectedLevel);
    _hardOffset = offsetForLevel(_selectedLevel);
    setMenuTileSize();
    loadLevelsFromJson("json/levelList.json");
    
    // Create Mika Node
    _mikaNode = createMikaNode();
    _worldNode->addChild(_mikaNode);
    updateMikaNode();
    
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
    _mikaNode = nullptr;
    _active = false;
    _menuTiles.clear();
    _menuButtons.clear();
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
    _maxOffset = _menuTileSize.height * (_levelsJson->size() - 1);
    for (auto i = 0; i < _levelsJson->size(); i++) {
        std::shared_ptr<Node> menuTile = createLevelNode(i);
        _worldNode->addChild(menuTile);
        _menuTiles.push_back(menuTile);
    }
    
}

/** Create level node */
std::shared_ptr<Node> MenuMode::createLevelNode(int levelIdx) {
    // Get level string
    std::stringstream ss;
    ss << levelIdx;

    // Initialize node
    std::shared_ptr<AnimationNode> menuTile = AnimationNode::alloc(_assets->get<Texture>(MENU_TILE_KEY_0), MENU_TILE_ROWS, MENU_TILE_COLS, MENU_TILE_SIZE);
    menuTile->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    menuTile->setContentSize(_menuTileSize);
    menuTile->setPosition(menuTilePosition(levelIdx));
    menuTile->setName(ss.str());
    menuTile->setFrame(menuTileFrame(levelIdx));
    
//    // Initialize Level Button Node
//    std::shared_ptr<Font> font = _assets->get<Font>("script");
//    std::shared_ptr<Label> levelLabel = Label::alloc(ss.str(), font);
//    levelLabel->setBackground(Color4::BLACK);
//    levelLabel->setForeground(Color4::WHITE);
//    std::shared_ptr<Button> levelButton = Button::alloc(levelLabel);
//    levelButton->setAnchor(Vec2::ANCHOR_CENTER);
//    levelButton->setName(ss.str());
//
//    // Set Button Position
//    float frameLength = _menuTileSize.height*0.5f;
//    levelButton->setPosition(_menuTileSize.width*0.5f, _menuTileSize.height*0.5f);
//    levelButton->setContentSize(frameLength, frameLength);
//    levelLabel->setContentSize(frameLength, frameLength);
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
//    menuTile->addChild(levelButton);
//    _menuButtons.push_back(levelButton);
    
    // Initialize Level Dot
    std::shared_ptr<PolygonNode> levelDot = PolygonNode::allocWithTexture(_assets->get<Texture>(MENU_DOT_KEY));
    levelDot->setAnchor(Vec2::ANCHOR_CENTER);
    float height = _menuTileSize.height*0.5f;
    float width = levelDot->getContentSize().width/levelDot->getContentSize().height * height;
    levelDot->setContentSize(width, height);
    levelDot->setPosition(_menuTileSize.width*levelFractionX(levelIdx), _menuTileSize.height*0.5f);
    menuTile->addChild(levelDot);
    
    return menuTile;
}

/** Create mika node */
std::shared_ptr<Node> MenuMode::createMikaNode() {
    // Null Tile Sprite
    std::shared_ptr<AnimationNode> nullTileSprite = AnimationNode::alloc(_assets->get<Texture>(TILE_TEXTURE_KEY_NULL), TILE_IMG_ROWS, TILE_IMG_COLS, TILE_IMG_SIZE);
    nullTileSprite->setAnchor(Vec2::ANCHOR_CENTER);
    nullTileSprite->setFrame(TILE_IMG_NORMAL);
    float length = _menuTileSize.height*0.6f;
    nullTileSprite->setContentSize(length, length);
    
    // Mika Sprite
    std::shared_ptr<AnimationNode> mikaSprite = AnimationNode::alloc(_assets->get<Texture>(PLAYER_TEXTURE_KEY_0), PLAYER_IMG_ROWS, PLAYER_IMG_COLS, PLAYER_IMG_SIZE);
    mikaSprite->setAnchor(Vec2::ANCHOR_CENTER);
    mikaSprite->setFrame(PLAYER_IMG_NORMAL);
    float height = nullTileSprite->getContentSize().height*1.3f;
    float width = mikaSprite->getContentSize().width/mikaSprite->getContentSize().height * height;
    mikaSprite->setContentSize(width, height);
    float x = nullTileSprite->getContentSize().width*0.5f;
    float y = nullTileSprite->getContentSize().height*0.9f;
    mikaSprite->setPosition(x, y);
    
    // Combine and return
    nullTileSprite->addChildWithName(mikaSprite, MENU_MIKA_NAME);
    return nullTileSprite;
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

/** Calculate level spritesheet frame given the level index [levelIdx] */
int MenuMode::menuTileFrame(int levelIdx) {
    return MENU_TILE_SIZE - (levelIdx % MENU_TILE_SIZE) - 1;
}

/** Return offset for level at [levelIdx] */
float MenuMode::offsetForLevel(int levelIdx) {
    return _menuTileSize.height*levelIdx;
}

/** Return horizontal position of level dot as fraction of level width */
float MenuMode::levelFractionX(int levelIdx) {
    // In reverse order than on screen
    float xFractions[MENU_TILE_SIZE];
    xFractions[0] = 0.5f;
    xFractions[1] = 0.4f;
    xFractions[2] = 0.59f;
    xFractions[3] = 0.31f;
    xFractions[4] = 0.39f;
    xFractions[5] = 0.75f;
    return xFractions[menuTileFrame(levelIdx)];
}

/** Set menu tile size */
void MenuMode::setMenuTileSize() {
    std::shared_ptr<AnimationNode> tileNode = AnimationNode::alloc(_assets->get<Texture>(MENU_TILE_KEY_0), MENU_TILE_ROWS, MENU_TILE_COLS, MENU_TILE_SIZE);
    Size tileSize = tileNode->getContentSize();
    float width = _dimen.width;
    float height = (tileSize.height / tileSize.width) * width;
    _menuTileSize = Size(width, height);
}

/** Update mika node */
void MenuMode::updateMikaNode() {
    // Update position
    float dragY = _softOffset / _menuTileSize.height;
    float fraction = dragY - std::round(dragY);
    int closestLevelIdx = (fraction < 0) ? _selectedLevel-1 : _selectedLevel+1;
    fraction = std::abs(fraction);
    float x = ((1.0f-fraction)*levelFractionX(_selectedLevel) + fraction*levelFractionX(closestLevelIdx)) * _menuTileSize.width;
    float y = _originY + _menuTileSize.height*0.5f;     // Menu Tiles are anchored in bottom left
    // Check for caps
    bool dragLowCap = dragY < 0.0f;
    bool dragHighCap = dragY > (double)(_levelsJson->size()-1.0f);
    if (dragLowCap || dragHighCap) {
        // Interpolate towards center
        fraction = dragLowCap ? std::ceil(dragY) - dragY : dragY - std::floor(dragY);
        x = ((1.0f-fraction)*levelFractionX(_selectedLevel) + fraction*0.5f) * _menuTileSize.width;
        if (fraction > 1.0f) { x = 0.5f * _menuTileSize.width; }
    }
    _mikaNode->setPosition(x, y);
}

/** Update selected level */
void MenuMode::updateSelectedLevel() {
    _selectedLevel = (int)std::round(_softOffset / _menuTileSize.height);
    // Check for caps
    if (_selectedLevel < 0) { _selectedLevel = 0; }
    if (_selectedLevel > _levelsJson->size()-1) { _selectedLevel = (int)(_levelsJson->size()-1); }
}

/** Return true if touch selected the level */
bool MenuMode::touchSelectedLevel(cugl::Vec2 touchPosition) {
    bool inNullTile = _mikaNode->getBoundingBox().contains(touchPosition);
    bool inMikaSprite = _mikaNode->getChildByName(MENU_MIKA_NAME)->getBoundingBox().contains(touchPosition);
    return inNullTile || inMikaSprite;
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
            // Check if player tapped on Mika
            Vec2 touchPosition = _input->getTouchPosition();
            if (touchSelectedLevel(touchPosition)) {
                // Select level and exit Level Select Menu
                this->_selectedLevelJson = _levelsJson->get(_selectedLevel)->asString();
                this->setActive(false);
            }
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
            _input->clear();
        }
    }
    
    // Update
    updateSelectedLevel();
    updateMikaNode();
    
    // Relax back to selected level
    if (moveEvent == InputController::MoveEvent::END) {
        _hardOffset = offsetForLevel(_selectedLevel);
    }
    if (moveEvent == InputController::MoveEvent::NONE || moveEvent == InputController::MoveEvent::END) {
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
