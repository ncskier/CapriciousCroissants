//
//  PlayMode.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

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
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    _input.init();
    
    // Create board
    populate();
    _state = State::PLAYER;
    _active = true;
    _complete = false;
    setDebug(false);
    
    // Start up turn controllers
    _playerController.init(_board, &_input);
    _boardController.init(_board);
    _enemyController.init(_board);
    
    // Set Background
    Application::get()->setClearColor(Color4(229, 229, 229, 255));
    
    return true;
}

bool PlayMode::init(const std::shared_ptr<AssetManager>& assets, int width, int height, int colors, int allies, bool placePawn) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene::init(dimen)) {
		return false;
	}

	// Start up the input handler
	_assets = assets;
	_input.init();

	// Create board
	populate(width, height, colors, allies, placePawn);
	_state = State::PLAYER;
	_active = true;
	_complete = false;
	setDebug(false);

	// Start up turn controllers
	_playerController.init(_board, &_input);
	_boardController.init(_board);
	_enemyController.init(_board);

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
    populate();
    CULog("populated board:\n%s", _board->toString().c_str());
}

/**
 * Lays out the game geography.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
void PlayMode::populate() {
    _board = BoardModel::alloc(5, 5);
    _board->tileTexture = _assets->get<Texture>("100squareWhite");
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    _board->gameWidth = dimen.width;
    _board->gameHeight = dimen.height;
}

void PlayMode::populate(int height, int width, int colors, int allies, bool place) {
	_board = BoardModel::alloc(width, height, colors, allies, place);
	_board->tileTexture = _assets->get<Texture>("100squareWhite");
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	_board->gameWidth = dimen.width;
	_board->gameHeight = dimen.height;
}

#pragma mark -
#pragma mark Gameplay Handling

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
    _input.update(dt);
    
    // Update
//    CULog("PlayMode Update");
    if (_state == State::PLAYER) {
        // PLAYER turn
        _playerController.update(dt);
        if (_playerController.isComplete()) {
            _state = State::BOARD;
            _playerController.reset();
        }
    } else if (_state == State::BOARD) {
        // BOARD turn
        _boardController.update(dt);
        if (_boardController.isComplete()) {
            _state = State::ENEMY;
            _boardController.reset();
        }
    } else {
        // ENEMY turn
        _enemyController.update(dt);
        if (_enemyController.isComplete()) {
            _state = State::PLAYER;
            _enemyController.reset();
        }
    }
}

/**
 * Draw the game
 *
 * @param batch     The SpriteBatch to draw with.
 */
void PlayMode::draw(const std::shared_ptr<SpriteBatch>& batch) {
    // Render anything on the SceneGraph
    render(batch);
    
    // Draw the Board
    _board->draw(batch);
}
