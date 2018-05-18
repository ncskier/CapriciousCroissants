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
    _settingsJson = nullptr;
    _levelsJson = nullptr;
    _levelListJson = nullptr;
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
    
    // LevelsJson
    if (!_settingsJson->has(LEVELS_KEY)) {
        _settingsJson->appendChild(LEVELS_KEY, JsonValue::allocObject());
    }
    _levelsJson = _settingsJson->get(LEVELS_KEY);
    
    // LevelListJson
    _levelListJson = JsonReader::allocWithAsset(LEVEL_LIST_PATH)->readJson()->get("levels");
    
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

/** Initializes the [level] JSON */
void GameData::initLevelData(int level) {
    std::shared_ptr<JsonValue> starsData = JsonValue::alloc(0.0f);
    std::shared_ptr<JsonValue> movesData = JsonValue::alloc(0.0f);
    std::shared_ptr<JsonValue> levelData = JsonValue::allocObject();
    levelData->appendChild(STARS_KEY, starsData);
    levelData->appendChild(MOVES_KEY, movesData);
    _levelsJson->appendChild(to_string(level), levelData);
    saveSettings();
}

/** Returns reference to the level JsonValue */
std::shared_ptr<JsonValue> GameData::getLevelJson(int level) {
    std::string levelKey = std::to_string(level);
    if (!_levelsJson->has(levelKey)) {
        initLevelData(level);
    }
    return _levelsJson->get(levelKey);
}

/** Writes settings json */
void GameData::saveSettings() {
//    CULog("save json: \n%s", _settingsJson->toString().c_str());
    std::shared_ptr<JsonWriter> writer = JsonWriter::alloc(_settingsPath);
    writer->writeJson(_settingsJson);
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
    saveSettings();
}

/** Get the saved level number of stars */
int GameData::getLevelStars(int level) {
    std::shared_ptr<JsonValue> starsJson = getLevelJson(level)->get(STARS_KEY);
    return starsJson->asInt();
}

/** Set the saved level number of stars */
void GameData::setLevelStars(int level, int stars) {
    std::shared_ptr<JsonValue> starsJson = getLevelJson(level)->get(STARS_KEY);
    starsJson->set((double)stars);
    saveSettings();
}

/** Get the level saved number of moves */
int GameData::getLevelMoves(int level) {
    std::shared_ptr<JsonValue> movesJson = getLevelJson(level)->get(MOVES_KEY);
    return movesJson->asInt();
}

/** Set the level saved number of moves */
void GameData::setLevelMoves(int level, int moves) {
    std::shared_ptr<JsonValue> movesJson = getLevelJson(level)->get(MOVES_KEY);
    movesJson->set((double)moves);
    saveSettings();
}

/** Return which of the 3 realms a level is in */
int GameData::getRealm(int level) {
    auto numLevels = _levelListJson->size();
    float cap0 = numLevels/3.0f;
    float cap1 = numLevels*2.0f/3.0f;
    if (level < cap0) {
        return 0;
    } else if (level < cap1) {
        return 1;
    }
    return 2;
}
