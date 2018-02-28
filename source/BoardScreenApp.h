//
// BoardScreenApp.h
// Header file for the scene that contains a board that is playable

#ifndef __BSCREEN_APP_H__
#define __BSCREEN_APP_H__
#include <cugl/cugl.h>
#include "TileBoard.h"


/**
* Class for the main board of game
*
* Holds reference to a board and monitors player inputs to apply to board
*
*/


class BoardScreenApp : public cugl::Application {
protected:
	std::shared_ptr<cugl::AssetManager> _assets;
	std::shared_ptr<cugl::Scene> _scene;
	std::shared_ptr<cugl::SpriteBatch> _batch;
	std::shared_ptr<TileBoard> _board;

	//Temp Variables
	bool showHighlight = false;
	float highlightX;
	float highlightY;
	std::shared_ptr<cugl::Texture> tileAsset;
	float usedSize;


	float startX;
	float startY;
	float moveSince;
	bool moveVert;
	bool isMoving;
	bool finishedMove = false;

	void buildScene();

public:
	BoardScreenApp() : Application() {}

	~BoardScreenApp() {}

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
	virtual void onStartup() override;

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
	virtual void onShutdown() override;

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
	virtual void update(float timestep) override;

	/**
	* The method called to draw the application to the screen.
	*
	* This is your core loop and should be replaced with your custom implementation.
	* This method should OpenGL and related drawing calls.
	*
	* When overriding this method, you do not need to call the parent method
	* at all. The default implmentation does nothing.
	*/
	virtual void draw() override;

};

#endif
