#pragma once

#ifndef __Tutorial_Mode_H__
#define __Tutorial_Mode_H__

#include <cugl/cugl.h>
#include "InputController.h"

class TutorialScenes : public cugl::Scene {
protected:
	std::shared_ptr<cugl::AssetManager> _assets;

	// TODO: VIEW
	cugl::Size _dimen;
	std::shared_ptr<InputController> _input;
	std::shared_ptr<cugl::Button> _button;
	std::shared_ptr<cugl::TexturedNode> _tutorialImage;
	int _counter;

public:
	bool done = false;

	bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<InputController>& input);

	TutorialScenes();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources, like the input controller.
	*/
	~TutorialScenes() { dispose(); }

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	void dispose();

	static std::shared_ptr<TutorialScenes> alloc(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<InputController>& input) {
		std::shared_ptr<TutorialScenes> result = std::make_shared<TutorialScenes>();
		return (result->init(assets, input) ? result : nullptr);
	}

	/**
	* Sets whether the scene is currently active
	*
	* @param value whether the scene is currently active
	*/
	virtual void setActive(bool value) override;
};

#endif /* __Tutorial_Mode_H__ */