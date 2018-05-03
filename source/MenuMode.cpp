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
    setMenuTileSize();
    loadLevelsFromJson("json/levelList.json");
    
    // Create Mika Node
    _mikaNode = createMikaNode();
    _worldNode->addChild(_mikaNode);
    
    // Begin intro scroll
    _introScroll = true;
    _scroll = false;
    _hardOffset = offsetForLevel(_selectedLevel);
    _softOffset = _hardOffset;
    updateMikaNode();
    _softOffset = 0.0f;
    
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
    _input = nullptr;
    _mikaNode = nullptr;
    _active = false;
    _menuTiles.clear();
    _menuDots.clear();
    _softOffset = 0.0f;
    _hardOffset = 0.0f;
    _minOffset = 0.0f;
    _maxOffset = 0.0f;
    _originY = 0.0f;
    _introScroll = true;
    _playSelected = false;
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
    ss << (levelIdx+1);

    // Initialize node
    std::shared_ptr<AnimationNode> menuTile = AnimationNode::alloc(_assets->get<Texture>(MENU_TILE_KEY_0), MENU_TILE_ROWS, MENU_TILE_COLS, MENU_TILE_SIZE);
    menuTile->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    menuTile->setContentSize(_menuTileSize);
    menuTile->setPosition(menuTilePosition(levelIdx));
    menuTile->setName(ss.str());
    menuTile->setFrame(menuTileFrame(levelIdx));
    
    // Initialize Level Dot
    std::shared_ptr<PolygonNode> levelDot = PolygonNode::allocWithTexture(_assets->get<Texture>(MENU_DOT_KEY));
    levelDot->setAnchor(Vec2::ANCHOR_CENTER);
    float height = _menuTileSize.height*0.5f;
    float width = levelDot->getContentSize().width/levelDot->getContentSize().height * height;
    levelDot->setContentSize(width, height);
    levelDot->setPosition(_menuTileSize.width*levelFractionX(levelIdx), _menuTileSize.height*0.5f);
    menuTile->addChild(levelDot);
    
//    // Initialize Level Button Node
    std::shared_ptr<Font> font = _assets->get<Font>("script");
    std::shared_ptr<Label> levelLabel = Label::alloc(ss.str(), font);
    levelLabel->setAnchor(Vec2::ANCHOR_CENTER);
    levelLabel->setPosition(levelDot->getContentSize().width*0.5f, levelDot->getContentSize().height*0.5f);
    levelDot->addChild(levelLabel);
    _menuDots.push_back(levelDot);
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
    int closestLevelUp = (int)std::floor(dragY);
    int closestLevelDown = (int)std::ceil(dragY);
    float fraction = dragY - (float)closestLevelDown;
//    int closestLevelIdx = (fraction < 0) ? _selectedLevel-1 : _selectedLevel+1;
    fraction = std::abs(fraction);
    float x = ((1.0f-fraction)*levelFractionX(closestLevelDown) + fraction*levelFractionX(closestLevelUp)) * _menuTileSize.width;
    float y = _originY + _menuTileSize.height*0.5f;     // Menu Tiles are anchored in bottom left
    // Check for caps
    bool dragLowCap = dragY < 0.0f;
    bool dragHighCap = dragY > (double)(_levelsJson->size()-1);
    if (dragLowCap || dragHighCap) {
        // Interpolate towards center
        if (dragLowCap) {
            x = ((1.0f-fraction)*levelFractionX(closestLevelDown) + fraction*0.5f) * _menuTileSize.width;
        }
        if (dragHighCap) {
            x = ((1.0f-fraction)*0.5f + fraction*levelFractionX(closestLevelUp)) * _menuTileSize.width;
        }
        if (dragY < -1.0f || (double)_levelsJson->size() < dragY) { x = 0.5f * _menuTileSize.width; }
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
    Vec2 touchPositionNodeCoords = _mikaNode->worldToNodeCoords(touchPosition);
    bool inMikaSprite = _mikaNode->getChildByName(MENU_MIKA_NAME)->getBoundingBox().contains(touchPositionNodeCoords);
    return inNullTile || inMikaSprite;
}

/** Returns tapped level and -1 if no level tapped */
int MenuMode::tappedLevel(cugl::Vec2 touchPosition) {
    for (int i = 0; i < _menuDots.size(); i++) {
        std::shared_ptr<Node> menuTile = _menuTiles[i];
        Vec2 touchPositionNodeCoords = menuTile->worldToNodeCoords(touchPosition);
        if (_menuDots[i]->getBoundingBox().contains(touchPositionNodeCoords)) {
            return i;
        }
    }
    return -1;
}

/** Returns if menu tile at index [i] should be hidden (if it's off the screen) */
bool MenuMode::menuTileOnScreen(int i) {
    float minY = menuTilePosition(i).y;
    float maxY = minY + _menuTileSize.height;
    return ((minY > 0.0f && minY < _dimen.height) || (maxY > 0.0f && maxY < _dimen.height));
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
    _input->update(timestep);
    InputController::MoveEvent moveEvent = _input->getMoveEvent();
    if (!_introScroll && !_scroll) {
        if (moveEvent != InputController::MoveEvent::NONE) {
            // Calculate Menu Tile offsets
            Vec2 moveOffset = _input->getMoveOffset();
            if (moveEvent == InputController::MoveEvent::START) {
                // Check if player tapped on Mika
                _velocity = 0.0f;
                Vec2 touchPosition = _input->getTouchPosition();
                if (touchSelectedLevel(touchPosition)) {
                    _playSelected = true;
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
                if (_playSelected && touchSelectedLevel(_input->getTouchPosition())) {
                    // Select level and exit Level Select Menu
                    this->_selectedLevelJson = _levelsJson->get(_selectedLevel)->asString();
                    this->setActive(false);
                }
                if (_input->isTapTime()) {
                    int level = tappedLevel(_input->getTouchPosition());
                    if (level != -1) {
                        _selectedLevel = level;
                        _hardOffset = offsetForLevel(_selectedLevel);
                        _scroll = true;
                    }
                } else if (_input->isSwipe()) {
                    float time = _input->getSwipeTime();
                    float distance = _input->getSwipeVector().y;
                    _velocity = -distance / time;
//                    CULog("time: %f", time);
//                    CULog("distance: %f", distance);
//                    CULog("init_velocity: %f", _velocity);
                }
                _input->clear();
                _hardOffset = offsetForLevel(_selectedLevel);
            }
        }
    }
    
    // Update
    if (!_introScroll && !_scroll) {
        updateSelectedLevel();
    }
    if (!_introScroll) {
        updateMikaNode();
    }
    
    // Relax back to selected level
    if (moveEvent == InputController::MoveEvent::NONE || moveEvent == InputController::MoveEvent::END) {
        float epsilon = 0.1f;
        float velocityThreshold = _menuTileSize.height*0.1f;
        // Inertia
        if (std::abs(_velocity) > velocityThreshold) {
            // Apply inertia
            float dy = _velocity * timestep;
            _softOffset += dy;
            // Update velocity
            float drag = _menuTileSize.height*20.0f;
            if (_softOffset < _minOffset || _maxOffset < _softOffset) {
                drag *= std::pow(1.0f + std::min(std::abs(_softOffset-_minOffset), std::abs(_softOffset-_maxOffset))/_menuTileSize.height, 2);
            }
            float dv = drag * timestep;
            if (std::abs(dv) > std::abs(_velocity)) {
                _velocity = 0.0f;
            } else {
                _velocity = (_velocity > 0) ? _velocity-dv : _velocity+dv;
            }
            updateSelectedLevel();
            _hardOffset = offsetForLevel(_selectedLevel);
        } else {
            _velocity = 0.0f;
        }
        // Relax to level & check bounds
        if ((_softOffset < _minOffset || _maxOffset < _softOffset) || std::abs(_velocity) <= velocityThreshold) {
            if (std::abs(_softOffset - _hardOffset) > epsilon) {
                float diff = std::abs(_softOffset - _hardOffset);
                float velocity = diff*10.0f;
                float dy = velocity * timestep;
                if (_softOffset < _hardOffset) {
                    _softOffset = _softOffset + dy;
                    if (_softOffset > _hardOffset) { _softOffset = _hardOffset; }
                } else {
                    _softOffset = _softOffset - dy;
                    if (_softOffset < _hardOffset) { _softOffset = _hardOffset; }
                }
            } else if (_introScroll || _scroll) {
                _softOffset = _hardOffset;
                _introScroll = false;
                _scroll = false;
            }
        }
    }
    
    // Move Menu Tiles
    for (auto i = 0; i < _menuTiles.size(); i++) {
        _menuTiles[i]->setPosition(menuTilePosition(i));
        _menuTiles[i]->setVisible(menuTileOnScreen(i));
    }
}
