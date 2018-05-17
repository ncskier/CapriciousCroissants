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
bool MenuMode::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController>& input, int selectedLevel) {
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
    _selectedLevel = selectedLevel;
    
    // Initialize ActionManager
    _actions = ActionManager::alloc();
    
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
    _menuCapHiTiles.clear();
    _menuCapLowTiles.clear();
    _softOffset = 0.0f;
    _hardOffset = 0.0f;
    _minOffset = 0.0f;
    _maxOffset = 0.0f;
    _originY = 0.0f;
    _introScroll = true;
    _playSelected = false;
    _mikaAttack = false;
}


#pragma mark -
#pragma mark Helper Initialization

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
    _originY = _dimen.height*0.2f;
    _maxOffset = _menuTileSize.height * (_levelsJson->size() - 1);
    // Create low cap
    for (int i = 0; i < _lowCapTiles; i++) {
        int lvlIdx = -1 - i;
        std::shared_ptr<Node> menuTileCap = createLevelNode(lvlIdx, true);
        _worldNode->addChild(menuTileCap);
        _menuCapLowTiles.push_back(menuTileCap);
    }
    // Create levels
    for (auto i = 0; i < _levelsJson->size(); i++) {
        std::shared_ptr<Node> menuTile = createLevelNode(i);
        _worldNode->addChild(menuTile);
        _menuTiles.push_back(menuTile);
    }
    // Create hi cap
    for (int i = 0; i < _hiCapTiles; i++) {
        int lvlIdx = int(_levelsJson->size())+i;
        std::shared_ptr<Node> menuTileCap = createLevelNode(lvlIdx, true);
        _worldNode->addChild(menuTileCap);
        _menuCapHiTiles.push_back(menuTileCap);
    }
}

/** Create level node */
std::shared_ptr<Node> MenuMode::createLevelNode(int levelIdx, bool cap) {
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
    
    if (!cap) {
        // Initialize Level Dot
        std::shared_ptr<PolygonNode> levelDot = PolygonNode::allocWithTexture(_assets->get<Texture>(MENU_DOT_KEY));
        levelDot->setAnchor(Vec2::ANCHOR_CENTER);
        levelDot->setContentSize(_dotSize);
        levelDot->setPosition(_menuTileSize.width*levelFractionX(levelIdx), _menuTileSize.height*0.5f);
        menuTile->addChild(levelDot);
        
        // Initialize Level Number
        std::shared_ptr<Font> font = _assets->get<Font>("alwaysHereToo");
        std::shared_ptr<Label> levelLabel = Label::alloc(ss.str(), font);
        levelLabel->setAnchor(Vec2::ANCHOR_CENTER);
        levelLabel->setPosition(levelDot->getContentSize().width*0.55f, levelDot->getContentSize().height*0.5f);    // Font appears off-center
        levelDot->addChild(levelLabel);
        _menuDots.push_back(levelDot);
        
        // TODO: Initialize Level Stars
        float starXLeft = levelDot->getContentSize().width*0.20f;
        float starXMid = levelDot->getContentSize().width*0.55f;
        float starXRight = levelDot->getContentSize().width*0.90f;
        float starY = levelDot->getContentSize().height*0.0f;
        float starYMid = levelDot->getContentSize().height*-0.13f;
        int levelStars = GameData::get()->getLevelStars(levelIdx);
        CULog("level %d stars: %d", levelIdx, levelStars);
        // 1
        std::string star1Key = (levelStars >= 1) ? MENU_STAR_KEY : MENU_STAR_EMPTY_KEY;
        std::shared_ptr<PolygonNode> star1 = PolygonNode::allocWithTexture(_assets->get<Texture>(star1Key));
        star1->setAnchor(Vec2::ANCHOR_CENTER);
        star1->setContentSize(_starSize);
        star1->setPosition(starXLeft, starY);
        levelDot->addChild(star1);
        // 2
        std::string star2Key = (levelStars >= 2) ? MENU_STAR_KEY : MENU_STAR_EMPTY_KEY;
        std::shared_ptr<PolygonNode> star2 = PolygonNode::allocWithTexture(_assets->get<Texture>(star2Key));
        star2->setAnchor(Vec2::ANCHOR_CENTER);
        star2->setContentSize(_starSize);
        star2->setPosition(starXMid, starYMid);
        levelDot->addChild(star2);
        // 3
        std::string star3Key = (levelStars >= 3) ? MENU_STAR_KEY : MENU_STAR_EMPTY_KEY;
        std::shared_ptr<PolygonNode> star3 = PolygonNode::allocWithTexture(_assets->get<Texture>(star3Key));
        star3->setAnchor(Vec2::ANCHOR_CENTER);
        star3->setContentSize(_starSize);
        star3->setPosition(starXRight, starY);
        levelDot->addChild(star3);
        // TODO: Initialize Level High Score
        int levelMoves = GameData::get()->getLevelMoves(levelIdx);
        
        CULog("level %d moves: %d", levelIdx, levelMoves);
    }
    
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
    _mikaSprite = AnimationNode::alloc(_assets->get<Texture>(PLAYER_TEXTURE_KEY_0), PLAYER_IMG_ROWS, PLAYER_IMG_COLS, PLAYER_IMG_SIZE);
    _mikaSprite->setAnchor(Vec2::ANCHOR_CENTER);
    _mikaSprite->setFrame(PLAYER_IMG_NORMAL);
    float height = nullTileSprite->getContentSize().height*1.3f;
    float width = _mikaSprite->getContentSize().width/_mikaSprite->getContentSize().height * height;
    _mikaSprite->setContentSize(width, height);
    float x = nullTileSprite->getContentSize().width*0.5f;
    float y = nullTileSprite->getContentSize().height*0.9f;
    _mikaSprite->setPosition(x, y);
    
    // Combine and return
    nullTileSprite->addChildWithName(_mikaSprite, MENU_MIKA_NAME);
    return nullTileSprite;
}

/** Set menu tile size */
void MenuMode::setMenuTileSize() {
    // Menu Tile Size
    std::shared_ptr<AnimationNode> tileNode = AnimationNode::alloc(_assets->get<Texture>(MENU_TILE_KEY_0), MENU_TILE_ROWS, MENU_TILE_COLS, MENU_TILE_SIZE);
    Size tileSize = tileNode->getContentSize();
    float width = _dimen.width;
    float height = (tileSize.height / tileSize.width) * width;
    _menuTileSize = Size(width, height);
    
    // Dot Size
    std::shared_ptr<PolygonNode> levelDot = PolygonNode::allocWithTexture(_assets->get<Texture>(MENU_DOT_KEY));
    float dotHeight = _menuTileSize.height*0.5f;
    float dotWidth = levelDot->getContentSize().width/levelDot->getContentSize().height * dotHeight;
    _dotSize = Size(dotWidth, dotHeight);
    
    // Star Size
    std::shared_ptr<PolygonNode> star = PolygonNode::allocWithTexture(_assets->get<Texture>(MENU_STAR_KEY));
    float starHeight = _menuTileSize.height*0.2f;
    float starWidth = star->getContentSize().width/star->getContentSize().height * starHeight;
    _starSize = Size(starWidth, starHeight);
}


#pragma mark -
#pragma mark Helper Coordinates

/** Calculate menu tile position given the level index */
cugl::Vec2 MenuMode::menuTilePosition(int levelIdx) {
    float positionY = _originY + _menuTileSize.height*levelIdx - _softOffset;
    return Vec2(0.0f, positionY);
}

/** Calculate dot position */
cugl::Vec2 MenuMode::dotPosition(int levelIdx) {
    return Vec2(_menuTileSize.width*levelFractionX(levelIdx), _menuTileSize.height*0.5f);
}

/** Apply offset cap function to difference between offset min/max and movement offset */
float MenuMode::applyOffsetCapFunction(float diff) {
    float converter = _menuTileSize.height*0.1f;        // Lower value means cap will converge faster
    float x = diff / converter;
    float y = std::log(std::pow(x+1, 2));               // Lower power will converge faster
    return y * converter;
}

/** Return true if touch selected the level */
bool MenuMode::touchSelectedLevel(cugl::Vec2 touchPosition) {
    // Check Null Tile
    bool inNullTile = _mikaNode->getBoundingBox().contains(touchPosition);
    // Check Mika Sprite
    Vec2 touchPositionMikaNodeCoords = _mikaNode->worldToNodeCoords(touchPosition);
    bool inMikaSprite = _mikaSprite->getBoundingBox().contains(touchPositionMikaNodeCoords);
    // Check Dot
    std::shared_ptr<Node> menuTile = _menuTiles[_selectedLevel];
    Vec2 touchPositionMenuNodeCoords = menuTile->worldToNodeCoords(touchPosition);
    bool inDotSprite = _menuDots[_selectedLevel]->getBoundingBox().contains(touchPositionMenuNodeCoords);
    return inNullTile || inMikaSprite || inDotSprite;
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


#pragma mark -
#pragma mark Helper Visuals

/** Return offset for level at [levelIdx] */
float MenuMode::offsetForLevel(int levelIdx) {
    return _menuTileSize.height*levelIdx;
}

/** Calculate level spritesheet frame given the level index [levelIdx] */
int MenuMode::menuTileFrame(int levelIdx) {
    int frame = MENU_TILE_SIZE - (levelIdx % MENU_TILE_SIZE) - 1;
    // TODO: Update for negatives
    if (levelIdx < 0) {
        frame = (MENU_TILE_SIZE + ((-levelIdx) % MENU_TILE_SIZE) - 1) % MENU_TILE_SIZE;
    }
    return frame;
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

#pragma mark -
#pragma mark Menu Tiles

/** Returns if menu tile at index [i] should be hidden (if it's off the screen) */
bool MenuMode::menuTileOnScreen(int i) {
    float minY = menuTilePosition(i).y;
    float maxY = minY + _menuTileSize.height;
    return ((minY > 0.0f && minY < _dimen.height) || (maxY > 0.0f && maxY < _dimen.height));
}


#pragma mark -
#pragma mark Helper Input Handling

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

/** Update mika animation */
void MenuMode::updateMikaAnimation(float timestep, bool touchDown) {
    if (!touchDown) {
        _mikaAttack = false;
        if (!_actions->isActive(MIKA_IDLE_KEY)) {
            _actions->activate(MIKA_IDLE_KEY, _mikaIdleAction, _mikaSprite);
        }
    } else {
        if (!_mikaAttack) {
            if (!_actions->isActive(MIKA_TRANSITION_KEY)) {
                _actions->activate(MIKA_TRANSITION_KEY, _mikaTransitionAction, _mikaSprite);
            } else {
                _mikaAttack = true;
            }
        } else {
            if (!_actions->isActive(MIKA_ATTACK_KEY)) {
                _actions->activate(MIKA_ATTACK_KEY, _mikaAttackAction, _mikaSprite);
            }
        }
    }
}

/** Update selected level */
void MenuMode::updateSelectedLevel() {
    _selectedLevel = (int)std::round(_softOffset / _menuTileSize.height);
    // Check for caps
    if (_selectedLevel < 0) { _selectedLevel = 0; }
    if (_selectedLevel > _levelsJson->size()-1) { _selectedLevel = (int)(_levelsJson->size()-1); }
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
                // START
                // Check if player tapped on Mika
                _velocity = 0.0f;
                Vec2 touchPosition = _input->getTouchPosition();
                if (touchSelectedLevel(touchPosition)) {
                    _playSelected = true;
                }
                _hardOffset = _softOffset;
                _input->recordMove();
            } else if (moveEvent != InputController::MoveEvent::END) {
                // MOVED
                _softOffset = _hardOffset - moveOffset.y;
                // Check drag bounds (top & bottom)
                if (_softOffset > _maxOffset) {
                    float diff = std::abs(_softOffset-_maxOffset);
                    _softOffset = _maxOffset + applyOffsetCapFunction(diff);
                } else if (_softOffset < _minOffset) {
                    float diff = std::abs(_softOffset-_minOffset);
                    _softOffset = _minOffset - applyOffsetCapFunction(diff);
                }
                // Update _playSelected
                _playSelected = _playSelected && touchSelectedLevel(_input->getTouchPosition());
            } else {
                // END
                if (_playSelected && touchSelectedLevel(_input->getTouchPosition())) {
                    // Select level and exit Level Select Menu
                    this->_selectedLevelJson = _levelsJson->get(_selectedLevel)->asString();
                    this->setActive(false);
                }
                if (_input->isTapTime() && _input->isTapSpace()) {
                    int level = tappedLevel(_input->getTouchPosition());
                    if (level != -1) {
//                        _selectedLevel = level;
//                        _hardOffset = offsetForLevel(_selectedLevel);
                        _hardOffset = offsetForLevel(level);
                        _scroll = true;
                    }
                } else {
                    _hardOffset = offsetForLevel(_selectedLevel);
                    if (_input->isSwipe()) {
                        float time = _input->getSwipeTime();
                        float distance = _input->getSwipeVector().y;
                        _velocity = -distance / time;
                        float maxVelocity = 15000.0f;
                        if (std::abs(_velocity) > maxVelocity) {
                            _velocity = (_velocity < 0.0f) ? -maxVelocity : maxVelocity;
                        }
                        CULog("time: %f", time);
                        CULog("distance: %f", distance);
                        CULog("velocity: %f", _velocity);
                        if (std::isnan(_velocity)) {
                            _velocity = 0.0f;
                        }
                    }
                }
                _input->clear();
            }
        }
    }
    
    // Update
    if (!_introScroll) {
        updateSelectedLevel();
    }
    if (!_introScroll) {
        updateMikaNode();
    }
    
    // Relax back to selected level
    bool animateMika = (moveEvent != InputController::MoveEvent::NONE && moveEvent != InputController::MoveEvent::END);
    if (moveEvent == InputController::MoveEvent::NONE || moveEvent == InputController::MoveEvent::END) {
        float epsilon = _menuTileSize.height*0.01f;
        float velocityThreshold = _menuTileSize.height*0.1f;
        // Inertia
        if (std::abs(_velocity) > velocityThreshold) {
            animateMika = true;
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
        // Menu Tiles
        _menuTiles[i]->setPosition(menuTilePosition(i));
        _menuTiles[i]->setVisible(menuTileOnScreen(i));
        
        // Dots
        float time = 0.1f;
        float scale = (i == _selectedLevel) ? 2.1f : 1.0f;
        std::stringstream ss;
        if (i == _selectedLevel) {
            ss << "select_";
        } else {
            ss << "unselect_";
        }
        ss << i;
//        Size maxDotSize = _dotSize*2.1f;
//        Size size = (i == _selectedLevel) ? maxDotSize : _dotSize;
        Size size = _dotSize*scale;
        Vec2 position = (i == _selectedLevel) ? dotPosition(i)-Vec2(0.0f, _menuTileSize.height*0.1f) : dotPosition(i);
//        _menuDots[i]->setContentSize(size);
        _menuDots[i]->setPosition(position);
        std::shared_ptr<ScaleTo> scaleAction = ScaleTo::alloc(Vec2(scale, scale), time);
        if (!_actions->isActive(ss.str())) {
            _actions->activate(ss.str(), scaleAction, _menuDots[i]);
        }
        
        // Label
//        Vec2 labelPos = (i == _selectedLevel) ? Vec2(size.width*0.5f, size.height*0.2f) : Vec2(size.width*0.5f, size.height*0.5f);
        Vec2 labelPos = (i == _selectedLevel) ? Vec2(size.width*0.25f, size.height*0.1f) : Vec2(size.width*0.55f, size.height*0.5f);
//        _menuDots[i]->getChild(0)->setPosition(labelPos);
        std::stringstream ssLabel;
        std::stringstream ssLabelScale;
        if (i == _selectedLevel) {
            ssLabel << "select_label_";
            ssLabelScale << "select_label_scale_";
        } else {
            ssLabel << "unselect_label_";
            ssLabelScale << "unselect_label_scale_";
        }
        ssLabel << i;
        ssLabelScale << i;
        std::shared_ptr<MoveTo> moveAction = MoveTo::alloc(labelPos, time);
        std::shared_ptr<ScaleTo> labelScaleAction = ScaleTo::alloc(Vec2(1.0f/scale, 1.0f/scale), time);
        if (!_actions->isActive(ssLabel.str())) {
            _actions->activate(ssLabel.str(), moveAction, _menuDots[i]->getChild(0));
        }
        if (!_actions->isActive(ssLabelScale.str())) {
            _actions->activate(ssLabelScale.str(), labelScaleAction, _menuDots[i]->getChild(0));
        }
    }
    // Move upper cap
    for (auto i = 0; i < _menuCapHiTiles.size(); i++) {
        // Menu Tiles
        int lvlIdx = int(_levelsJson->size())+i;
        _menuCapHiTiles[i]->setPosition(menuTilePosition(lvlIdx));
        _menuCapHiTiles[i]->setVisible(menuTileOnScreen(lvlIdx));
    }
    // Move lower cap
    for (auto i = 0; i < _menuCapLowTiles.size(); i++) {
        // Menu Tiles
        int lvlIdx = -1 - i;
        _menuCapLowTiles[i]->setPosition(menuTilePosition(lvlIdx));
        _menuCapLowTiles[i]->setVisible(menuTileOnScreen(lvlIdx));
    }
    
    // Update Mika Animation
    updateMikaAnimation(timestep, animateMika);
    _actions->update(timestep);
}
