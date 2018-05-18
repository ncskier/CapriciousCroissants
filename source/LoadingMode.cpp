//
//  LoadingMode.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "LoadingMode.h"

using namespace cugl;

/** The ID for the button listener */
#define LISTENER_ID 1
/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

#pragma mark -
#pragma mark Constructors

/**
 * Initializes the controller contents, making it ready for loading
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool LoadingMode::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/loading.json");
    auto layer = assets->get<Node>("load");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    
	std::shared_ptr<Texture> bgBar = assets->get<Texture>("progressbar_background");
	std::shared_ptr<Texture> fgBar = assets->get<Texture>("progressbar_foreground");
	_bar = ProgressBar::alloc(bgBar, fgBar);
	_bar->setAnchor(0.5f, 0.5f);
	
	std::shared_ptr<Node> bg = layer->getChildByName("background");
	float bgheight = bg->getContentHeight();
	float heightscale = dimen.height / bgheight;
	bg->setScale(heightscale);


	layer->getChildByName("background")->addChildWithName(_bar, "bar");
	_bar->setPosition(480, 140);



    //_bar = std::dynamic_pointer_cast<ProgressBar>(assets->get<Node>("load_bar"));
    _button = std::dynamic_pointer_cast<Button>(assets->get<Node>("load_background_play"));
    _button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
    });
    
	_button->setPosition(480, 140);
    Application::get()->setClearColor(Color4(192,192,192,255));
    addChild(layer);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LoadingMode::dispose() {
    // Deactivate the button (platform dependent)
    if (isPending()) {
        _button->deactivate();
    }
    _button = nullptr;
    _bar = nullptr;
    _assets = nullptr;
    _progress = 0.0f;
}


#pragma mark -
#pragma mark Progress Monitoring
/**
 * The method called to update the game mode.
 *
 * This method updates the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LoadingMode::update(float progress) {
    if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
			_button->setZOrder(1);
			_bar->setZOrder(0);
			auto layer = _assets->get<Node>("load_background");
			layer->sortZOrder();
            _progress = 1.0f;
            _button->setVisible(true);
            _button->activate(1);
        }
        _bar->setProgress(_progress);
    }
}

/**
 * Returns true if loading is complete, but the player has not pressed play
 *
 * @return true if loading is complete, but the player has not pressed play
 */
bool LoadingMode::isPending( ) const {
    return _button != nullptr && _button->isVisible();
}
