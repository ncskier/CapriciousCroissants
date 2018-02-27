//
// BoardScreenApp.cpp
// Cpp file for the scene that contains a board that is playable


#include "BoardScreenApp.h"
#include <cugl/base/CUBase.h>


using namespace cugl;

// This is adjusted by screen aspect ratio to get the height
#define GAME_WIDTH 1024

/**
* The method called after OpenGL is initialized, but before running the application.
*
* This is the method in which all user-defined program intialization should
* take place.  You should not create a new init() method.
*
* When overriding this method, you should call the parent method as the
* very last line.  This ensures that the state will transition to FOREGROUND,
* causing the application to run.
*/
void BoardScreenApp::onStartup() {
	Size size = getDisplaySize();
	size *= GAME_WIDTH / size.width;

	// Create a scene graph the same size as the window
	_scene = Scene::alloc(size.width, size.height);
	// Create a sprite batch (and background color) to render the scene
	_batch = SpriteBatch::alloc();
	setClearColor(Color4(229, 229, 229, 255));

	// Create an asset manager to load all assets
	_assets = AssetManager::alloc();

	// You have to attach the individual loaders for each asset type
	_assets->attach<Texture>(TextureLoader::alloc()->getHook());
	_assets->attach<Font>(FontLoader::alloc()->getHook());

	// This reads the given JSON file and uses it to load all other assets
	_assets->loadDirectory("json/assets.json");

	// Sets up a board of tiles
	_board = TileBoard::alloc();
    CULog("Initialize Board:\n%s", _board->toString().c_str());
    
    // Test get(x, y)
//    CULog("get(0,0) = %d", _board->get(0, 0));
//    CULog("get(1,0) = %d", _board->get(1, 0));
//    CULog("get(0,1) = %d", _board->get(0, 1));
    
    // Test Slide
//    _board->slideCol(0, 1);
//    CULog("slideCol(0, 1):\n%s", _board->toString().c_str());
//    _board->slideCol(1, 2);
//    CULog("slideCol(1, 2):\n%s", _board->toString().c_str());
//    _board->slideCol(2, 3);
//    CULog("slideCol(2, 3):\n%s", _board->toString().c_str());
//    _board->slideRow(3, 4);
//    CULog("slideRow(3, 4):\n%s", _board->toString().c_str());
//    _board->slideRow(4, 6);
//    CULog("slideRow(4, 6):\n%s", _board->toString().c_str());

	// Activate mouse or touch screen input as appropriate
	// We have to do this BEFORE the scene, because the scene has a button
#if defined (CU_TOUCH_SCREEN)
	Input::activate<Touchscreen>();
#else
	Input::activate<Mouse>();
#endif

	// Build the scene from these assets
	buildScene();

	Application::onStartup();
}

/**
* The method called when the application is ready to quit.
*
* This is the method to dispose of all resources allocated by this
* application.  As a rule of thumb, everything created in onStartup()
* should be deleted here.
*
* When overriding this method, you should call the parent method as the
* very last line.  This ensures that the state will transition to NONE,
* causing the application to be deleted.
*/
void BoardScreenApp::onShutdown() {
	// Delete all smart pointers
	_scene = nullptr;
	_batch = nullptr;
	_assets = nullptr;
	_board->~TileBoard();
	_board = nullptr;

	// Deativate input
#if defined CU_TOUCH_SCREEN
	Input::deactivate<Touchscreen>();
#else
	Input::deactivate<Mouse>();
#endif
	Application::onShutdown();
}

/**
* The method called to update the application data.
*
* This is your core loop and should be replaced with your custom implementation.
* This method should contain any code that is not an OpenGL call.
*
* When overriding this method, you do not need to call the parent method
* at all. The default implmentation does nothing.
*
* @param timestep  The amount of time (in seconds) since the last frame
*/
void BoardScreenApp::update(float timestep) {

}

/**
* The method called to draw the application to the screen.
*
* This is your core loop and should be replaced with your custom implementation.
* This method should OpenGL and related drawing calls.
*
* When overriding this method, you do not need to call the parent method
* at all. The default implmentation does nothing.
*/
void BoardScreenApp::draw() {
	// This takes care of begin/end
	_scene->render(_batch);
}

/**
* Internal helper to build the scene graph.
*
* Scene graphs are not required.  You could manage all scenes just like
* you do in 3152.  However, they greatly simplify scene management, and
* have become standard in most game engines.
*/
void BoardScreenApp::buildScene() {
	Size size = getDisplaySize();
	size *= GAME_WIDTH / size.width;


	// Left this for reference

	//// Get the image and add it to the node.
	//std::shared_ptr<Texture> texture = _assets->get<Texture>("claw");
	//std::shared_ptr<PolygonNode> claw = PolygonNode::allocWithTexture(texture);
	//claw->setScale(0.1f); // Magic number to rescale asset
	//claw->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
	//claw->setPosition(0, 0);

	//// Get the font and make a label for the logo
	//std::shared_ptr<Font> font = _assets->get<Font>("charlemagne");
	//std::shared_ptr<Label> label = Label::alloc("CUGL", font);
	//label->setAnchor(Vec2::ANCHOR_TOP_CENTER);
	//label->setPosition(15, -15); // Magic numbers for some manual kerning
	//_logo->addChild(label);

	//// Put the logo in the middle of the screen
	//_logo->setAnchor(Vec2::ANCHOR_CENTER);
	//_logo->setPosition(size.width / 2, size.height / 2);


	// Create a button.  A button has an up image and a down image
	std::shared_ptr<Texture> up = _assets->get<Texture>("close-normal");
	std::shared_ptr<Texture> down = _assets->get<Texture>("close-selected");

	Size bsize = up->getSize();
	std::shared_ptr<Button> button = Button::alloc(PolygonNode::allocWithTexture(up),
		PolygonNode::allocWithTexture(down));

	// Create a callback function for the button
	button->setName("close");
	button->setListener([=](const std::string& name, bool down) {
		// Only quit when the button is released
		if (!down) {
			CULog("Goodbye!");
			this->quit();
		}
	});

	// Position the button in the bottom right corner
	button->setAnchor(Vec2::ANCHOR_CENTER);
	button->setPosition(size.width - bsize.width / 2, bsize.height / 2);

	// Add the logo and button to the scene graph
	_scene->addChild(button);

	// We can only activate a button AFTER it is added to a scene
	button->activate(1);
}
