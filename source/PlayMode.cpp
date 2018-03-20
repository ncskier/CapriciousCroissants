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

//    _worldNode = Node::allocWithBounds(dimen);
    _worldNode = _assets->get<Node>("game");
    _worldNode->setContentSize(dimen);
    _worldNode->setAnchor(Vec2::ZERO);
    _worldNode->doLayout(); // This rearranges the children to fit the screen
	addChild(_worldNode);

    // Setup win/lose text
	_text = std::dynamic_pointer_cast<Label>(assets->get<Node>("game_labelend"));
	_text->setVisible(false);

	// Create board
    auto boardNode = Node::alloc();
//    boardNode->setContentSize(dimen);
//    _worldNode->addChild(boardNode);
    _board = BoardModel::alloc(width, height, colors, allies, enemies, placePawn, _assets, dimen);
//    _board->getNode()->setContentSize(dimen);
    _worldNode->addChild(_board->getNode());
//    std::shared_ptr<TileModel> tile = TileModel::alloc(3);
//    tile->setSprite(_assets);
//    boardNode->addChild(tile->getSprite());
    
//    auto tileNode = AnimationNode::alloc(_assets->get<Texture>("tile1_strip"), TILE_IMG_ROWS, TILE_IMG_COLS, TILE_IMG_SIZE);
//    auto playerNode = AnimationNode::alloc(_assets->get<Texture>("player1_strip"), PLAYER_IMG_ROWS, PLAYER_IMG_COLS, PLAYER_IMG_SIZE);
//    auto enemyNode = AnimationNode::alloc(_assets->get<Texture>("enemy1_strip"), ENEMY_IMG_ROWS, ENEMY_IMG_COLS, ENEMY_IMG_SIZE);
//    boardNode->addChild(tileNode, 2);
//    boardNode->addChild(playerNode, 1);
//    boardNode->addChild(enemyNode, 0);
//    populate(width, height, colors, allies, enemies, placePawn);
	_state = State::PLAYER;
	_active = true;
	_complete = false;
	setDebug(false);
    
    // Setup Input handler
    _input.init(getCamera());

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
//
//void PlayMode::populate(int height, int width, int colors, int allies, int enemies, bool place) {
//    _board = BoardModel::alloc(width, height, colors, allies, enemies, place);
//    _board->tileTexture = _assets->get<Texture>("100squareWhite");
//    _board->playerTexture = _assets->get<Texture>("player");
//    _board->tile1Texture = _assets->get<Texture>("tile1");
//    _board->tile2Texture = _assets->get<Texture>("tile2");
//    Size dimen = Application::get()->getDisplaySize();
//    dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
//    _board->gameWidth = dimen.width;
//    _board->gameHeight = dimen.height;
//}

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
	if (!done) {
		//    CULog("PlayMode Update");
		if (_state == State::PLAYER) {
			// PLAYER turn
			_playerController.update(dt);
			if (_playerController.isComplete()) {
				_state = State::BOARD;
				_playerController.reset();
			}
		}
		else if (_state == State::BOARD) {
			// BOARD turn
			_boardController.update(dt);
			if (_boardController.isComplete()) {
				if (_boardController.win) {
					done = true;
					win = true;

					_text->setText("You win");
					_text->setVisible(true);
				}
				_state = State::ENEMY;
				_boardController.reset();
			}
		}
		else {
			// ENEMY turn
			_enemyController.update(dt);
			if (_enemyController.isComplete()) {
				if (_enemyController.lose) {
					done = true;
					win = false;

					_text->setText("You lose");
					_text->setVisible(true);
				}
				_state = State::PLAYER;
				_enemyController.reset();
			}
		}
	}
    _board->updateNodes();
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

