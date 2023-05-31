//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GameConfig
//
// Description: GameConfig Interface and Constants.
//
// Authors:     Ziemek Trzesicki
//
// Revisions		Date			Author	    Revision
//                  2003/05/02      ziemek      Created for SRR2
//
//===========================================================================

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <data/config/configstring.h>

struct GameConfigHandler
{
    // Returns the name/title of the configuration data.
    // For example - "InputControls".
    virtual const char* GetConfigName() const = 0;

    // Returns the number of properties stored by this config handler.
    virtual int GetNumProperties() const = 0;

    // Called to load the default settings.
    // Used in case that no configuration exists in the file for
    // this handler, or simply to revert the config to the default.
    virtual void LoadDefaults() = 0;

    // Called to load configuration data from the config string.
    virtual void LoadConfig( ConfigString& config ) = 0;

    // Called to save configuration data to the config string.
    virtual void SaveConfig( ConfigString& config ) = 0;
};

#endif // GAMECONFIG_H
