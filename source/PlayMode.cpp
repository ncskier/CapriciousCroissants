//
//  PlayMode.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include <sstream>
#include <set>
#include "PlayMode.h"

using namespace cugl;

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

/** The key for push sounds */
#define PUSH_SOUND "boop1"

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
PlayMode::PlayMode() : Scene(),
_state(State::PLAYER),
_complete(false),
_debug(false),
done(false)
{
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool PlayMode::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController>& input, std::string& levelJson) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	} else if (!Scene::init(dimen)) {
		return false;
	}
    _dimen = dimen;
    _levelJson = levelJson;

	_assets = assets;
    _actions = ActionManager::alloc();

	_entityManager = std::make_shared<EntityManager>();

	// Initialize all systems that are used for playing
	_entityManager->addSystem(std::make_shared<MovementDumbSystem>(_entityManager), EntityManager::movement);
	_entityManager->addSystem(std::make_shared<MovementSmartSystem>(_entityManager), EntityManager::movement);
	_entityManager->addSystem(std::make_shared<AttackMeleeSystem>(_entityManager), EntityManager::attack);
	_entityManager->addSystem(std::make_shared<AttackRangedSystem>(_entityManager), EntityManager::attack);
	_entityManager->addSystem(std::make_shared<SmartMovementFacingSystem>(_entityManager), EntityManager::onPlayerMove);


    // Add Background Node
    std::shared_ptr<PolygonNode> background = PolygonNode::allocWithTexture(assets->get<Texture>("background"));
    background->setContentSize(dimen);
    addChild(background);
    
//    _worldNode = Node::allocWithBounds(dimen);
    _worldNode = _assets->get<Node>("game");
    _worldNode->setContentSize(dimen);
    _worldNode->setAnchor(Vec2::ZERO);
    _worldNode->doLayout(); // This rearranges the children to fit the screen
	addChild(_worldNode);

    // Setup win/lose text
	_text = std::dynamic_pointer_cast<Label>(assets->get<Node>("game_labelend"));
	_text->setVisible(false);

//    // setup reset button
//    _resetButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("game_reset"));
//    _resetButton->setListener([=](const std::string& name, bool down) {
//        if (down) {
//            CULog("RESET");
//            win = false;
//            done = true;
//        }
//    });
//    _resetButton->activate(2);
    initMenu();
    _worldNode->addChild(_menuNode, 100);
    
    // Setup Touch Node
    _touchNode = AnimationNode::alloc(assets->get<Texture>("touch"), 4, 8, 32);
    _touchNode->setFrame(0);
    _touchNode->setZOrder(1000);
    _touchNode->setVisible(false);
    addChild(_touchNode);
    sortZOrder();
    _touchAction = Animate::alloc(0, 31, 1.0f);

    // Setup state
	_state = State::PLAYER;
	_active = true;
	_complete = false;
	setDebug(false);
    
    // Create board from level file
    setupLevelFromJson(levelJson, dimen);
//    setupLevelFromJson("json/levels/level1.json", dimen);
    
    // Setup Input handler
    _input = input;
    _input->init(getCamera());
    _input->clear();

	// Start up turn controllers
	_playerController.init(_actions, _board, _input, _entityManager);
	_boardController.init(_actions, _board, _entityManager);
	_enemyController.init(_actions, _board, _entityManager);
    
    // Add all sprites to scene graph
    setupLevelSceneGraph();
    
    // Start Music
    if (!AudioEngine::get()->isActiveEffect("music")) {
        auto source = _assets->get<Sound>("music");
        AudioEngine::get()->playEffect("music", source, true, source->getVolume());
    }

	// Set Background
	Application::get()->setClearColor(Color4(229, 229, 229, 255));

	return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void PlayMode::dispose() {
    if (_active) {
        CULog("dispose PlayMode");
        removeAllChildren();
        _assets = nullptr;
        _text = nullptr;
        _actions = nullptr;
        _worldNode = nullptr;
        _touchNode = nullptr;
        _touchAction = nullptr;
        _entityManager = nullptr;
        _input = nullptr;
        _active = false;
        _complete = false;
        _debug = false;
        _playerController.dispose();
        _boardController.dispose();
        _enemyController.dispose();
        _board = nullptr;
        _state = State::PLAYER;
        done = false;
        doneCtr = 30;
        win = false;
        winTimer = 0.0f;
        _beginAttack = false;
        _attacking = false;
		if (_resetButton) {
			_resetButton->deactivate();
		}
        _menuNode = nullptr;
		_resetButton = nullptr;
        _soundButton = nullptr;
        _soundSprite = nullptr;
    }
}


#pragma mark -
#pragma mark Level Layout

/** Reset level/PlayMode */
void PlayMode::reset() {
    // Remove/reset
    _input->clear();
    _worldNode->removeChild(_board->getNode());
    _board = nullptr;
    _playerController.dispose();
    _boardController.dispose();
    _enemyController.dispose();
    _state = State::PLAYER;
    _complete = false;
    done = false;
    doneCtr = 30;
    win = false;
    _beginAttack = false;
    _attacking = false;
    _entityManager = nullptr;
    
    // Re initialize
    _entityManager = std::make_shared<EntityManager>();
    _entityManager->addSystem(std::make_shared<MovementDumbSystem>(_entityManager), EntityManager::movement);
    _entityManager->addSystem(std::make_shared<MovementSmartSystem>(_entityManager), EntityManager::movement);
    _entityManager->addSystem(std::make_shared<AttackMeleeSystem>(_entityManager), EntityManager::attack);
    _entityManager->addSystem(std::make_shared<AttackRangedSystem>(_entityManager), EntityManager::attack);
    _entityManager->addSystem(std::make_shared<SmartMovementFacingSystem>(_entityManager), EntityManager::onPlayerMove);
    setupLevelFromJson(_levelJson, _dimen);
    _playerController.init(_actions, _board, _input, _entityManager);
    _boardController.init(_actions, _board, _entityManager);
    _enemyController.init(_actions, _board, _entityManager);
    setupLevelSceneGraph();
}

/** Exits the game */
void PlayMode::exit() {
    // Stop Music
    if (AudioEngine::get()->isActiveEffect("music")) {
        AudioEngine::get()->stopEffect("music");
    }
    setComplete(true);
}

/** Toggle sound */
void PlayMode::toggleSound() {
    int frame = _soundSprite->getFrame();
    if (frame == 0) {
        // Turn Sound Off
        _soundSprite->setFrame(1);
        AudioEngine::get()->pauseEffect("music");
    } else {
        // Turn Sound On
        _soundSprite->setFrame(0);
        AudioEngine::get()->resumeEffect("music");
    }
}


/** Load level from json */
void PlayMode::setupLevelFromJson(const std::string& filePath, Size dimen) {
    // Load json
    std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(filePath);
    std::shared_ptr<JsonValue> json = reader->readJson();
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
        return;
    }
    
    _board = BoardModel::alloc(json, _assets, dimen, _entityManager, _actions);
}

/** Add level sprites to scene graph */
void PlayMode::setupLevelSceneGraph() {
    // Board
    _worldNode->addChild(_board->getNode());
    
    // Remove tiles
    std::set<std::shared_ptr<TileModel>>::iterator tileIter;
    for (tileIter = _board->getRemovedTiles().begin(); tileIter != _board->getRemovedTiles().end(); ++tileIter) {
        _board->getAddedTiles().erase(*tileIter);
    }
    _board->clearRemovedTiles();
    
    // Add tiles
    int i = 0;
    for (tileIter = _board->getAddedTiles().begin(); tileIter != _board->getAddedTiles().end(); ++tileIter) {
        _board->getNode()->addChild((*tileIter)->getSprite());
        if ((*tileIter)->getDeathSprite()) {
            _board->getNode()->addChild((*tileIter)->getDeathSprite());
        }
        (*tileIter)->getSprite()->setFrame(TILE_IMG_APPEAR_START);
        std::stringstream key;
        key << "int_add_tile_" << i;
        _actions->activate(key.str(), _board->tileAddAction, (*tileIter)->getSprite());
        i++;
    }
    _board->clearAddedTiles();
    
    // Add allies
    i = 0;
    std::set<std::shared_ptr<PlayerPawnModel>>::iterator allyIter;
    for (allyIter = _board->getAddedAllies().begin(); allyIter != _board->getAddedAllies().end(); ++allyIter) {
        _board->getNode()->addChild((*allyIter)->getSprite());
        std::stringstream key;
        key << "int_add_ally_" << i;
        _actions->activate(key.str(), _board->allyAddAction, (*allyIter)->getSprite());
        i++;
    }
    _board->clearAddedAllies();
    
    // Add enemies
	
    i = 0;
    std::set<size_t>::iterator enemyIter;
    for (enemyIter = _board->getAddedEnemies().begin(); enemyIter != _board->getAddedEnemies().end(); ++enemyIter) {
		_board->getNode()->addChild(_entityManager->getComponent<IdleComponent>((*enemyIter)).sprite);
        std::stringstream key;
        key << "int_add_enemy_" << i;
        _actions->activate(key.str(), _board->enemyAddAction, _entityManager->getComponent<IdleComponent>((*enemyIter)).sprite);
        i++;
    }
    _board->clearAddedEnemies();
}


#pragma mark -
#pragma mark Helper Functions
/** Initialize in-game menu */
void PlayMode::initMenu() {
//    _resetButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("game_reset"));
//    _resetButton->setListener([=](const std::string& name, bool down) {
//        if (down) {
//            CULog("RESET");
//            win = false;
//            done = true;
//        }
//    });
//    _resetButton->activate(2);
    float unit = _dimen.height*0.075f;
    float padding = unit*0.25f;
    float height = unit - padding;
    
    // Background
    _menuNode = PolygonNode::allocWithTexture(_assets->get<Texture>(PLAY_MENU_KEY_BACKGROUND));
    _menuNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _menuNode->setContentSize(unit*3.0f, unit);
    _menuNode->setPosition(padding, padding);
    float y = _menuNode->getContentSize().height*0.5f;
    int i;
    
    // Sound On
    i = 0;
    _soundSprite = AnimationNode::alloc(_assets->get<Texture>(PLAY_MENU_KEY_SOUND), 1, 2);
    _soundSprite->setFrame(0);
    _soundButton = Button::alloc(_soundSprite);
    _soundButton->setAnchor(Vec2::ANCHOR_CENTER);
    float soundWidth = _soundButton->getContentSize().width/_soundButton->getContentSize().height * height;
    _soundSprite->setContentSize(soundWidth, height);
    _soundButton->setContentSize(soundWidth, height);
    _soundButton->setPosition(unit*0.5f + i*unit, y);
    _soundButton->setListener([=](const std::string& name, bool down) {
        if (down) {
            CULog("Sound on");
            this->toggleSound();
        }
    });
    _soundButton->activate(PLAY_MENU_LISTENER_SOUND);
    _menuNode->addChild(_soundButton);

    
    // Restart
    i = 1;
    std::shared_ptr<PolygonNode> restartNode = PolygonNode::allocWithTexture(_assets->get<Texture>(PLAY_MENU_KEY_RESTART));
    std::shared_ptr<Button> restartButton = Button::alloc(restartNode);
    restartButton->setAnchor(Vec2::ANCHOR_CENTER);
    float restartWidth = restartButton->getContentSize().width/restartButton->getContentSize().height * height;
    restartNode->setContentSize(restartWidth, height);
    restartButton->setContentSize(restartWidth, height);
    restartButton->setPosition(unit*0.5f + i*unit, y);
    restartButton->setListener([=](const std::string& name, bool down) {
        if (down) {
            CULog("Restart");
            this->reset();
        }
    });
    restartButton->activate(PLAY_MENU_LISTENER_RESTART);
    _menuNode->addChildWithName(restartButton, PLAY_MENU_KEY_RESTART);
    
    // Exit
    i = 2;
    std::shared_ptr<PolygonNode> exitNode = PolygonNode::allocWithTexture(_assets->get<Texture>(PLAY_MENU_KEY_EXIT));
    std::shared_ptr<Button> exitButton = Button::alloc(exitNode);
    exitButton->setAnchor(Vec2::ANCHOR_CENTER);
    float exitWidth = exitButton->getContentSize().width/exitButton->getContentSize().height * height;
    exitNode->setContentSize(exitWidth, height);
    exitButton->setContentSize(exitWidth, height);
    exitButton->setPosition(unit*0.5f + i*unit, y);
    exitButton->setListener([=](const std::string& name, bool down) {
        if (down) {
            CULog("Exit");
            this->exit();
        }
    });
    exitButton->activate(PLAY_MENU_LISTENER_EXIT);
    _menuNode->addChildWithName(exitButton, PLAY_MENU_KEY_EXIT);
}


#pragma mark -
#pragma mark Gameplay Handling

// Update touch node
void PlayMode::updateAnimations() {
    // Update touch node
    updateTouchNode();
    
    // Update Mika
    updateMikaAnimations();
}

/** Update touch node */
void PlayMode::updateTouchNode() {
    if (_input->getMoveEvent() != InputController::MoveEvent::NONE) {
        // Activate action
        if (!_actions->isActive("touchAction") && !_actions->isActive("touchFade")) {
            _actions->activate("touchAction", _touchAction, _touchNode);
        }
        _touchNode->setPosition(_input->getTouchPosition());
        _touchNode->setVisible(true);
    } else {
        if (!_actions->isActive("touchAction")) {
            _touchNode->setVisible(false);
        }
    }
}

/** Update Mika animations */
void PlayMode::updateMikaAnimations() {
    // Update state
    if (_state == PlayMode::ENEMY) {
        _beginAttack = false;
        _attacking = false;
    }
    if (_input->getMoveEvent() == InputController::MoveEvent::START) {
        _beginAttack = !_attacking;
        _attacking = true;
    } else if (_input->getMoveEvent() == InputController::MoveEvent::END || _input->getMoveEvent() == InputController::MoveEvent::NONE) {
        _beginAttack = false;
        _attacking = false;
    }
    if (_beginAttack) {
        if (!_actions->isActive("mikaBeginAttackAction")) {
            if (!_board->getAllies().empty()) {
//                _actions->pause("mikaIdleAction");
                _actions->activate("mikaBeginAttackAction", _board->mikaBeginAttackAction, _board->getAllies()[0]->getSprite());
            }
        }
        _beginAttack = false;
    } else if (_attacking) {
        if (!_actions->isActive("mikaBeginAttackAction") && !_actions->isActive("mikaAttackingAction")) {
            if (!_board->getAllies().empty()) {
                _actions->activate("mikaAttackingAction", _board->mikaAttackingAction, _board->getAllies()[0]->getSprite());
            }
        }
    } else {
        if (!_actions->isActive("mikaIdleAction")) {
            if (!_board->getAllies().empty()) {
                _actions->activate("mikaIdleAction", _board->mikaIdleAction, _board->getAllies()[0]->getSprite());
            }
        }
    }
}

/** Update for player turn */
void PlayMode::updatePlayerTurn(float dt) {
    _playerController.update(dt);
    if (_playerController.isComplete()) {
        _state = State::BOARD;

		//Play push sound
		/*auto source = _assets->get<Sound>("boop1");
		bool didSound = AudioEngine::get()->playEffect("boop1", source, false, source->getVolume(), true);
		CULog(didSound?"true":"false");*/
    }
}

/** Update for board turn */
void PlayMode::updateBoardTurn(float dt) {
    _boardController.update(dt);
    if (_boardController.isComplete()) {
        if (_boardController.win) {
            done = true;
            win = true;
            
            _text->setText("You win");
            _text->setVisible(true);
            _text->setZOrder(1000);
            sortZOrder();
        }
        if (!_enemyController.isComplete()) {
             _state = State::ENEMY;
        } else {
            _playerController.reset();
            _enemyController.reset();
            _state = State::PLAYER;
        }
        _boardController.reset();
    }
}


/** Update for enemy turn */
void PlayMode::updateEnemyTurn(float dt) {
    _enemyController.update(dt);
    if (_enemyController.isComplete()) {
        if (_board->lose) {
            done = true;
            win = false;
            
            _text->setText("You lose");
            _text->setVisible(true);
            _text->setZOrder(1000);
            sortZOrder();
        }
        _state = State::BOARD;
    }
}

/** Update interrupting animations (action manager is already updated every iteration) */
void PlayMode::updateInterruptingAnimations(std::set<std::string>& interruptingActions) {
    bool done = true;
    for (std::set<std::string>::iterator it = interruptingActions.begin(); it != interruptingActions.end(); ++it) {
        if (_actions->isActive(*it)) {
            done = false;
        }
    }
    if (done) {
        interruptingActions.clear();
    }
}

/** Update interrupting win animation if player has won */
void PlayMode::updateWinAnimation(float dt) {
    float disappearTime = 0.035f+0.035f+0.035f+0.035f+0.035f+0.035f+0.4f+0.025f+0.025f+0.025f+0.025f+0.025f+0.025f+0.025f+0.025f+0.025f;
    float appearTime = TILE_IMG_APPEAR_TIME;
    float endTime = 0.75f;
//    float timeInterval = disappearTime/std::max(_board->getWidth(), _board->getHeight());
    float timeInterval = 0.15f;
//    float time = winTimer / timeInterval;
    int mikaX = _board->getAlly(0)->getX();
    int mikaY = _board->getAlly(0)->getY();
    bool winAnimationOver = true;
    for (int x = 0; x < _board->getWidth(); x++) {
        for (int y = 0; y < _board->getHeight(); y++) {
            bool winOver = false;
            float diff = (float)std::max(std::abs(mikaX-x), std::abs(mikaY - y));
//            if (diff + disappearTime + appearTime < winTimer) {
            if (diff*timeInterval + disappearTime + appearTime < winTimer) {
//                if (diff + disappearTime + appearTime + endTime < winTimer) {
                if (diff*timeInterval + disappearTime + appearTime + endTime < winTimer) {
                    // Animation over
                    winOver = true;
                }
//            } else if (diff + disappearTime < winTimer) {
            } else if (diff*timeInterval + disappearTime < winTimer) {
                // Appear animation
                std::stringstream ss_appear;
                ss_appear << "tile_win_animation_appear_(" << x << "," << y << ")";
                if (!_actions->isActive(ss_appear.str())) {
                    _board->getTile(x, y)->getSprite()->setVisible(false);
                    int color = 2;
                    Rect bounds = _board->calculateDrawBounds(x, y);
                    std::shared_ptr<TileModel> tile = TileModel::alloc(color, bounds, _assets);
                    tile->getSprite()->setZOrder(_board->calculateDrawZ(x, y, true));
                    tile->getSprite()->setFrame(TILE_IMG_APPEAR_START);
                    _board->getNode()->addChild(tile->getSprite());
                    _actions->activate(ss_appear.str(), _board->tileAddAction, tile->getSprite());
                    _board->getNode()->sortZOrder();
                }
//            } else if (diff < winTimer) {
            } else if (diff*timeInterval < winTimer) {
                // Disappear animation
                std::stringstream ss_disappear;
                ss_disappear << "tile_win_animation_disappear_(" << x << "," << y << ")";
                if (!_actions->isActive(ss_disappear.str())) {
                    _actions->activate(ss_disappear.str(), _board->tileRemoveAction, _board->getTile(x, y)->getSprite());
                }
            }
            winAnimationOver = winAnimationOver && winOver;
        }
    }
    if (winAnimationOver) {
        setComplete(true);
    }
    
    winTimer += dt;
}

/**
 * Executes the core gameplay loop of this world.
 *
 * This method contains the specific update code for this mini-game. It does
 * not handle collisions, as those are managed by the parent class WorldController.
 * This method is called after input is read, but before collisions are resolved.
 * The very last thing that it should do is apply forces to the appropriate objects.
 *
 * @param  delta    Number of seconds since last animation frame
 */
void PlayMode::update(float dt) {
    // Update input controller
    _input->update(dt);
    
    // Update animations
    updateAnimations();
    
    // Update actions
    _actions->update(dt);
    if (win) {
        updateWinAnimation(dt);
    } else {
        // Check for interrupting animations
        bool hasInterrupts = false;
        for (auto enem = _board->getEnemies().begin(); enem != _board->getEnemies().end(); enem++) {
            if (!_entityManager->getComponent<IdleComponent>((*enem))._interruptingActions.empty()) {
                hasInterrupts = true;
                break;
            }
        }
        if (_playerController.getInterruptingActions().empty() && _boardController.getInterruptingActions().empty() && !hasInterrupts) {
            // Update Gameplay
            if (!done) {
                //    CULog("PlayMode Update");
                if (_state == State::PLAYER) {
                    // PLAYER turn
                    updatePlayerTurn(dt);
                } else if (_state == State::BOARD) {
                    // BOARD turn
                    updateBoardTurn(dt);
                } else {
                    // ENEMY turn
                    updateEnemyTurn(dt);
                }
            } else {
                if (doneCtr == 0) {
                    setComplete(true);
                    CULog("Level Complete");
                } else {
                    doneCtr -= 1;
                }
            }
        } else {
            // Update Interrupting Animations
            if (!_playerController.getInterruptingActions().empty()) { updateInterruptingAnimations(_playerController.getInterruptingActions()); }
            if (!_boardController.getInterruptingActions().empty()) { updateInterruptingAnimations(_boardController.getInterruptingActions()); }
            if (!_enemyController.getInterruptingActions().empty()) { updateInterruptingAnimations(_enemyController.getInterruptingActions()); }

            for (auto enem = _board->getEnemies().begin(); enem != _board->getEnemies().end(); enem++) {
                if (!_entityManager->getComponent<IdleComponent>((*enem))._interruptingActions.empty()) {
                    IdleComponent idle = _entityManager->getComponent<IdleComponent>((*enem));
                    updateInterruptingAnimations(idle._interruptingActions);
                    _entityManager->addComponent<IdleComponent>((*enem), idle);
                }
            }

            _input->clear();
        }
    }
}

