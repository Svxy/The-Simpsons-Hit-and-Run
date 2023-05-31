//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GameConfigManager
//
// Description: Implementation for the GameConfigManager class.
//
// Authors:     Ziemek Trzesicki
//
// Revisions		Date			Author	    Revision
//                  2003/05/07      ziemek      Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <data/config/gameconfigmanager.h>
#include <data/config/configstring.h>
#include <data/memcard/memorycardmanager.h>

#include <memory/srrmemory.h>

//===========================================================================
// Static Member Initializations
//===========================================================================

GameConfigManager* GameConfigManager::spInstance = NULL;

const char* GameConfigManager::ConfigFilename = "simpsons.ini";

//===========================================================================
// Static Public Member Functions
//===========================================================================

GameConfigManager* GameConfigManager::CreateInstance()
{
    MEMTRACK_PUSH_GROUP( "GameConfigManager" );
    spInstance = new GameConfigManager;
    rAssert( spInstance != NULL );
    MEMTRACK_POP_GROUP( "GameConfigManager" );

    return spInstance;
}

void GameConfigManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}

GameConfigManager* GameConfigManager::GetInstance()
{
    rAssert( spInstance != NULL );
    return spInstance;
}

//===========================================================================
// Public Member Functions
//===========================================================================

GameConfigManager::GameConfigManager()
:   mNumConfig( 0 ),
    mConfigSize( 0 )
{
    for( int i = 0; i < MaxGameConfigs; i++ )
    {
        mConfig[ i ] = NULL;
    }
}

GameConfigManager::~GameConfigManager() 
{}

void GameConfigManager::RegisterConfig( GameConfigHandler* config )
{
    // validate the config
    rAssert( config != NULL );
    rAssert( strlen( config->GetConfigName() ) > 0 );
    rAssert( config->GetNumProperties() > 0 );

    // make sure we have room
    rAssertMsg( mNumConfig < MaxGameConfigs,
                "ERROR: *** Exceeded maximum number of registered Game Config!" );

    // add to registered list of Game Config
    mConfig[ mNumConfig++ ] = config;

    // update Game Config size (extra 1 cause of section name)
    mConfigSize += ( config->GetNumProperties() + 1 ) * ConfigString::MaxLength;
}

bool GameConfigManager::LoadConfigFile()
{
    // Open the config file for reading.
    IRadFile* radFile = NULL; 
    radFileOpenSync( &radFile,
                     ConfigFilename,
                     false,
                     OpenExisting );
    rAssert( radFile );

    bool success = false;

    // If opened, read the config file.
    if( radFile->IsOpen() && radFile->GetSize() > 0 )
    {
        unsigned size = radFile->GetSize();

        // Make sure we're not swamped with illegally huge files.
        if( (int) size > 2 * mConfigSize )
        {
            size = 2 * mConfigSize;
        }

        ConfigString config( ConfigString::Read, size );

        radFile->ReadSync( config.GetBuffer(), size );

        LoadFromConfig( config );

        success = true;
    }

    radFile->Release();
    return success;
}

bool GameConfigManager::SaveConfigFile()
{
    // Save the configuration to a config string.
    ConfigString config( ConfigString::Write, mConfigSize );
    SaveToConfig( config );

    // Open a file for saving.
    IRadFile* radFile = NULL;
    radFileOpenSync( &radFile,
                     ConfigFilename,
                     true,
                     CreateAlways );

    rAssert( radFile );

    bool success = false;

    // Save the configuration.
    if( radFile->IsOpen() )
    {
        radFile->WriteSync( config.GetBuffer(),
                            strlen( config.GetBuffer() ) );

        radFile->CommitSync();

        success = true;
    }
    
    radFile->Release();
    return success;
}

//===========================================================================
// Private Member Functions
//===========================================================================

void GameConfigManager::LoadFromConfig( ConfigString& config )
{
    rAssert( config.GetMode() == ConfigString::Read );

    char section[ ConfigString::MaxLength ];
    int i;

    while( config.ReadSection( section ) )
    {
        // Find the config handler for the section.
        for( i = 0; i < mNumConfig; i++ )
        {
            if( strcmp( section, mConfig[i]->GetConfigName() ) == 0 )
            {
                break;
            }
        }

        // Check for unknown sections.
        if( i == mNumConfig )
        {
            continue;
        }

        // Get the config handler to read the section
        mConfig[i]->LoadConfig( config );
    }
}

void GameConfigManager::SaveToConfig( ConfigString& config )
{
    rAssert( config.GetMode() == ConfigString::Write );

    for( int i = 0; i < mNumConfig; i++ )
    {
        config.WriteSection( mConfig[i]->GetConfigName() );

        mConfig[i]->SaveConfig( config );
    }
}
