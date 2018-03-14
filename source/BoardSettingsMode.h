//
//  LoadingMode.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Settings_Mode_H__
#define __Settings_Mode_H__

#include <cugl/cugl.h>

/**
 * This class is a simple loading screen for asychronous asset loading.
 *
 * The screen will display a very minimal progress bar that displays the
 * status of the asset manager.  Make sure that all asychronous load requests
 * are issued BEFORE calling update for the first time, or else this screen
 * will think that asset loading is complete.
 *
 * Once asset loading is completed, it will display a play button.  Clicking
 * this button will inform the application root to switch to the gameplay mode.
 */
class BoardSettingsMode : public cugl::Scene {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // NO CONTROLLER (ALL IN SEPARATE THREAD)
    
    // VIEW
    std::shared_ptr<cugl::Button> _upHeightButton;
	std::shared_ptr<cugl::Button> _downHeightButton;
	std::shared_ptr<cugl::Button> _upWidthButton;
	std::shared_ptr<cugl::Button> _downWidthButton;
	std::shared_ptr<cugl::Button> _upColorsButton;
	std::shared_ptr<cugl::Button> _downColorsButton;
	std::shared_ptr<cugl::Button> _upAlliesButton;
	std::shared_ptr<cugl::Button> _downAlliesButton;

	std::shared_ptr<cugl::Button> _upEnemiesButton;
	std::shared_ptr<cugl::Button> _downEnemiesButton;

	std::shared_ptr<cugl::Button> _placePawnsButton;
	std::shared_ptr<cugl::Button> _doneButton;

	std::shared_ptr<cugl::Label> _heightLabel;
	std::shared_ptr<cugl::Label> _widthLabel;
	std::shared_ptr<cugl::Label> _colorsLabel;
	std::shared_ptr<cugl::Label> _alliesLabel;

	std::shared_ptr<cugl::Label> _enemiesLabel;

	std::shared_ptr<cugl::PolygonNode> _checkmark;

	
    
    // MODEL
    /** The progress displayed on the screen */
    float _progress;
    /** Whether or not the player has pressed play to continue */
    bool  _completed;
    
    
public:
#pragma mark -
#pragma mark Constructors
	bool isDone = false;

	int height = 5;
	int width = 5;
	int colors = 4;
	int allies = 1;
	int enemies = 1;

	bool placePawns = false;

    /**
     * Creates a new loading mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
	BoardSettingsMode() : Scene(), _progress(0.0f) {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~BoardSettingsMode() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * This method updates the progress bar amount.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
};

#endif /* __Loading_Mode_H__ */
