#include "TutorialScenes.h"

using namespace cugl;

#define LISTENER_ID 100 //Just making sure not to interfere with anything else
#define SCENE_WIDTH 1024
#define NUM_TUTORIAL_IMAGES 5

TutorialScenes::TutorialScenes() : Scene() {
}

bool TutorialScenes::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController> &input) {
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width;
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene::init(dimen)) {
		return false;
	}

	_assets = assets;
	_dimen = dimen;

	_input = input;
	_input->init(getCamera());
	_input->clear();

	auto layer = assets->get<Node>("tutorial");
	layer->setContentSize(dimen);
	layer->doLayout();
	addChild(layer);
	done = false;



	_button = std::dynamic_pointer_cast<Button>(assets->get<Node>("tutorial_trigger"));
	_tutorialImage = std::dynamic_pointer_cast<TexturedNode>(assets->get<Node>("tutorial_trigger_up_img"));
	_counter = 1;
	_button->setListener([=](const std::string& name, bool down) {
		if (down) {
			this->_counter++;
			if (_counter <= NUM_TUTORIAL_IMAGES) {
				_tutorialImage->setTexture(_assets->get<Texture>("tutorial" + std::to_string(_counter)));
				_tutorialImage->refresh();
			}
			else {
				this->done = true;
			}
		}
	});

	if (_active) {
		_button->activate(LISTENER_ID);
	}

	return true;
}

void TutorialScenes::dispose() {
	removeAllChildren();
	_assets = nullptr;
	_input = nullptr;
	_button = nullptr;
	_tutorialImage = nullptr;
	_active = false;
	Scene::dispose();


}

void TutorialScenes::setActive(bool value) {
	_active = value;
	if (value && !_button->isActive()) {
		_button->activate(LISTENER_ID);
	}
	else if (!value && _button->isActive()) {
		_button->deactivate();
	}
}