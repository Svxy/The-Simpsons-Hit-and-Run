//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GameConfigManager
//
// Description: Interface for the GameConfigManager class.
//
// Authors:     Ziemek Trzesicki
//
// Revisions		Date			Author	    Revision
//                  2003/05/07      ziemek      Created for SRR2
//
//===========================================================================

#ifndef GAMECONFIGMANAGER_H
#define GAMECONFIGMANAGER_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <data/config/gameconfig.h>
#include <radfile.hpp>

//===========================================================================
// Forward References
//===========================================================================

class ConfigString;

//===========================================================================
// Interface Definitions
//===========================================================================

class GameConfigManager
{
public:
    // Static Methods for accessing this singleton.
    static GameConfigManager* CreateInstance();
    static void DestroyInstance();
    static GameConfigManager* GetInstance();

    static const char* ConfigFilename;
    static const unsigned int MaxGameConfigs = 16;

public:

    // Registration of client game data
    void RegisterConfig( GameConfigHandler* gdHandler );

    // Loads the config file and applies loaded settings to
    // registered config handlers.
    bool LoadConfigFile();

    // Prompts for config data from registered config handlers and
    // saves it to the config file.
    bool SaveConfigFile();

private:
    // Can only be created by CreateInstance().
    GameConfigManager();
    ~GameConfigManager();

    // No copying or assignment.
    GameConfigManager( const GameConfigManager& );
    GameConfigManager& operator= ( const GameConfigManager& );

    // Loads all config data from the config string.
    void LoadFromConfig( ConfigString& config );
    // Saves all config data to the config string.
    void SaveToConfig( ConfigString& config );

private:
    // Pointer to the one and only instance of this singleton.
    static GameConfigManager* spInstance;

    GameConfigHandler* mConfig[ MaxGameConfigs ];
    int mNumConfig;
    int mConfigSize;
};

// A little syntactic sugar for getting at this singleton.
inline GameConfigManager* GetGameConfigManager() { return( GameConfigManager::GetInstance() ); }

#endif // GAMECONFIGMANAGER_H
