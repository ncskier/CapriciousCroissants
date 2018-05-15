//
//  GameData.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 5/15/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "GameData.h"
#include <sstream>
#include <fstream>

using namespace cugl;

/** Reference to the game data singleton */
GameData* GameData::_gData = nullptr;

#pragma mark -
#pragma mark Constructors
/** Constructor */
GameData::GameData() {
}

/** Dispose of all (non-static) resources */
void GameData::dispose() {
    _saveDir = nullptr;
    _settingsPath = nullptr;
    _settingsJson = nullptr;
}

/** Initialize GameData */
bool GameData::init(std::string saveDir) {
    _saveDir = saveDir;
    
    // SettingsPath
    std::stringstream ssPath;
    ssPath << saveDir << SETTINGS_FILENAME;
    _settingsPath = ssPath.str();
    
    // SettingsJson
    std::ifstream ifile(_settingsPath);
    if (!ifile) {
        CULog("init settings");
        initSettings();
    }
    std::shared_ptr<JsonReader> reader = JsonReader::alloc(_settingsPath);
    _settingsJson = reader->readJson();
    
    return true;
}


#pragma mark -
#pragma mark Helper Methods
/** Initialize the settings JSON file from INIT_SETTINGS_PATH */
void GameData::initSettings() {
    std::shared_ptr<JsonWriter> writer = JsonWriter::alloc(_settingsPath);
    std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(INIT_SETTINGS_PATH);
    std::shared_ptr<JsonValue> initSettingsJson = reader->readJson();
    writer->writeJson(initSettingsJson);
}


#pragma mark -
#pragma mark Static Accessors

/** Start the game data */
void GameData::start(std::string saveDir) {
    if (_gData != nullptr) {
        return;
    }
    _gData = new GameData();
    if (!_gData->init(saveDir)) {
        delete _gData;
        _gData = nullptr;
        CUAssertLog(false,"Game Data failed to initialize");
    }
}

/**
 * Stops the singleton game data, releasing all resources.
 *
 * Once this method is called, the method get() will return nullptr.
 * Calling the method multiple times (without calling stop) will have
 * no effect.
 */
void GameData::stop() {
    if (_gData == nullptr) {
        return;
    }
    delete _gData;
    _gData = nullptr;
}


#pragma mark -
#pragma mark Static Accessors/Mutators

/** Get the saved mute setting */
bool GameData::getMuteSetting() {
    std::shared_ptr<JsonValue> muteJson = _settingsJson->get(MUTE_KEY);
    return muteJson->asBool();
}

/** Set the saved mute setting */
void GameData::setMuteSetting(bool mute) {
    std::shared_ptr<JsonValue> muteJson = _settingsJson->get(MUTE_KEY);
    muteJson->set(mute);
    CULog("save json: \n%s", _settingsJson->toString().c_str());
    std::shared_ptr<JsonWriter> writer = JsonWriter::alloc(_settingsPath);
    writer->writeJson(_settingsJson);
}

