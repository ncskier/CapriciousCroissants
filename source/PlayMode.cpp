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
_debug(false)
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
bool PlayMode::init(const std::shared_ptr<AssetManager>& assets) {
    return init(assets, 5, 5, 5, 3, 5, false);
}

bool PlayMode::init(const std::shared_ptr<AssetManager>& assets, int width, int height, int colors, int allies, int enemies, bool placePawn) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	} else if (!Scene::init(dimen)) {
		return false;
	}

	_assets = assets;
    _actions = ActionManager::alloc();

	_entityManager = std::make_shared<EntityManager>();

	// Initialize all systems that are used for playin
	_entityManager->addSystem(std::make_shared<MovementDumbSystem>(_entityManager), EntityManager::movement);

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
    
    // Setup Touch Node
    _touchNode = AnimationNode::alloc(assets->get<Texture>("touch"), 6, 8, 48);
    _touchNode->setFrame(0);
    _touchNode->setZOrder(1000);
    _touchNode->setVisible(false);
    addChild(_touchNode);
    sortZOrder();
    _touchAction = Animate::alloc(0, 47, 1.2f);

    // Setup state
	_state = State::PLAYER;
	_active = true;
	_complete = false;
	setDebug(false);
    
    // Create board
    populate(width, height, colors, allies, enemies, placePawn, dimen);
    
    // Setup Input handler
    _input.init(getCamera());

	// Start up turn controllers
	_playerController.init(_actions, _board, &_input, _entityManager);
	_boardController.init(_actions, _board, _entityManager);
	_enemyController.init(_actions, _board, _entityManager);

	// Set Background
	Application::get()->setClearColor(Color4(229, 229, 229, 255));

	return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void PlayMode::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _active = false;
        _complete = false;
        _debug = false;
        _state = State::PLAYER;
    }
}


#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void PlayMode::reset() {
    setComplete(false);
//    populate();
}

/**
 * Lays out the game geography.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
//void PlayMode::populate() {
//    _board = BoardModel::alloc(5, 5);
//    _board->tileTexture = _assets->get<Texture>("100squareWhite");
//    _board->playerTexture = _assets->get<Texture>("player");
//    Size dimen = Application::get()->getDisplaySize();
//    CULog("dimen: %s", dimen.toString().c_str());
//    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
//    _board->gameWidth = dimen.width;
//    _board->gameHeight = dimen.height;
//    CULog("scale: %s", dimen.toString().c_str());
//}

void PlayMode::populate(int height, int width, int colors, int allies, int enemies, bool placePawn, Size dimen) {
    // Create board
    _board = BoardModel::alloc(width, height, colors, allies, enemies, placePawn, _assets, dimen);
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
    std::set<std::shared_ptr<EnemyPawnModel>>::iterator enemyIter;
    for (enemyIter = _board->getAddedEnemies().begin(); enemyIter != _board->getAddedEnemies().end(); ++enemyIter) {
        _board->getNode()->addChild((*enemyIter)->getSprite());
        std::stringstream key;
        key << "int_add_enemy_" << i;
        _actions->activate(key.str(), _board->enemyAddAction, (*enemyIter)->getSprite());
        i++;
    }
    _board->clearAddedEnemies();

	//Create entities and register them to manager
	{ // EXAMPLE ENTITY 0
		EntityId newEntity = _entityManager->createEntity();
		LocationComponent loc;
		loc.x = 5;
		loc.y = 5;
		DumbMovementComponent move;
		move.movementDistance = 0;
		_entityManager->addComponent<DumbMovementComponent>(newEntity, move);
		_entityManager->addComponent<LocationComponent>(newEntity, loc);
		 _entityManager->registerEntity(newEntity);
	}
	{ // EXAMPLE ENTITY 1
		EntityId newEntity = _entityManager->createEntity();
		LocationComponent loc;
		loc.x = 3;
		loc.y = 3;
		_entityManager->addComponent<LocationComponent>(newEntity, loc);
		_entityManager->registerEntity(newEntity);
	}


}


#pragma mark -
#pragma mark Gameplay Handling

/** Update touch node */
void PlayMode::updateTouchNode() {
    if (_input.getMoveEvent() != InputController::MoveEvent::NONE) {
        // Activate action
        if (!_actions->isActive("touchAction") && !_actions->isActive("touchFade")) {
            _actions->activate("touchAction", _touchAction, _touchNode);
        }
        _touchNode->setPosition(_input.getTouchPosition());
        _touchNode->setVisible(true);
    } else {
        if (!_actions->isActive("touchAction")) {
            _touchNode->setVisible(false);
        }
    }
}

/** Update for player turn */
void PlayMode::updatePlayerTurn(float dt) {
    _playerController.update(dt);
    if (_playerController.isComplete()) {
        _state = State::BOARD;
        _playerController.reset();
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
        _state = State::ENEMY;
        _boardController.reset();
    }
}


/** Update for enemy turn */
void PlayMode::updateEnemyTurn(float dt) {
    _enemyController.update(dt);
    if (_enemyController.isComplete()) {
        if (_enemyController.lose) {
            done = true;
            win = false;
            
            _text->setText("You lose");
            _text->setVisible(true);
            _text->setZOrder(1000);
            sortZOrder();
        }
        _state = State::PLAYER;

		{	//EXAMPLE CODE FOR DUMB MOVEMENT SYSTEM TO SHOW UPDATING ONE ENTITY(HAS DUMBMOVEMENT) BUT NOT OTHER (DOESN'T HAVE DUMBMOVEMENT)
			LocationComponent loc = _entityManager->getComponent<LocationComponent>(0);
			CULog("Should change %d, %d", loc.x, loc.y);
			loc = _entityManager->getComponent<LocationComponent>(1);
			CULog("Shouldn't change %d, %d", loc.x, loc.y);
		}

        _enemyController.reset();
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
    _input.update(dt);
    
    // Update touch node
    updateTouchNode();
    
    // Update actions
    _actions->update(dt);
    
    // Check for interrupting animations
    if (_playerController.getInterruptingActions().empty() && _boardController.getInterruptingActions().empty() && _enemyController.getInterruptingActions().empty()) {
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
        }
    } else {
        // Update Interrupting Animations
        if (!_playerController.getInterruptingActions().empty()) { updateInterruptingAnimations(_playerController.getInterruptingActions()); }
        if (!_boardController.getInterruptingActions().empty()) { updateInterruptingAnimations(_boardController.getInterruptingActions()); }
        if (!_enemyController.getInterruptingActions().empty()) { updateInterruptingAnimations(_enemyController.getInterruptingActions()); }
        _input.clear();
    }
}

/**
 * Draw the game
 *
 * @param batch     The SpriteBatch to draw with.
 */
void PlayMode::draw(const std::shared_ptr<SpriteBatch>& batch) {
    // Draw the Board
//    _board->draw(batch);

    // Render anything on the SceneGraph
    render(batch);
}

