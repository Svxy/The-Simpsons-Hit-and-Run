//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundloader.cpp
//
// Description: Implement SoundLoader class, which makes sure that sounds
//              required in the game are allocated and resident in sound memory
//
// History:     26/06/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <stdio.h>

#include <sound/soundloader.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundresourcemanager.h>

#include <memory/srrmemory.h>
#include <loading/loadingmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <constants/vehicleenum.h>
#include <mission/gameplaymanager.h>
#include <events/eventmanager.h>
#include <worldsim/character/character.h>

#include <radscript.hpp>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Names of sound clusters.  Used so that the loading manager can tell us
// when it's our turn to loading something.  Should correspond to SoundClusterName
// enumeration
//
static const char* s_clusterNames[] = 
{
    "permanent",
    "frontend",
    "ingame",
    "suburbs",
    "downtown",
    "seaside",
    "level1",
    "level2",
    "level3",
    "level4",
    "level5",
    "level6",
    "level7",
    "minigame",
    "huh?",
    "apu",
    "bart",
    "homer",
    "lisa",
    "marge",
    "bart_v",
    "apu_v",
    "snake_v",
    "homer_v",
    "famil_v",
    "gramp_v",
    "cletu_v",
    "wiggu_v",
    "empty1",
    "marge_v",
    "empty2",
    "empty3",
    "smith_v",
    "empty4",
    "empty5",
    "empty6",
    "zombi_v",
    "empty7",
    "empty8",
    "cVan",
    "compactA",
    "comic_v",
    "skinn_v",
    "cCola",
    "cSedan",
    "cPolice",
    "cCellA",
    "cCellB",
    "cCellC",
    "cCellD",
    "minivanA_v",
    "pickupA",
    "taxiA_v",
    "sportsA",
    "sportsB",
    "SUVA",
    "wagonA",
    "hbike_v",
    "burns_v",
    "honor_v",
    "cArmor",
    "cCurator",
    "cHears",
    "cKlimo",
    "cLimo",
    "cNerd",
    "frink_v",
    "cMilk",
    "cDonut",
    "bbman_v",
    "bookb_v",
    "carhom_v",
    "elect_v",
    "fone_v",
    "gramR_v",
    "moe_v",
    "mrplo_v",
    "otto_v",
    "plowk_v",
    "scorp_v",
    "willi_v",
    "sedanA",
    "sedanB",
    "cBlbart",
    "cCube",
    "cDuff",
    "cNonup",
    "lisa_v",
    "krust_v",
    "coffin",
    "hallo",
    "ship",
    "witchcar",
    "huska",
    "atv_v",
    "dune_v",
    "hype_v",
    "knigh_v",
    "mono_v",
    "oblit_v",
    "rocke_v",
    "ambul",
    "burnsarm",
    "fishtruc",
    "garbage",
    "icecream",
    "istruck",
    "nuctruck",
    "pizza",
    "schoolbu",
    "votetruc",
    "glastruc",
    "cfire_v",
    "cBone",
    "redbrick"
};

static const int NumScriptNames = sizeof( s_clusterNames ) / sizeof( const char* );

//
// Indices of character namespaces for each level.
//
// TODO: I don't like these tables, there must be a more data-driven way to do
//       this.
//       0 == Apu
//       1 == Bart
//       2 == Homer
//       3 == Lisa
//       4 == Marge
//
static unsigned int s_charNamespaceIndices[] = { 2, 1, 3, 4, 0, 1, 2, 0 };

static VehicleEnum::VehicleID s_carIndices[] = { VehicleEnum::FAMIL_V,
                                                 VehicleEnum::HONOR_V,
                                                 VehicleEnum::LISA_V,
                                                 VehicleEnum::MARGE_V,
                                                 VehicleEnum::APU_V,
                                                 VehicleEnum::BART_V,
                                                 VehicleEnum::HOMER_V,
                                                 VehicleEnum::FAMIL_V,
};

static unsigned int s_levelIndices[] = { 0, 1, 2, 0, 1, 2, 0, 0 };

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundLoader::SoundLoader
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundLoader::SoundLoader() :
    m_currentCluster( SC_ALWAYS_LOADED )
{
    unsigned int i;
    SoundClusterName clusterIndex;
    Sound::daSoundRenderingManager* renderingMgr = Sound::daSoundRenderingManagerGet();

    for( i = 0; i < SC_MAX_CLUSTERS; i++ )
    {
        m_clusterList[i] = NULL;
    }

    for( clusterIndex = SC_ALWAYS_LOADED;
         clusterIndex < SC_CHAR_APU; 
         clusterIndex = static_cast<SoundClusterName>( clusterIndex + 1 ) )
    {
        m_clusterList[clusterIndex] = 
            new(GMA_PERSISTENT) SoundCluster( clusterIndex,
                                              renderingMgr->GetSoundNamespace() );
    }

    for( clusterIndex = SC_CHAR_APU;
         clusterIndex < SC_CAR_BASE;
         clusterIndex = static_cast<SoundClusterName>( clusterIndex + 1 ) )
    {
        m_clusterList[clusterIndex] = 
            new(GMA_PERSISTENT) SoundCluster( clusterIndex,
                                              renderingMgr->GetCharacterNamespace( clusterIndex - SC_CHAR_APU ) );
    }

    for( clusterIndex = SC_CAR_BASE;
        clusterIndex < SC_MAX_CLUSTERS;
        clusterIndex = static_cast<SoundClusterName>( clusterIndex + 1 ) )
    {
        m_clusterList[clusterIndex] = 
            new(GMA_PERSISTENT) SoundCluster( clusterIndex,
                                              renderingMgr->GetSoundNamespace() );
    }

    //
    // Register event listeners
    //
    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_START );
}

//==============================================================================
// SoundLoader::~SoundLoader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundLoader::~SoundLoader()
{
    unsigned int i;

    for( i = 0; i < SC_MAX_CLUSTERS; i++ )
    {
        if( m_clusterList[i] != NULL )
        {
            m_clusterList[i]->Release();
        }
    }

    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// SoundLoader::LevelLoad
//=============================================================================
// Description: Loads the sound cluster for a particular level
//
// Parameters:  RenderEnums::LevelEnum level -
//                  enumeration indicating the level whose sound cluster 
//                  is to be loaded
//
// Return:      void 
//
//=============================================================================
void SoundLoader::LevelLoad( RenderEnums::LevelEnum level )
{
    IRadNameSpace* charNamespace;
    unsigned int levelNum;

    clusterUnload( SC_FRONTEND );

    queueLoad( SC_INGAME );

    if( GetGameplayManager()->IsSuperSprint() )
    {
        queueLoad( SC_MINIGAME );
    }
    else
    {
        rAssert( level <= RenderEnums::numLevels );
        levelNum = static_cast<unsigned int>(level);

        queueLoad( static_cast<SoundClusterName>( SC_LEVEL_SUBURBS + s_levelIndices[levelNum] ) );
        queueLoad( static_cast<SoundClusterName>( SC_CHAR_APU + s_charNamespaceIndices[levelNum] ) );
        queueLoad( static_cast<SoundClusterName>( SC_CAR_BASE + s_carIndices[levelNum] ) );
        queueLoad( static_cast<SoundClusterName>( SC_LEVEL1 + levelNum ) );

        charNamespace = m_clusterList[SC_CHAR_APU + s_charNamespaceIndices[level]]->GetMyNamespace();
        rAssert( charNamespace != NULL );

        //
        // We need to do this for RadTuner, since we've got duplicate names and
        // it won't necessarily find the correct character otherwise
        //
        charNamespace->MoveToFront();
    }
}

//=============================================================================
// SoundLoader::LevelUnload
//=============================================================================
// Description: Unloads the sound cluster for a particular level
//
// Parameters:  RenderEnums::LevelEnum level -
//                  enumeration indicating the level whose sound cluster 
//                  is to be unloaded
//
// Return:      void 
//
//=============================================================================
void SoundLoader::LevelUnload( bool goingToFe )
{
    SoundClusterName clusterIndex;

    //
    // Unload everything that's not permanent
    //
    for( clusterIndex = SC_INGAME;
         clusterIndex < SC_MAX_CLUSTERS;
         clusterIndex = static_cast<SoundClusterName>( clusterIndex + 1 ) )
    {
        if( m_clusterList[clusterIndex]->IsLoaded() )
        {
            clusterUnload( clusterIndex );
        }
    }

    if( goingToFe )
    {
        queueLoad( SC_FRONTEND );
    }
}

//=============================================================================
// SoundLoader::MissionLoad
//=============================================================================
// Description: Loads the sound cluster for a particular mission
//
// Parameters:  RenderEnums::MissionEnum mission -
//                  enumeration indicating the mission whose sound cluster 
//                  is to be loaded
//
// Return:      void 
//
//=============================================================================
void SoundLoader::MissionLoad( RenderEnums::MissionEnum mission )
{
}

//=============================================================================
// SoundLoader::MissionUnload
//=============================================================================
// Description: Unloads the sound cluster for a particular mission
//
// Parameters:  RenderEnums::MissionEnum mission -
//                  enumeration indicating the mission whose sound cluster 
//                  is to be unloaded
//
// Return:      void 
//
//=============================================================================
void SoundLoader::MissionUnload( RenderEnums::MissionEnum mission )
{
}

void SoundLoader::LoadCarSound( Vehicle* theCar, bool unloadOtherCars )
{
    rAssert( theCar );

    SoundClusterName clusterIndex;
    SoundClusterName newCarCluster = static_cast<SoundClusterName>(SC_CAR_BASE + theCar->mVehicleID);
    bool validCar = ( SC_CAR_BASE + theCar->mVehicleID ) < NumScriptNames;

    rAssertMsg( validCar, "A new vehicle has been added that the sound system does not have a script for.  Tell Esan.\n" );

    //
    // Have we loaded this car already?
    //
    if( !validCar || m_clusterList[newCarCluster]->IsLoaded() )
    {
        return;
    }

    //
    // Unload the existing car sound
    //
    if( unloadOtherCars )
    {
        for( clusterIndex = SC_CAR_BASE;
            clusterIndex < SC_MAX_CLUSTERS;
            clusterIndex = static_cast<SoundClusterName>( clusterIndex + 1 ) )
        {
            if( m_clusterList[clusterIndex]->IsLoaded() )
            {
                clusterUnload( clusterIndex );
            }
        }
    }

    //
    // Load the new car
    //
    queueLoad( newCarCluster );
}

//=============================================================================
// SoundLoader::IsSoundLoaded
//=============================================================================
// Description: Indicate whether a particular sound resource has been loaded
//
// Parameters:  soundKey - hashed name of the sound resource to look for
//
// Return:      true if loaded, falsed otherwise
//
//=============================================================================
bool SoundLoader::IsSoundLoaded( Sound::daResourceKey soundKey )
{
    unsigned int i;

    for( i = 0; i < SC_MAX_CLUSTERS; i++ )
    {
        if( ( m_clusterList[i] != NULL ) && ( m_clusterList[i]->ContainsResource( soundKey ) ) )
        {
            return( m_clusterList[i]->IsLoaded() );
        }
    }

    return( false );
}

//=============================================================================
// SoundLoader::LoadClusterByName
//=============================================================================
// Description: Given a cluster name from the loading manager, load the
//              desired cluster
//
// Parameters:  clusterName - text name for the cluster
//              callbackObj - loading file handler to notify on completion
//
// Return:      true if cluster already loaded, false otherwise 
//
//=============================================================================
bool SoundLoader::LoadClusterByName( const char* clusterName, SoundFileHandler* callbackObj )
{
    const char* shortName;
    unsigned int i;

    // Strip out the "sound:" prefix
    rAssert( strlen( clusterName ) > 6 );
    shortName = &(clusterName[6]);

    //
    // Find the matching cluster name
    //
    for( i = 0; i < SC_MAX_CLUSTERS; i++ )
    {
        if( strcmp( shortName, s_clusterNames[i] ) == 0 )
        {
            return( clusterLoad( static_cast<SoundClusterName>( i ), callbackObj ) );
        }
    }

    //
    // If we get here, cluster not found
    //
    rAssert( false );
    return( false );
}

//=============================================================================
// SoundLoader::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void SoundLoader::HandleEvent( EventEnum id, void* pEventData )
{
    Character* theCharacter;
    Vehicle* theCar;

    switch( id )
    {
        case EVENT_GETINTOVEHICLE_START:
            //
            // Make sure we've got the correct car sound for this vehicle
            //
            theCharacter = static_cast<Character*>(pEventData);
            rAssert( theCharacter != NULL );
            theCar = theCharacter->GetTargetVehicle();
            rAssert( theCar != NULL );

            LoadCarSound( theCar, true );
            break;

        default:
            rAssert( false );
            break;
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SoundLoader::queueLoad
//=============================================================================
// Description: Queue a cluster load with the loading manager
//
// Parameters:  cluster - name of cluster to queue
//
// Return:      void 
//
//=============================================================================
void SoundLoader::queueLoad( SoundClusterName cluster )
{
    char fakeFilename[50];
    
    //
    // Create a pseudo filename that we'll give to the loading manager.
    // Content doesn't really matter, we'll just throw it out when the
    // loading manager passes it back
    //
    if( cluster >= NumScriptNames )
    {
        //
        // Just load Bart for now
        //
        cluster = SC_CAR_BASE;
    }

    sprintf( fakeFilename, "sound:%s", s_clusterNames[cluster] );
    GetLoadingManager()->AddRequest( FILEHANDLER_SOUND, fakeFilename, GMA_LEVEL_AUDIO );
}

//=============================================================================
// SoundLoader::clusterLoad
//=============================================================================
// Description: Find the specified sound cluster and direct it to load sounds
//
// Parameters:  SoundClusterName name - specifies which cluster to load
//
// Return:      false if cluster not yet loaded, true otherwise
//
//=============================================================================
bool SoundLoader::clusterLoad( SoundClusterName name, SoundFileHandler* callbackObj )
{
    bool loaded;

    rAssert( name < SC_MAX_CLUSTERS );
    
    loaded = m_clusterList[name]->IsLoaded();
    if( !loaded )
    {
        m_clusterList[name]->LoadSounds( callbackObj );
    }

    return( loaded );
}

void SoundLoader::clusterUnload( SoundClusterName name )
{
    rAssert( name < SC_MAX_CLUSTERS );
    if( m_clusterList[name]->IsLoaded() )
    {
        m_clusterList[name]->UnloadSounds();
    }
}