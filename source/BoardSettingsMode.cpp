//
//  LoadingMode.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "BoardSettingsMode.h"

using namespace cugl;

/** The ID for the button listener */
#define LISTENER_ID 2
/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

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
bool BoardSettingsMode::init(const std::shared_ptr<cugl::AssetManager>& assets) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene::init(dimen)) {
		return false;
	}


	_assets = assets;
	auto layer = assets->get<Node>("settings");
	layer->setContentSize(dimen);
	layer->doLayout(); // This rearranges the children to fit the screen
	addChild(layer);

	Vec2 position;

	_heightLabel = std::dynamic_pointer_cast<Label>(assets->get<Node>("settings_labelheight"));
	_heightLabel->setPosition(position.set(626, 576 - 97));
	_widthLabel = std::dynamic_pointer_cast<Label>(assets->get<Node>("settings_labelwidth"));
	_widthLabel->setPosition(position.set(626, 576 - 171));
	_colorsLabel = std::dynamic_pointer_cast<Label>(assets->get<Node>("settings_labelcolors"));
	_colorsLabel->setPosition(position.set(626, 576 - 288));
	_alliesLabel = std::dynamic_pointer_cast<Label>(assets->get<Node>("settings_labelallies"));
	_alliesLabel->setPosition(position.set(626, 576 - 360));

	_checkmark = std::dynamic_pointer_cast<PolygonNode>(assets->get<Node>("settings_check"));
	_checkmark->setPosition(position.set(594, 576 - 505));
	_checkmark->setVisible(placePawns);


	_heightLabel->setText(std::to_string(height), true);
	_widthLabel->setText(std::to_string(width), true);
	_colorsLabel->setText(std::to_string(colors), true);
	_alliesLabel->setText(std::to_string(allies), true);




	_upHeightButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_upheight"));
	_upHeightButton->setPosition(position.set(734, 576 - 93));
	_upHeightButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			height++;
			if (height > 10) {
				height = 10;
			}
			_heightLabel->setText(std::to_string(height), true);
		}
	});
	_upHeightButton->activate(1);

	_downHeightButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_downheight"));
	_downHeightButton->setPosition(position.set(734, 576 - 125));
	_downHeightButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			height--;
			if (height < 1) {
				height = 1;
			}
			_heightLabel->setText(std::to_string(height), true);
		}
	});
	_downHeightButton->activate(2);

	_upWidthButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_upwidth"));
	_upWidthButton->setPosition(position.set(734, 576 - 168));
	_upWidthButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			width++;
			if (width > 10) {
				width = 10;
			}
			_widthLabel->setText(std::to_string(width), true);
		}
	});
	_upWidthButton->activate(3);

	_downWidthButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_downwidth"));
	_downWidthButton->setPosition(position.set(734, 576 - 200));
	_downWidthButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			width--;
			if (width < 1) {
				width = 1;
			}
			_widthLabel->setText(std::to_string(width), true);
		}
	});
	_downWidthButton->activate(4);

	_upColorsButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_upcolors"));
	_upColorsButton->setPosition(position.set(734, 576 - 285));
	_upColorsButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			colors++;
			if (colors > 8) {
				colors = 8;
			}
			_colorsLabel->setText(std::to_string(colors), true);
		}
	});
	_upColorsButton->activate(5);

	_downColorsButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_downcolors"));
	_downColorsButton->setPosition(position.set(734, 576 - 316));
	_downColorsButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			colors--;
			if (colors < 3) {
				colors = 3;
			}
			_colorsLabel->setText(std::to_string(colors), true);
		}
	});
	_downColorsButton->activate(6);

	_upAlliesButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_upallies"));
	_upAlliesButton->setPosition(position.set(734, 576 - 358));
	_upAlliesButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			allies++;
			if (allies > 5) {
				allies = 5;
			}
			_alliesLabel->setText(std::to_string(allies), true);
		}
	});
	_upAlliesButton->activate(7);

	_downAlliesButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_downallies"));
	_downAlliesButton->setPosition(position.set(734, 576 - 390));
	_downAlliesButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			allies--;
			if (allies < 0) {
				allies = 0;
			}
			_alliesLabel->setText(std::to_string(allies), true);
		}
	});
	_downAlliesButton->activate(8);

	_placePawnsButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_placepawns"));
	_placePawnsButton->setPosition(position.set(590, 576 - 512));
	_placePawnsButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			placePawns = !placePawns;
			_checkmark->setVisible(placePawns);
		}
	});
	_placePawnsButton->activate(9);

	_doneButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("settings_done"));
	_doneButton->setPosition(position.set(835, 576 - 561));
	_doneButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			this->isDone = true;
		}
	});
	_doneButton->activate(10);



	Application::get()->setClearColor(Color4(192, 192, 192, 255));

	return true;
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void BoardSettingsMode::dispose() {
	// Deactivate the button (platform dependent)

	if (_upHeightButton) {
		_upHeightButton->deactivate();
	}
	if (_upWidthButton) {
		_upWidthButton->deactivate();
	}
	if (_upColorsButton) {
		_upColorsButton->deactivate();
	}
	if (_upAlliesButton) {
		_upAlliesButton->deactivate();
	}
	if (_downHeightButton) {
		_downHeightButton->deactivate();
	}
	if (_downWidthButton) {
		_downWidthButton->deactivate();
	}
	if (_downColorsButton) {
		_downColorsButton->deactivate();
	}
	if (_downAlliesButton) {
		_downAlliesButton->deactivate();
	}
	if (_doneButton) {
		_doneButton->deactivate();
	}

	_upHeightButton = nullptr;
	_downHeightButton = nullptr;
	_upWidthButton = nullptr;
	_downWidthButton = nullptr;
	_upColorsButton = nullptr;
	_downColorsButton = nullptr;
	_upAlliesButton = nullptr;
	_downAlliesButton = nullptr;
	_placePawnsButton = nullptr;
	_doneButton = nullptr;

	_heightLabel = nullptr;
	_widthLabel = nullptr;
	_colorsLabel = nullptr;
	_alliesLabel = nullptr;

	_assets = nullptr;
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
void BoardSettingsMode::update(float progress) {
}
