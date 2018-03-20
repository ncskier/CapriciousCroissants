//
//  BoxApp.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "BoxApp.h"

using namespace cugl;

#pragma mark -
#pragma mark Application State

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
void BoxApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    
    // Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<Node>(SceneLoader::alloc()->getHook());
    
    // Create a "loading" screen
    _loaded = false;
    _loading.init(_assets);
	_inSettings = false;
	_inGameplay = false;
    
    // Que up the other assets
    AudioEngine::start();
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    
    Application::onStartup(); // YOU MUST END with call to parent
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
void BoxApp::onShutdown() {
    _loading.dispose();
    _gameplay.dispose();
	_settings.dispose();
    _assets = nullptr;
    _batch = nullptr;
    
    // Shutdown input
#ifdef CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    
    AudioEngine::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void BoxApp::onSuspend() {
    AudioEngine::get()->pauseAll();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void BoxApp::onResume() {
    AudioEngine::get()->resumeAll();
}


#pragma mark -
#pragma mark Application Loop

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
void BoxApp::update(float timestep) {
    if (!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (!_loaded && !_inSettings) {
        _loading.dispose(); // Disables the input listeners in this mode
		_settings.init(_assets);
		_inSettings = true;
		_loaded = true;
    } else if (_inSettings && !_inGameplay && !_settings.isDone){
		_settings.update(timestep);
	} else if (_inSettings && !_inGameplay) {
		_settings.dispose();

		//At this point you can get values from settings with _settings.height, _settings.width, _settings.allies, _settings.colors, _settings.placePawns

		_gameplay.init(_assets, _settings.height, _settings.width, _settings.colors, _settings.allies, _settings.enemies, _settings.placePawns);
		_inSettings = false;
		_inGameplay = true;
	} else {
		_gameplay.update(timestep);
	}
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
void BoxApp::draw() {
    if (!_loaded) {
        _loading.render(_batch);
    } else  if (_inSettings) {
		_settings.render(_batch);
	} else {
//        _gameplay.render(_batch);
        _gameplay.draw(_b{atch);
	}
}

