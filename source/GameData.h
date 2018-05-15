//
//  GameData.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 5/15/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef GameData_h
#define GameData_h

#define INIT_SETTINGS_PATH "json/settings.json"
#define SETTINGS_FILENAME "settings.json"
#define MUTE_KEY "mute"

#include <cugl/cugl.h>

class GameData {
private:
    /** Reference to GameData singleton */
    static GameData* _gData;
    
    /** Save Directory */
    std::string _saveDir;
    
    /** Settings file path */
    std::string _settingsPath;
    
    /** Reference to Settings JsonValue object */
    std::shared_ptr<cugl::JsonValue> _settingsJson;
    
#pragma mark -
#pragma mark Constructors
    /** Constructor */
    GameData();
    
    /** Destructor */
    ~GameData() { dispose(); }
    
    /** Dispose of all (non-static) resources */
    void dispose();
    
    /** Initialize GameData */
    bool init(std::string saveDir);
    
#pragma mark -
#pragma mark Helper Methods
    /** Initialize the settings JSON file from INIT_SETTINGS_PATH */
    void initSettings();
    
public:
#pragma mark -
#pragma mark Static Accessors
    /**
     * Returns the singleton instance of the game data.
     *
     * If the game data has not been started, then this method will return
     * nullptr.
     *
     * @return the singleton instance of the game data.
     */
    static GameData* get() { return _gData; }
    
    /** Start the game data */
    static void start(std::string saveDir);
    
    /**
     * Stops the singleton game data, releasing all resources.
     *
     * Once this method is called, the method get() will return nullptr.
     * Calling the method multiple times (without calling stop) will have
     * no effect.
     */
    static void stop();
    
#pragma mark -
#pragma mark Non-Static Accessors/Mutators
    /** Get the saved mute setting */
    bool getMuteSetting();
    
    /** Set the saved mute setting */
    void setMuteSetting(bool mute);
};

#endif /* GameData_h */
