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
done(false),
_winloseActive(false)
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
bool PlayMode::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController>& input, int level) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	} else if (!Scene::init(dimen)) {
		return false;
	}
    _dimen = dimen;
    _level = level;

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
    setupLevelFromJson(dimen);
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
		if (_worldNode != nullptr) {
			_worldNode->removeAllChildren();
		}
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
        loseTimer = 0.0f;
        loseDisappear = false;
        _beginAttack = false;
        _attacking = false;
        // Menu
		if (_resetButton && _resetButton->isActive()) {
			_resetButton->deactivate();
		}
        if (_exitButton && _exitButton->isActive()) {
            _exitButton->deactivate();
        }
        if (_soundButton && _soundButton->isActive()) {
            _soundButton->deactivate();
        }
        _menuNode = nullptr;
		_resetButton = nullptr;
        _soundButton = nullptr;
        _soundSprite = nullptr;
        _exitButton = nullptr;
        // WinLose
        _winloseNode = nullptr;
        _winloseActive = false;
        if (_winloseContinueButton && _winloseContinueButton->isActive()) {
            _winloseContinueButton->deactivate();
        }
        if (_winloseRetryButton && _winloseRetryButton->isActive()) {
            _winloseRetryButton->deactivate();
        }
        if (_winloseLevelsButton && _winloseLevelsButton->isActive()) {
            _winloseLevelsButton->deactivate();
        }
        _winloseContinueButton = nullptr;
        _winloseRetryButton = nullptr;
        _winloseLevelsButton = nullptr;
    }
}


#pragma mark -
#pragma mark Level Layout

/** Reset level/PlayMode */
void PlayMode::reset() {
    // Remove/reset
    _input->clear();
    _worldNode->removeChild(_board->getNode());
    if (_winloseActive) {
        _worldNode->removeChild(_winloseNode);
    }
    _board = nullptr;
    _playerController.dispose();
    _boardController.dispose();
    _enemyController.dispose();
    _state = State::PLAYER;
    _complete = false;
    done = false;
    doneCtr = 30;
    win = false;
    winTimer = 0.0f;
    loseTimer = 0.0f;
    loseDisappear = false;
    _beginAttack = false;
    _attacking = false;
    _entityManager = nullptr;
    if (_winloseActive) {
        _winloseNode->removeAllChildren();
    }
    _winloseNode = nullptr;
    _winloseActive = false;
    
    // Re initialize
    _entityManager = std::make_shared<EntityManager>();
    _entityManager->addSystem(std::make_shared<MovementDumbSystem>(_entityManager), EntityManager::movement);
    _entityManager->addSystem(std::make_shared<MovementSmartSystem>(_entityManager), EntityManager::movement);
    _entityManager->addSystem(std::make_shared<AttackMeleeSystem>(_entityManager), EntityManager::attack);
    _entityManager->addSystem(std::make_shared<AttackRangedSystem>(_entityManager), EntityManager::attack);
    _entityManager->addSystem(std::make_shared<SmartMovementFacingSystem>(_entityManager), EntityManager::onPlayerMove);
    setupLevelFromJson(_dimen);
    _playerController.init(_actions, _board, _input, _entityManager);
    _boardController.init(_actions, _board, _entityManager);
    _enemyController.init(_actions, _board, _entityManager);
    setupLevelSceneGraph();
}

/** Exits the game */
void PlayMode::exit() {
    // Stop Music
    AudioEngine::get()->stopMusic();
    setComplete(true);
}

/** Toggle sound */
void PlayMode::toggleSound() {
    int frame = _soundSprite->getFrame();
    if (frame == 0) {
        // Turn Sound Off
        _soundSprite->setFrame(1);
        AudioEngine::get()->pauseMusic();
        AudioEngine::get()->setMusicVolume(0.0f);
        AudioEngine::get()->stopAllEffects();
        
        // Save state
        GameData::get()->setMuteSetting(true);
    } else {
        // Turn Sound On
        _soundSprite->setFrame(0);
        AudioEngine::get()->resumeMusic();
        AudioEngine::get()->setMusicVolume(0.5f);
        
        // Save state
        GameData::get()->setMuteSetting(false);
    }
}


/** Load level from json */
void PlayMode::setupLevelFromJson(Size dimen) {
    // Get filepath
    std::shared_ptr<JsonReader> levelsReader = JsonReader::allocWithAsset("json/levelList.json");
    std::shared_ptr<JsonValue> levelsJson = levelsReader->readJson();
    std::string filePath = levelsJson->get("levels")->get(_level)->asString();
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
        _board->getNode()->addChild((*allyIter)->getEndSprite());
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
    // Get mute setting & set accordingly
    auto music = Music::alloc("sounds/music.wav");
    if (AudioEngine::get()->getMusicState() != AudioEngine::State::PLAYING) {
        AudioEngine::get()->playMusic(music, true, 0.5f);
    }
    if (GameData::get()->getMuteSetting()) {
        // Turn Sound Off
        _soundSprite->setFrame(1);
        AudioEngine::get()->pauseMusic();
        AudioEngine::get()->setMusicVolume(0.0f);
        AudioEngine::get()->stopAllEffects();
    } else {
        // Turn Sound On
        _soundSprite->setFrame(0);
        AudioEngine::get()->resumeMusic();
        AudioEngine::get()->setMusicVolume(0.5f);
    }

    
    // Restart
    i = 1;
    std::shared_ptr<PolygonNode> restartNode = PolygonNode::allocWithTexture(_assets->get<Texture>(PLAY_MENU_KEY_RESTART));
    _resetButton = Button::alloc(restartNode);
    _resetButton->setAnchor(Vec2::ANCHOR_CENTER);
    float restartWidth = _resetButton->getContentSize().width/_resetButton->getContentSize().height * height;
    restartNode->setContentSize(restartWidth, height);
    _resetButton->setContentSize(restartWidth, height);
    _resetButton->setPosition(unit*0.5f + i*unit, y);
    _resetButton->setListener([=](const std::string& name, bool down) {
        if (down) {
            CULog("Restart");
            this->reset();
        }
    });
    _resetButton->activate(PLAY_MENU_LISTENER_RESTART);
    _menuNode->addChildWithName(_resetButton, PLAY_MENU_KEY_RESTART);
    
    // Exit
    i = 2;
    std::shared_ptr<PolygonNode> exitNode = PolygonNode::allocWithTexture(_assets->get<Texture>(PLAY_MENU_KEY_EXIT));
    _exitButton = Button::alloc(exitNode);
    _exitButton->setAnchor(Vec2::ANCHOR_CENTER);
    float exitWidth = _exitButton->getContentSize().width/_exitButton->getContentSize().height * height;
    exitNode->setContentSize(exitWidth, height);
    _exitButton->setContentSize(exitWidth, height);
    _exitButton->setPosition(unit*0.5f + i*unit, y);
    _exitButton->setListener([=](const std::string& name, bool down) {
        if (down) {
            CULog("Exit");
            CULog("Exit Button");
            this->exit();
        }
    });
    _exitButton->activate(PLAY_MENU_LISTENER_EXIT);
    _menuNode->addChildWithName(_exitButton, PLAY_MENU_KEY_EXIT);
}

/** Calculate stars */
int PlayMode::calculateLevelStars() {
    int allies = (int)_board->getAllies().size();
    return (3 - (_board->maxAllies - allies));
}


#pragma mark -
#pragma mark Gameplay Handling

// Update touch node
void PlayMode::updateAnimations() {
    // Update touch node
    updateTouchNode();
    
    // Update Mika
    updateMikaAnimations();

	// Update Allys
	updateAllyAnimations();

	// Update Enemies
	updateEnemyAnimations();
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

/** Update Ally animations(Only idle animations */
void PlayMode::updateAllyAnimations() {
	std::vector<std::shared_ptr<PlayerPawnModel>> allies = _board->getAllies();
	for (auto a = allies.begin(); a != allies.end(); a++) {
		if (!(*a)->isMika()) {
			if (!_actions->isActive((*a)->ownName + "IdleAction")) {
				_actions->activate((*a)->ownName, _board->allyIdleAction, (*a)->getSprite());
			}
		}
	}
}

/** Update Enemy animations based on entity ids of board */
void PlayMode::updateEnemyAnimations() {
	std::vector<size_t> enemies = _board->getEnemies();
	for (auto e = enemies.begin(); e != enemies.end(); e++) {
		// All enemies have an idle and location component, so not bothering to check
		LocationComponent loc = _entityManager->getComponent<LocationComponent>(*e);
		IdleComponent idle = _entityManager->getComponent<IdleComponent>(*e);
		if (!loc.isMoving && !loc.isAttacking) {
			idle._actions->remove("moveAnimation" + (*e));
			idle._actions->remove("attackAnimation" + (*e));
			if (!idle._actions->isActive("idleAnimation" + (*e))) {
				switch (loc.dir) {
				case LocationComponent::UP:
					idle._actions->activate("idleAnimation" + (*e), _board->enemyIdleUpAction, idle.sprite);
					break;
				case LocationComponent::DOWN:
					idle._actions->activate("idleAnimation" + (*e), _board->enemyIdleDownAction, idle.sprite);
					break;
				case LocationComponent::LEFT:
					idle._actions->activate("idleAnimation" + (*e), _board->enemyIdleLeftAction, idle.sprite);
					break;
				case LocationComponent::RIGHT:
					idle._actions->activate("idleAnimation" + (*e), _board->enemyIdleRightAction, idle.sprite);
					break;
				}
			}
		}
		else if (loc.isMoving && !loc.isAttacking) {
			idle._actions->remove("idleAnimation" + (*e));
			idle._actions->remove("attackAnimation" + (*e));
			if (!idle._actions->isActive("moveAnimation" + (*e))) {
				switch (loc.dir) {
				case LocationComponent::UP:
					idle._actions->activate("moveAnimation" + (*e), _board->enemyMoveUpAction, idle.sprite);
					break;
				case LocationComponent::DOWN:
					idle._actions->activate("moveAnimation" + (*e), _board->enemyMoveDownAction, idle.sprite);
					break;
				case LocationComponent::LEFT:
					idle._actions->activate("moveAnimation" + (*e), _board->enemyMoveLeftAction, idle.sprite);
					break;
				case LocationComponent::RIGHT:
					idle._actions->activate("moveAnimation" + (*e), _board->enemyMoveRightAction, idle.sprite);
					break;
				}
			}
		}
		else if (!loc.isMoving && loc.isAttacking) {
			idle._actions->remove("idleAnimation" + (*e));
			idle._actions->remove("moveAnimation" + (*e));
			if (!idle._actions->isActive("attackAnimation" + (*e))) {
				switch (loc.dir) {
				case LocationComponent::UP:
					idle._actions->activate("attackAnimation" + (*e), _board->enemyAttackUpAction, idle.sprite);
					break;
				case LocationComponent::DOWN:
					idle._actions->activate("attackAnimation" + (*e), _board->enemyAttackDownAction, idle.sprite);
					break;
				case LocationComponent::LEFT:
					idle._actions->activate("attackAnimation" + (*e), _board->enemyAttackLeftAction, idle.sprite);
					break;
				case LocationComponent::RIGHT:
					idle._actions->activate("attackAnimation" + (*e), _board->enemyAttackRightAction, idle.sprite);
					break;
				}
			}
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
            
            // Set Stars
            int stars = calculateLevelStars();
            if (stars > GameData::get()->getLevelStars(_level)) {
                GameData::get()->setLevelStars(_level, stars);
            }
            
            // Begin Mika Win Animation
            std::shared_ptr<PlayerPawnModel> mika = _board->getAlly(0);
            mika->getSprite()->setVisible(false);
            mika->getEndSprite()->setVisible(true);
            mika->getEndSprite()->setFrame(PLAYER_END_WIN_START);
            std::string mikaWinActionKey = "mika-win-animation";
            if (!_actions->isActive(mikaWinActionKey)) {
                _actions->activate(mikaWinActionKey, _board->mikaWinAction, mika->getEndSprite());
            }
            
            // TODO: Set Moves
//            GameData::get()->setLevelMoves(_level, moves);
            
//            _text->setText("You win");
//            _text->setVisible(true);
//            _text->setZOrder(1000);
//            sortZOrder();
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
            
            // Plase lose sound
            if (AudioEngine::get()->getMusicVolume() != 0.0f && !AudioEngine::get()->isActiveEffect("lose")) {
                auto source = _assets->get<Sound>("lose");
                AudioEngine::get()->playEffect("lose", source, false, source->getVolume());
            }
            
            // Begin Mika Lose Animation
            std::shared_ptr<PlayerPawnModel> mika = _board->getAlly(0);
            mika->setSpriteLose();
            mika->getSprite()->setVisible(false);
            mika->getEndSprite()->setVisible(true);
            mika->getEndSprite()->setFrame(PLAYER_END_LOSE_START);
            std::string mikaLoseActionKey = "mika-lose-animation";
            if (!_actions->isActive(mikaLoseActionKey)) {
                _actions->activate(mikaLoseActionKey, _board->mikaLoseAction, mika->getEndSprite());
            }
            
//            _text->setText("You lose");
//            _text->setVisible(true);
//            _text->setZOrder(1000);
//            sortZOrder();
        }
		std::vector<size_t> enemies = _board->getEnemies();
		for (auto e = enemies.begin(); e != enemies.end(); e++) {
			// All enemies have an idle and location component, so not bothering to check
			LocationComponent loc = _entityManager->getComponent<LocationComponent>(*e);
			loc.isAttacking = false;
			loc.isMoving = false;
			_entityManager->addComponent<LocationComponent>((*e), loc);
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
    float endTime = 0.25f;
//    float timeInterval = disappearTime/std::max(_board->getWidth(), _board->getHeight());
//    float timeInterval = 0.15f;
    float timeInterval = 0.1f;
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
//        setComplete(true);
        // Present WinLose Screen
        if (!_winloseActive) {
            // Play win sound
            if (AudioEngine::get()->getMusicVolume() != 0.0f && !AudioEngine::get()->isActiveEffect("win")) {
                auto source = _assets->get<Sound>("win");
                AudioEngine::get()->playEffect("win", source, false, source->getVolume());
            }
            // Create WinLose Screen
            initWinLose();
        }
    }
    
    winTimer += dt;
}

/** Update interrupting lose animation if player has lost */
void PlayMode::updateLoseAnimation(float dt) {
    float waitTime = 0.5f;
    float disappearTime = 1.0f;
    float totalTime = PLAYER_END_LOSE_TIME + disappearTime + disappearTime + waitTime;
    
    // Present WinLose Screen
    if (loseTimer > totalTime) {
        if (!_winloseActive) {
            initWinLose();
        }
    } else if (loseTimer > PLAYER_END_LOSE_TIME + disappearTime) {
        // Fade out Mika
        std::shared_ptr<FadeOut> fadeOut = FadeOut::alloc(disappearTime);
        std::shared_ptr<PlayerPawnModel> mika = _board->getAlly(0);
        if (mika->getEndSprite()->isVisible()) {
            _actions->activate("fade_out_mika_lose", fadeOut, mika->getEndSprite());
        } else {
            _actions->activate("fade_out_mika_lose", fadeOut, mika->getSprite());
        }
    } else if (loseTimer > PLAYER_END_LOSE_TIME && !loseDisappear) {
        std::shared_ptr<FadeOut> fadeOut = FadeOut::alloc(disappearTime);
        // Fade Out all tiles and enemies
        int i = 0;
        int mikaX = _board->getAlly(0)->getX();
        int mikaY = _board->getAlly(0)->getY();
        // Tiles
        for (int j = 0; j < (_board->getWidth()*_board->getHeight()); j++) {
            int tileX = _board->xOfIndex(j);
            int tileY = _board->yOfIndex(j);
            if (!(tileX == mikaX && tileY == mikaY)) {
                std::shared_ptr<TileModel> tile = _board->getTile(tileX, tileY);
                std::stringstream key;
                key << "disappear_lose_" << i;
                _actions->activate(key.str(), fadeOut, tile->getSprite());
                i++;
            }
        }
        // Allies
        for (int j = 0; j < _board->getNumAllies(); j++) {
            if (j != 0) {
                std::shared_ptr<PlayerPawnModel> ally = _board->getAllies()[j];
                std::stringstream key;
                key << "disappear_lose_" << i;
                if (ally->getSprite()->isVisible()) {
                    _actions->activate(key.str(), fadeOut, ally->getSprite());
                } else {
                    _actions->activate(key.str(), fadeOut, ally->getEndSprite());
                }
                i++;
            }
        }
        // Enemies
        for (int j = 0; j < _board->getNumEnemies(); j++) {
            LocationComponent enemyLoc = _entityManager->getComponent<LocationComponent>(_board->getEnemy(j));
            if (!(enemyLoc.x == mikaX && enemyLoc.y == mikaY)) {
                IdleComponent enemyIdle = _entityManager->getComponent<IdleComponent>(_board->getEnemy(j));
                std::stringstream key;
                key << "disappear_lose_" << i;
                _actions->activate(key.str(), fadeOut, enemyIdle.sprite);
                i++;
            }
        }
        loseDisappear = true;
    }
    loseTimer += dt;
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
    if (!_winloseActive) {
        updateAnimations();
    }
    
    // Update actions
    _actions->update(dt);
    if (done) {
        if (win) {
            updateWinAnimation(dt);
        } else {
            updateLoseAnimation(dt);
        }
    } else {
        // Check for interrupting animations
        bool hasInterrupts = false;
        for (auto enem = _board->getEnemies().begin(); enem != _board->getEnemies().end(); enem++) {
            if (!_entityManager->getComponent<IdleComponent>((*enem))._interruptingActions.empty()) {
                hasInterrupts = true;
                break;
            }
        }
        if (_playerController.getInterruptingActions().empty() && _boardController.getInterruptingActions().empty() && _enemyController.getInterruptingActions().empty() && !hasInterrupts) {
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
//                if (doneCtr == 0) {
//                    setComplete(true);
//                    CULog("Level Complete");
//                } else {
//                    doneCtr -= 1;
//                }
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

#pragma mark -
#pragma mark WinLose Menu
/** Initialize WinLose Menu */
void PlayMode::initWinLose() {
    CULog("initWinLose");
    // Clean up if retry previously
    if (_winloseContinueButton && _winloseContinueButton->isActive()) {
        _winloseContinueButton->deactivate();
    }
    if (_winloseRetryButton && _winloseRetryButton->isActive()) {
        _winloseRetryButton->deactivate();
    }
    if (_winloseLevelsButton && _winloseRetryButton->isActive()) {
        _winloseLevelsButton->deactivate();
    }
    _winloseContinueButton = nullptr;
    _winloseRetryButton = nullptr;
    _winloseLevelsButton = nullptr;
    _winloseActive = true;
    
    // Unit for sizing
    float unit = _dimen.height*0.075f;
    float buttonHeight = unit;
    float buttonY = _dimen.height*0.095f;
    float starsY = _dimen.height*0.73f;
    float starsYUp = starsY + unit*0.65f;
    float starHeight = unit*2.0f;
    float starOffset = starHeight*-0.1f;
    float highStarsY = _dimen.height*0.275f;
    float highStarHeight = unit;
    float highStarOffset = highStarHeight*0.1f;
    float highscoreTextY = highStarsY - unit*0.8f;
    float mikaHeight = unit*5.5f;
    float mikaWinY = _dimen.height*0.49f;
//    float mikaLoseY = _dimen.height*0.53f;
    float mikaLoseY = _dimen.height*0.492;
    float textY = starsY;
    float levelTextY = _dimen.height*0.9f;
    
    // Init Node
    _winloseNode = Node::allocWithBounds(0, 0, _dimen.width, _dimen.height);
    
    // Background
    std::string backgroundTextureKey = win ? WIN_LOSE_BACKGROUND_WIN : WIN_LOSE_BACKGROUND_LOSE;
    std::shared_ptr<PolygonNode> background = PolygonNode::allocWithTexture(_assets->get<Texture>(backgroundTextureKey));
    background->setContentSize(_dimen);     // Stretch background to fit dimensions
    _winloseNode->addChild(background);
    
    // Retry Button
    std::shared_ptr<PolygonNode> retryNode = PolygonNode::allocWithTexture(_assets->get<Texture>(WIN_LOSE_RETRY));
    _winloseRetryButton = Button::alloc(retryNode);
    _winloseRetryButton->setAnchor(Vec2::ANCHOR_CENTER);
    float retryWidth = _winloseRetryButton->getContentSize().width/_winloseRetryButton->getContentSize().height * buttonHeight;
    retryNode->setContentSize(retryWidth, buttonHeight);
    _winloseRetryButton->setContentSize(retryWidth, buttonHeight);
    _winloseRetryButton->setPosition(_dimen.width*0.20f, buttonY);
    _winloseRetryButton->setListener([=](const std::string& name, bool down) {
//        if (!down && this->_winloseRetryButton->getBoundingBox().contains(this->_input->getTouchPosition())) {
        if (!down) {
            CULog("Retry Level");
            this->retryLevel();
        }
    });
    _winloseRetryButton->activate(WIN_LOSE_LISTENER_RETRY);
    _winloseNode->addChild(_winloseRetryButton);
    
    // Levels Button
    std::string levelsTextureKey = win ? WIN_LOSE_LEVELS_WIN : WIN_LOSE_LEVELS_LOSE;
    std::shared_ptr<PolygonNode> levelsNode = PolygonNode::allocWithTexture(_assets->get<Texture>(levelsTextureKey));
    _winloseLevelsButton = Button::alloc(levelsNode);
    _winloseLevelsButton->setAnchor(Vec2::ANCHOR_CENTER);
    float levelsWidth = _winloseLevelsButton->getContentSize().width/_winloseLevelsButton->getContentSize().height * buttonHeight;
    levelsNode->setContentSize(levelsWidth, buttonHeight);
    _winloseLevelsButton->setContentSize(levelsWidth, buttonHeight);
    float levelsX = win ? _dimen.width*0.5f : _dimen.width*0.80f;
    _winloseLevelsButton->setPosition(levelsX, buttonY);
    _winloseLevelsButton->setListener([=](const std::string& name, bool down) {
//        if (!down && this->_winloseLevelsButton->getBoundingBox().contains(this->_input->getTouchPosition())) {
        if (!down) {
            CULog("Level Menu");
            this->levelMenu();
        }
    });
    _winloseLevelsButton->activate(WIN_LOSE_LISTENER_LEVELS);
    _winloseNode->addChild(_winloseLevelsButton);
    
    // Continue Button
    if (win) {
        std::shared_ptr<PolygonNode> continueNode = PolygonNode::allocWithTexture(_assets->get<Texture>(WIN_LOSE_CONTINUE));
        _winloseContinueButton = Button::alloc(continueNode);
        _winloseContinueButton->setAnchor(Vec2::ANCHOR_CENTER);
        float continueWidth = _winloseContinueButton->getContentSize().width/_winloseContinueButton->getContentSize().height * buttonHeight;
        continueNode->setContentSize(continueWidth, buttonHeight);
        _winloseContinueButton->setContentSize(continueWidth, buttonHeight);
        _winloseContinueButton->setPosition(_dimen.width*0.80f, buttonY);
        _winloseContinueButton->setListener([=](const std::string& name, bool down) {
    //        if (!down && this->_winloseNextButton->getBoundingBox().contains(this->_input->getTouchPosition())) {
            if (!down) {
                CULog("Next Level");
                this->nextLevel();
            }
        });
        _winloseContinueButton->activate(WIN_LOSE_LISTENER_CONTINUE);
        _winloseNode->addChild(_winloseContinueButton);
    }
    
    // Mika
    std::string mikaTextureKey = win ? WIN_LOSE_MIKA_WIN : WIN_LOSE_MIKA_LOSE;
    std::shared_ptr<PolygonNode> mikaNode = PolygonNode::allocWithTexture(_assets->get<Texture>(mikaTextureKey));
    mikaNode->setAnchor(Vec2::ANCHOR_CENTER);
    float mikaWidth = mikaNode->getContentSize().width/mikaNode->getContentSize().height * mikaHeight;
    mikaNode->setContentSize(mikaWidth, mikaHeight);
    if (win) {
        mikaNode->setPosition(_dimen.width*0.5f, mikaWinY);
    } else {
        mikaNode->setPosition(_dimen.width*0.5f, mikaLoseY);
    }
    _winloseNode->addChild(mikaNode);
    
    // Level Label
    std::stringstream ssLevel;
    ssLevel << "Level " << (_level+1);
    std::shared_ptr<Font> font = _assets->get<Font>("alwaysHereToo");
    std::shared_ptr<Label> levelLabel = Label::alloc(ssLevel.str(), font);
    levelLabel->setAnchor(Vec2::ANCHOR_CENTER);
    levelLabel->setPosition(_dimen.width*0.5f, levelTextY);
    levelLabel->setForeground(Color4::WHITE);
    _winloseNode->addChild(levelLabel);
    
    // Level Stars
    int stars = win ? calculateLevelStars() : 0;
    // 1
    std::string star1Key = (stars >= 1) ? WIN_LOSE_STAR : WIN_LOSE_STAR_EMPTY;
    std::shared_ptr<PolygonNode> star1 = PolygonNode::allocWithTexture(_assets->get<Texture>(star1Key));
    float starWidth = star1->getContentSize().width/star1->getContentSize().height * starHeight;
    star1->setAnchor(Vec2::ANCHOR_CENTER);
    star1->setContentSize(starWidth, starHeight);
    star1->setPosition(_dimen.width*0.5f - starWidth - starOffset, starsY);
    _winloseNode->addChild(star1);
    // 2
    std::string star2Key = (stars >= 2) ? WIN_LOSE_STAR : WIN_LOSE_STAR_EMPTY;
    std::shared_ptr<PolygonNode> star2 = PolygonNode::allocWithTexture(_assets->get<Texture>(star2Key));
    star2->setAnchor(Vec2::ANCHOR_CENTER);
    star2->setContentSize(starWidth, starHeight);
    star2->setPosition(_dimen.width*0.5f, starsYUp);
    _winloseNode->addChild(star2);
    // 3
    std::string star3Key = (stars >= 3) ? WIN_LOSE_STAR : WIN_LOSE_STAR_EMPTY;
    std::shared_ptr<PolygonNode> star3 = PolygonNode::allocWithTexture(_assets->get<Texture>(star3Key));
    star3->setAnchor(Vec2::ANCHOR_CENTER);
    star3->setContentSize(starWidth, starHeight);
    star3->setPosition(_dimen.width*0.5f + starWidth + starOffset, starsY);
    _winloseNode->addChild(star3);
    if (!win) {
        Color4 color = star1->getColor();
        color.a = 127.0f;
        star1->setColor(color);
        star2->setColor(color);
        star3->setColor(color);
    }
    
    // High score stars
    int highStars = GameData::get()->getLevelStars(_level);
    // 1
    std::string highStar1Key = (highStars >= 1) ? WIN_LOSE_HIGH_STAR : WIN_LOSE_HIGH_STAR_EMPTY;
    std::shared_ptr<PolygonNode> highStar1 = PolygonNode::allocWithTexture(_assets->get<Texture>(highStar1Key));
    float highStarWidth = highStar1->getContentSize().width/highStar1->getContentSize().height * highStarHeight;
    highStar1->setAnchor(Vec2::ANCHOR_CENTER);
    highStar1->setContentSize(highStarWidth, highStarHeight);
    highStar1->setPosition(_dimen.width*0.5f - highStarWidth - highStarOffset, highStarsY);
    _winloseNode->addChild(highStar1);
    // 2
    std::string highStar2Key = (highStars >= 2) ? WIN_LOSE_HIGH_STAR : WIN_LOSE_HIGH_STAR_EMPTY;
    std::shared_ptr<PolygonNode> highStar2 = PolygonNode::allocWithTexture(_assets->get<Texture>(highStar2Key));
    highStar2->setAnchor(Vec2::ANCHOR_CENTER);
    highStar2->setContentSize(highStarWidth, highStarHeight);
    highStar2->setPosition(_dimen.width*0.5f, highStarsY);
    _winloseNode->addChild(highStar2);
    // 3
    std::string highStar3Key = (highStars >= 3) ? WIN_LOSE_HIGH_STAR : WIN_LOSE_HIGH_STAR_EMPTY;
    std::shared_ptr<PolygonNode> highStar3 = PolygonNode::allocWithTexture(_assets->get<Texture>(highStar3Key));
    highStar3->setAnchor(Vec2::ANCHOR_CENTER);
    highStar3->setContentSize(highStarWidth, highStarHeight);
    highStar3->setPosition(_dimen.width*0.5f + highStarWidth + highStarOffset, highStarsY);
    _winloseNode->addChild(highStar3);
    
    // High score text
    std::shared_ptr<Label> highscoreText = Label::alloc("HIGHSCORE", font);
    highscoreText->setAnchor(Vec2::ANCHOR_CENTER);
    highscoreText->setPosition(_dimen.width*0.5f, highscoreTextY);
    highscoreText->setScale(0.55f);
    highscoreText->setForeground(Color4::WHITE);
    _winloseNode->addChild(highscoreText);
    
    // Text
    if (!win) {
        std::string text = win ? "Victory!" : "You Lose";
        std::shared_ptr<Label> textLabel = Label::alloc(text, font);
        textLabel->setAnchor(Vec2::ANCHOR_CENTER);
        textLabel->setPosition(_dimen.width*0.5f, textY);
        textLabel->setScale(2.5f);
        textLabel->setForeground(Color4::WHITE);
        _winloseNode->addChild(textLabel);
    }
    
    // Add Node to World
    _worldNode->addChild(_winloseNode, 1000);
    _worldNode->sortZOrder();
    
    // Disable Menu Buttons
    if (_resetButton && _resetButton->isActive()) {
        _resetButton->deactivate();
    }
    if (_exitButton && _exitButton->isActive()) {
        _exitButton->deactivate();
    }
    if (_soundButton && _soundButton->isActive()) {
        _soundButton->deactivate();
    }
}

/** Go to the next level. */
void PlayMode::nextLevel() {
    // Activate Menu Buttons
    if (_resetButton && !_resetButton->isActive()) {
        _resetButton->activate(PLAY_MENU_LISTENER_RESTART);
    }
    if (_exitButton && !_exitButton->isActive()) {
        _exitButton->activate(PLAY_MENU_LISTENER_EXIT);
    }
    if (_soundButton && !_soundButton->isActive()) {
        _soundButton->activate(PLAY_MENU_LISTENER_SOUND);
    }
    _level++;
    
    // Check if past last level
    std::shared_ptr<JsonReader> levelsReader = JsonReader::allocWithAsset("json/levelList.json");
    std::shared_ptr<JsonValue> levelsJson = levelsReader->readJson();
    if (_level >= levelsJson->get("levels")->size()) {
        _level--;
        exit();
    } else {
        reset();
    }
}

/** Go to the level select screen. */
void PlayMode::levelMenu() {
    // Activate Menu Buttons
    if (_resetButton && !_resetButton->isActive()) {
        _resetButton->activate(PLAY_MENU_LISTENER_RESTART);
    }
    if (_exitButton && !_exitButton->isActive()) {
        _exitButton->activate(PLAY_MENU_LISTENER_EXIT);
    }
    if (_soundButton && !_soundButton->isActive()) {
        _soundButton->activate(PLAY_MENU_LISTENER_SOUND);
    }
    if (win) {
        _level++;
    }
    
    // Check if past last level
    std::shared_ptr<JsonReader> levelsReader = JsonReader::allocWithAsset("json/levelList.json");
    std::shared_ptr<JsonValue> levelsJson = levelsReader->readJson();
    if (_level >= levelsJson->get("levels")->size()) {
        _level--;
        exit();
    }
    exit();
}

/** Retry the level. */
void PlayMode::retryLevel() {
    // Activate Menu Buttons
    if (_resetButton && !_resetButton->isActive()) {
        _resetButton->activate(PLAY_MENU_LISTENER_RESTART);
    }
    if (_exitButton && !_exitButton->isActive()) {
        _exitButton->activate(PLAY_MENU_LISTENER_EXIT);
    }
    if (_soundButton && !_soundButton->isActive()) {
        _soundButton->activate(PLAY_MENU_LISTENER_SOUND);
    }
    reset();
}

