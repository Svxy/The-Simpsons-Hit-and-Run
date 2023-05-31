//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundmanager.cpp
//
// Description: Manager interface that the other game components use to
//              interact with sound.
//
// History:     01/06/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radfactory.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundmanager.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/soundnucleus.hpp>

#include <sound/music/musicplayer.h>
#include <sound/dialog/dialogcoordinator.h>
#include <sound/dialog/dialogline.h>
#include <sound/tuning/globalsettings.h>
#include <sound/soundfx/soundeffectplayer.h>
#include <sound/soundfx/reverbsettings.h>
#include <sound/soundfx/positionalsoundsettings.h>
#include <sound/movingpositional/movingsoundmanager.h>
#include <sound/sounddebug/sounddebugdisplay.h>

#include <loading/loadingmanager.h>
#include <loading/soundfilehandler.h>

#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <events/eventmanager.h>
#include <data/gamedatamanager.h>
#include <interiors/interiormanager.h>
#include <gameflow/gameflow.h>
#include <worldsim/avatarmanager.h>

#include <string.h>

#ifdef RAD_GAMECUBE
#include <dolphin/os.h>
#endif

#ifdef RAD_WIN32
#include <data/config/gameconfigmanager.h>
#endif

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
SoundManager* SoundManager::spInstance = NULL;

//
// Sound file extensions
//
const char* RADSCRIPT_TYPE_INFO_FILE = "typ";
const char* RADSCRIPT_SCRIPT_FILE = "spt";
const char* RADMUSIC_SCRIPT_FILE = "rms";

//
// Sound mode flags, necessary because it's too late in the project
// to turn the boolean used to save the sound mode into an enumeration.
// Doh.
//
static const int SOUND_MODE_STEREO_FLAG = 1;
static const int SOUND_MODE_SURROUND_FLAG = 1 << 1;

static unsigned int gLastServiceTime;
static unsigned int gLastServiceOncePerFrameTime;

static const unsigned int BAD_SERVICE_TIME = 100;
//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundManager::CreateInstance
//==============================================================================
//
// Description: Creates the SoundManager.
//
// Parameters:	None.
//
// Return:      Pointer to the SoundManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
SoundManager* SoundManager::CreateInstance( bool muteSound, bool noMusic, 
                                            bool noEffects, bool noDialogue )
{
    MEMTRACK_PUSH_GROUP( "Sound" );
    
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) SoundManager( muteSound, noMusic, noEffects, noDialogue );
    rAssert( spInstance );

    spInstance->initialize();

    MEMTRACK_POP_GROUP( "Sound" );

    return spInstance;
}

//==============================================================================
// SoundManager::GetInstance
//==============================================================================
//
// Description: - Access point for the SoundManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the SoundManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
SoundManager* SoundManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// SoundManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the SoundManager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void SoundManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}

//==============================================================================
// SoundManager::Update
//==============================================================================
//
// Description: Update the sound renderer.  This should be done as frequently
//              as possible.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void SoundManager::Update()
{
    unsigned int now = radTimeGetMilliseconds( );
    unsigned int dif = now - gLastServiceTime;
    
    if ( dif > BAD_SERVICE_TIME )
    {
        #ifndef RAD_DEBUG
            rReleasePrintf( "\nAUDIO: Detected Service Lag:[%d]ms -- this could cause skipping\n\n", dif );
        #endif
    }
    
    gLastServiceTime = now;

    if( m_isMuted )
    {
        return;
    }

    m_musicPlayer->Service();

    rAssert( m_pSoundRenderMgr != NULL );
    m_pSoundRenderMgr->Service();
}

//==============================================================================
// SoundManager::UpdateOncePerFrame
//==============================================================================
//
// Description: Update the sound renderer's expensive, can-do-once-per-frame
//              stuff (e.g. positional sound info).
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void SoundManager::UpdateOncePerFrame( unsigned int elapsedTime, ContextEnum context, bool useContext, bool isPausedForErrors )
{

    unsigned int now = radTimeGetMilliseconds( );
    unsigned int dif = now - gLastServiceOncePerFrameTime;
    
    if ( dif > BAD_SERVICE_TIME )
    {
        #ifndef RAD_DEBUG
            rReleasePrintf( "\nAUDIO: Detected ServiceOpf Lag:[%d]ms -- this could cause skipping\n\n", dif );
        #endif
    }
    
    gLastServiceOncePerFrameTime = now;
    
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_pSoundRenderMgr != NULL );
    m_pSoundRenderMgr->ServiceOncePerFrame( elapsedTime );
    m_soundFXPlayer->ServiceOncePerFrame( elapsedTime );
    m_movingSoundManager->ServiceOncePerFrame();

    if( !isPausedForErrors )
    {
        //
        // If we've paused the players, don't update this thing because it could
        // start up new sounds
        //
        m_avatarSoundPlayer.UpdateOncePerFrame( elapsedTime );
    }

    //
    // No point in updating listener position more than once per frame
    //
    if( useContext )
    {
        m_listener.Update( context );
    }

    //
    // The dialog queue timer list isn't that time-sensitive, once per frame
    // should be fine
    //
    if( m_dialogCoordinator != NULL )
    {
        m_dialogCoordinator->ServiceOncePerFrame();
    }
}

//=============================================================================
// SoundManager::HandleEvent
//=============================================================================
// Description: Handle any events that the underlying systems won't do
//              during mute.
//
// Parameters:  id - ID of event to be handled
//              pEventData - user data for event
//
// Return:      void 
//
//=============================================================================
void SoundManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_CONVERSATION_SKIP:
            //
            // We only get this in mute mode.  Nothing to stop, signal that we're done
            //
            GetEventManager()->TriggerEvent( EVENT_CONVERSATION_DONE );
            break;
        case EVENT_CONVERSATION_START:
            m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_LETTERBOX, NULL, false );
            break;
        case EVENT_CONVERSATION_DONE:
            m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_LETTERBOX, NULL, true );
            break;
        case EVENT_GETINTOVEHICLE_END:
            m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_ONFOOT, NULL, true );
            break;
        case EVENT_GETOUTOFVEHICLE_END:
            m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_ONFOOT, NULL, false );
            break;
        case EVENT_ENTER_INTERIOR_START:
        case EVENT_EXIT_INTERIOR_START:
            m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_JUST_MUSIC, NULL, false );
            break;
        case EVENT_ENTER_INTERIOR_END:
        case EVENT_EXIT_INTERIOR_END:
            m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_JUST_MUSIC, NULL, true );
            break;

        case EVENT_MISSION_RESET:
            //
            // D'oh!  Problem: if you fail a mission just as you're trying to go into an interior, you never
            // get the enter/exit_interior_end event.  However, we will be getting a mission reset in this
            // case, so bring up the volume just in case.
            //
            m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_JUST_MUSIC, NULL, true );

            //
            // Fall through to case below
            //

        case EVENT_CHARACTER_POS_RESET:
        case EVENT_VEHICLE_DESTROYED_SYNC_SOUND:
            if( GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar() )
            {
                m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_ONFOOT, NULL, true );
            }
            else
            {
                m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_ONFOOT, NULL, false );
            }
            break;

        case EVENT_FE_MENU_SELECT:
            playStartupAcceptSound();
            break;

        case EVENT_FE_MENU_UPORDOWN:
            playStartupScrollSound();
            break;

        default:
            rAssertMsg( false, "Huh?  SoundManager getting events it shouldn't\n" );
            break;
    }
}

//=============================================================================
// SoundManager::OnBootupStart
//=============================================================================
// Description: Called when bootup context started
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnBootupStart()
{
    if( m_isMuted )
    {
        return;
    }

    //
    // Load the RadScript and RadMusic files
    //
    m_pSoundRenderMgr->QueueCementFileRegistration();
    m_musicPlayer->QueueRadmusicScriptLoad();
    m_pSoundRenderMgr->QueueRadscriptFileLoads();

    //
    // Load front end sounds
    //
    m_soundLoader->LoadFrontEnd();
}

//=============================================================================
// SoundManager::OnBootupComplete
//=============================================================================
// Description: Initialize the dialog system once the scripts are known to
//              have been loaded
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnBootupComplete()
{
    if( m_isMuted )
    {
        return;
    }

    dumpStartupSounds();

    // Set up the tuner
    m_pSoundRenderMgr->GetTuner()->PostScriptLoadInitialize();

    //
    // If the sound levels haven't been auto-loaded, set them now
    //
    if( !( GetGameDataManager()->IsGameLoaded() ) )
    {
        ResetData();
    }
    else
    {
        //
        // Hack!  If we've loaded sound volumes before we had the scripts loaded,
        // then we wouldn't have been able to calculate the ambience volumes properly.
        // Fix those up now.
        //
        SetAmbienceVolume( GetCalculatedAmbienceVolume() );
    }
}

//=============================================================================
// SoundManager::QueueLevelSoundLoads
//=============================================================================
// Description: Prepare to load level-related sound through the loading manager
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::QueueLevelSoundLoads()
{
    RenderEnums::LevelEnum level;

    if( m_isMuted )
    {
        return;
    }

    level = GetGameplayManager()->GetCurrentLevelIndex();
    m_soundLoader->LevelLoad( level );
    m_musicPlayer->QueueMusicLevelLoad( level );
}

//=============================================================================
// SoundManager::ResetDucking
//=============================================================================
// Description: Bring all the volume levels back up
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::ResetDucking()
{
    if( m_isMuted )
    {
        return;
    }
    m_pSoundRenderMgr->GetTuner()->ResetDuck();
}

//=============================================================================
// SoundManager::OnFrontEndStart
//=============================================================================
// Description: To be called when front end starts, so we can do initialization
//              stuff
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnFrontEndStart()
{
    if( m_isMuted )
    {
        return;
    }

    //
    // Start loading the permanent, always-in-memory stuff
    //
    m_soundLoader->LoadPermanentSounds();

    m_musicPlayer->OnFrontEndStart();
    m_soundFXPlayer->OnFrontEndStart();
}

//=============================================================================
// SoundManager::OnFrontEndEnd
//=============================================================================
// Description: To be called when front end ends, so we can do destruction
//              stuff
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnFrontEndEnd()
{
    if( m_isMuted )
    {
        return;
    }

    m_musicPlayer->OnFrontEndFinish();
}

//=============================================================================
// SoundManager::OnGameplayStart
//=============================================================================
// Description: To be called when gameplay starts, so we can do initialization
//              stuff
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnGameplayStart()
{
    bool playerInCar;

    if( m_isMuted )
    {
        return;
    }

    //
    // Pass on start notice
    //
    playerInCar = m_avatarSoundPlayer.OnBeginGameplay();
    m_musicPlayer->OnGameplayStart( playerInCar );
    m_soundFXPlayer->OnGameplayStart();

    //
    // Assume we're starting gameplay on foot, except for minigame.  Start the ducking
    //
    if( GetGameplayManager()->IsSuperSprint() )
    {
        m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_MINIGAME, NULL, false );
    }
    else
    {
        m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_ONFOOT, NULL, false );
    }
}

//=============================================================================
// SoundManager::OnGameplayEnd
//=============================================================================
// Description: To be called when gameplay ends, so we can do destruction
//              stuff
//
// Parameters:  goingToFE - indicates whether we're exiting gameplay to go
//                          to the front end (as opposed to loading a different
//                          level)
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnGameplayEnd( bool goingToFE )
{
    if( m_isMuted )
    {
        return;
    }

    m_avatarSoundPlayer.OnEndGameplay();
    m_musicPlayer->OnGameplayFinish();
    m_dialogCoordinator->OnGameplayEnd();
    m_soundFXPlayer->OnGameplayEnd();

    m_soundLoader->LevelUnload( goingToFE );
    m_musicPlayer->UnloadRadmusicScript();

    //
    // Set up for the front end
    //
    if( goingToFE )
    {
        m_musicPlayer->QueueMusicLevelLoad( RenderEnums::L3 );
    }
}

//=============================================================================
// SoundManager::OnPauseStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnPauseStart()
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_PAUSE, NULL, false );

    m_musicPlayer->OnPauseStart();
    m_dialogCoordinator->OnPauseStart();
    m_soundFXPlayer->OnPauseStart();
    m_NISPlayer->PauseAllNISPlayers();

    GetEventManager()->TriggerEvent( EVENT_FE_PAUSE_MENU_START );
}

//=============================================================================
// SoundManager::OnPauseEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnPauseEnd()
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_PAUSE, NULL, true );

    m_musicPlayer->OnPauseEnd();
    m_dialogCoordinator->OnPauseEnd();
    m_soundFXPlayer->OnPauseEnd();
    m_NISPlayer->ContinueAllNISPlayers();

    GetEventManager()->TriggerEvent( EVENT_FE_PAUSE_MENU_END );
}

//=============================================================================
// SoundManager::OnStoreScreenStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnStoreScreenStart( bool playMusic )
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_STORE, NULL, false );

    if( playMusic )
    {
        m_musicPlayer->OnStoreStart();
    }
}

//=============================================================================
// SoundManager::OnStoreScreenEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnStoreScreenEnd()
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_STORE, NULL, true );

    m_musicPlayer->OnStoreEnd();
}

//=============================================================================
// SoundManager::DuckEverythingButMusicBegin
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::DuckEverythingButMusicBegin( bool playMuzak )
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_JUST_MUSIC, NULL, false );

    if( playMuzak )
    {
        m_musicPlayer->OnStoreStart();
    }
}

//=============================================================================
// SoundManager::DuckEverythingButMusicEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::DuckEverythingButMusicEnd( bool playMuzak )
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_JUST_MUSIC, NULL, true );

    if( playMuzak )
    {
        m_musicPlayer->OnStoreEnd();
    }
}

//=============================================================================
// SoundManager::OnMissionBriefingStart
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnMissionBriefingStart()
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_MISSION, NULL, false );
    m_NISPlayer->PauseAllNISPlayers();
}

//=============================================================================
// SoundManager::OnMissionBriefingEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::OnMissionBriefingEnd()
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_MISSION, NULL, true );
    m_NISPlayer->ContinueAllNISPlayers();
}

//=============================================================================
// SoundManager::DuckForInGameCredits
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::DuckForInGameCredits()
{
    if( m_isMuted )
    {
        return;
    }

    m_pSoundRenderMgr->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_CREDITS, NULL, false );
}

//=============================================================================
// SoundManager::LoadSoundFile
//=============================================================================
// Description: Pass on the file loading directive to the appropriate subsystem,
//              with callback object for the sake of the loading manager
//
// Parameters:  filename - name of file to load
//              callbackObj - callback into loading system when complete
//
// Return:      void 
//
//=============================================================================
void SoundManager::LoadSoundFile( const char* filename, SoundFileHandler* callbackObj )
{
    char fileExtension[4];
    int length = strlen( filename );

    //
    // Determine the appropriate sound subsystem by the file extension
    //
    rAssert( length > 4 );
    if( filename[length - 4] == '.' )
    {
        strcpy( fileExtension, &(filename[strlen(filename) - 3]) );

        if( strcmp( fileExtension, RADSCRIPT_TYPE_INFO_FILE ) == 0 )
        {
            m_pSoundRenderMgr->LoadTypeInfoFile( filename, callbackObj );
        }
        else if( strcmp( fileExtension, RADSCRIPT_SCRIPT_FILE ) == 0 )
        {
            m_pSoundRenderMgr->LoadScriptFile( filename, callbackObj );
        }
        else if( strcmp( fileExtension, RADMUSIC_SCRIPT_FILE ) == 0 )
        {
            m_musicPlayer->LoadRadmusicScript( filename, callbackObj );
        }
        else
        {
            //
            // Oops, don't recognize that type
            //
            rAssert( false );
        }
    }
    else
    {
        //
        // No file extension, must be a sound cluster name
        //
        if( m_soundLoader->LoadClusterByName( filename, callbackObj ) )
        {
            //
            // LoadClusterByName indicated that the cluster was already loaded,
            // so call the callback
            //
            callbackObj->LoadCompleted();
        }
    }
}

//=============================================================================
// SoundManager::LoadCarSound
//=============================================================================
// Description: Load car sound.  Duh.
//
// Parameters:  theCar - vehicle whose sound is to be loaded
//
// Return:      void 
//
//=============================================================================
void SoundManager::LoadCarSound( Vehicle* theCar, bool unloadOtherCars )
{
    if( m_isMuted )
    {
        return;
    }

    m_soundLoader->LoadCarSound( theCar, unloadOtherCars );
}

//=============================================================================
// SoundManager::GetMasterVolume
//=============================================================================
// Description: Get master volume
//
// Parameters:  None
//
// Return:      Volume setting from 0.0f to 1.0f 
//
//=============================================================================
float SoundManager::GetMasterVolume()
{
    if( m_isMuted )
    {
        return( 0.0f );
    }

    rAssert( m_pSoundRenderMgr != NULL );

    return( m_pSoundRenderMgr->GetTuner()->GetMasterVolume() );
}

//=============================================================================
// SoundManager::SetMasterVolume
//=============================================================================
// Description: Set master volume
//
// Parameters:  volume - new master volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetMasterVolume( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_pSoundRenderMgr != NULL );

    m_pSoundRenderMgr->GetTuner()->SetMasterVolume( volume );

    //
    // Stinky special cases.  This must be fixed.
    //
    /*if ( m_musicPlayer )
    {
        m_musicPlayer->SetVolume( volume * m_pSoundRenderMgr->GetTuner()->GetMusicVolume() );
        m_musicPlayer->SetAmbienceVolume( volume * m_pSoundRenderMgr->GetTuner()->GetAmbienceVolume() );
    }*/
}

//=============================================================================
// SoundManager::GetSfxVolume
//=============================================================================
// Description: Get sound effect volume
//
// Parameters:  None
//
// Return:      Volume setting from 0.0f to 1.0f 
//
//=============================================================================
float SoundManager::GetSfxVolume()
{
    if( m_isMuted )
    {
        return( 0.0f );
    }

    rAssert( m_pSoundRenderMgr != NULL );

    return( m_pSoundRenderMgr->GetTuner()->GetSfxVolume() );
}

//=============================================================================
// SoundManager::SetSfxVolume
//=============================================================================
// Description: Set sound effect volume
//
// Parameters:  volume - new sound effect volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetSfxVolume( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_pSoundRenderMgr != NULL );

    m_pSoundRenderMgr->GetTuner()->SetSfxVolume( volume );
}

//=============================================================================
// SoundManager::GetCarVolume
//=============================================================================
// Description: Get sound effect volume
//
// Parameters:  None
//
// Return:      Volume setting from 0.0f to 1.0f 
//
//=============================================================================
float SoundManager::GetCarVolume()
{
    if( m_isMuted )
    {
        return( 0.0f );
    }

    rAssert( m_pSoundRenderMgr != NULL );

    return( m_pSoundRenderMgr->GetTuner()->GetCarVolume() );
}

//=============================================================================
// SoundManager::SetCarVolume
//=============================================================================
// Description: Set sound effect volume
//
// Parameters:  volume - new sound effect volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetCarVolume( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_pSoundRenderMgr != NULL );

    m_pSoundRenderMgr->GetTuner()->SetCarVolume( volume );
}

//=============================================================================
// SoundManager::GetMusicVolume
//=============================================================================
// Description: Get music volume
//
// Parameters:  None
//
// Return:      Volume setting from 0.0f to 1.0f 
//
//=============================================================================
float SoundManager::GetMusicVolume()
{
    if( m_isMuted )
    {
        return( 0.0f );
    }

    rAssert( m_pSoundRenderMgr != NULL );

    //rAssert( m_musicPlayer->GetVolume() == m_pSoundRenderMgr->GetTuner()->GetMusicVolume() );

    return( m_pSoundRenderMgr->GetTuner()->GetMusicVolume() );
}

//=============================================================================
// SoundManager::SetMusicVolume
//=============================================================================
// Description: Set music volume
//
// Parameters:  volume - new music volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetMusicVolume( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_pSoundRenderMgr != NULL );

    m_pSoundRenderMgr->GetTuner()->SetMusicVolume( volume );
    
    //
    // Sadly, we have to deal with the music player outside of the tuner.  The
    // daSound layer doesn't do our interactive music, so volume is controlled
    // separately.
    //
    //rAssert( m_musicPlayer != NULL );
    //m_musicPlayer->SetVolume( volume * m_pSoundRenderMgr->GetTuner()->GetMasterVolume() );
}

//=============================================================================
// SoundManager::SetMusicVolumeWithoutTuner
//=============================================================================
// Description: Set music volume without affecting the tuner's internally
//              stored volume setting
//
// Parameters:  volume - new music volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetMusicVolumeWithoutTuner( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_musicPlayer != NULL );
    m_musicPlayer->SetVolume( volume * m_pSoundRenderMgr->GetTuner()->GetMasterVolume() );
}

//=============================================================================
// SoundManager::GetAmbienceVolume
//=============================================================================
// Description: Get ambience volume
//
// Parameters:  None
//
// Return:      Volume setting from 0.0f to 1.0f 
//
//=============================================================================
float SoundManager::GetAmbienceVolume()
{
    if( m_isMuted )
    {
        return( 0.0f );
    }

    rAssert( m_pSoundRenderMgr != NULL );

#ifdef RAD_WIN32
    m_musicPlayer->SetAmbienceVolume( m_pSoundRenderMgr->GetTuner()->GetAmbienceVolume() );
#endif

    rAssert( m_musicPlayer->GetAmbienceVolume() == m_pSoundRenderMgr->GetTuner()->GetAmbienceVolume() );

    return( m_pSoundRenderMgr->GetTuner()->GetAmbienceVolume() );
}

//=============================================================================
// SoundManager::SetAmbienceVolume
//=============================================================================
// Description: Set ambience volume
//
// Parameters:  volume - new music volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetAmbienceVolume( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_pSoundRenderMgr != NULL );

    m_pSoundRenderMgr->GetTuner()->SetAmbienceVolume( volume );
    
    //
    // Sadly, we have to deal with the ambience player outside of the tuner.  The
    // daSound layer doesn't do our interactive music, so volume is controlled
    // separately.
    //
    rAssert( m_musicPlayer != NULL );
    m_musicPlayer->SetAmbienceVolume( volume * m_pSoundRenderMgr->GetTuner()->GetMasterVolume() );
}

//=============================================================================
// SoundManager::SetAmbienceVolumeWithoutTuner
//=============================================================================
// Description: Set ambience volume without affecting the tuner's internally
//              stored volume setting
//
// Parameters:  volume - new music volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetAmbienceVolumeWithoutTuner( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_musicPlayer != NULL );
    m_musicPlayer->SetAmbienceVolume( volume * m_pSoundRenderMgr->GetTuner()->GetMasterVolume() );
}

//=============================================================================
// SoundManager::GetCalculatedAmbienceVolume
//=============================================================================
// Description: Hack!!  Used to figure out what the ambience volume should
//              be based on the sfx volume, to tie it to the effect slider.
//
// Parameters:  None
//
// Return:      float 
//
//=============================================================================
float SoundManager::GetCalculatedAmbienceVolume()
{
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    globalSettings* settings;
    float ratio;
    float newVolume;

    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    if(!nameSpace)
    {
        return( -1.0f );
    }
    nameSpaceObj = nameSpace->GetInstance( "tuner" );
    if(!nameSpaceObj)
    {
        return( -1.0f );
    }

    settings = static_cast<globalSettings*>( nameSpaceObj );
    ratio = GetSfxVolume() / settings->GetSfxVolume();

    newVolume = ratio * settings->GetAmbienceVolume();
    if( newVolume < 0.0f )
    {
        newVolume = 0.0f;
    }
    else if( newVolume > 1.0f )
    {
        newVolume = 1.0f;
    }

    return( newVolume );
}

//=============================================================================
// SoundManager::GetDialogueVolume
//=============================================================================
// Description: Get dialogue volume
//
// Parameters:  None
//
// Return:      Volume setting from 0.0f to 1.0f 
//
//=============================================================================
float SoundManager::GetDialogueVolume()
{
    if( m_isMuted )
    {
        return( 0.0f );
    }

    rAssert( m_pSoundRenderMgr != NULL );

    return( m_pSoundRenderMgr->GetTuner()->GetDialogueVolume() );
}

//=============================================================================
// SoundManager::DebugRender
//=============================================================================
// Description: Display the sound debug info
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::DebugRender()
{
    m_debugDisplay->Render();
}

//=============================================================================
// SoundManager::SetDialogueVolume
//=============================================================================
// Description: Set dialogue volume
//
// Parameters:  volume - new dialog volume
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetDialogueVolume( float volume )
{
    if( m_isMuted )
    {
        return;
    }

    rAssert( m_pSoundRenderMgr != NULL );

    m_pSoundRenderMgr->GetTuner()->SetDialogueVolume( volume );
}

//=============================================================================
// SoundManager::PlayCarOptionMenuStinger
//=============================================================================
// Description: Play option menu sound for car slider
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::PlayCarOptionMenuStinger()
{
    if( m_isMuted )
    {
        return;
    }

    m_soundFXPlayer->PlayCarOptionStinger( GetCarVolume() );
}

//=============================================================================
// SoundManager::PlayDialogueOptionMenuStinger
//=============================================================================
// Description: Play option menu sound for dialogue slider
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::PlayDialogueOptionMenuStinger()
{
    if( m_isMuted )
    {
        return;
    }

    m_soundFXPlayer->PlayDialogOptionStinger( GetDialogueVolume() );
}

//=============================================================================
// SoundManager::PlayMusicOptionMenuStinger
//=============================================================================
// Description: Play option menu sound for music slider
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::PlayMusicOptionMenuStinger()
{
    //
    // Music is special.  We're already playing music in the front end,
    // so we don't need to play a stinger there.  We need it in game,
    // though.
    //
    if( m_isMuted || ( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ) )
    {
        return;
    }

    m_soundFXPlayer->PlayMusicOptionStinger( GetMusicVolume() );
}

//=============================================================================
// SoundManager::PlaySfxOptionMenuStinger
//=============================================================================
// Description: Play option menu sound for sfx slider
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::PlaySfxOptionMenuStinger()
{
    if( m_isMuted )
    {
        return;
    }

    m_soundFXPlayer->PlaySfxOptionStinger( GetSfxVolume() );
}

void SoundManager::LoadNISSound( radKey32 NISSoundID, NISSoundLoadedCallback* callback )
{
    if( m_isMuted )
    {
        return;
    }

    m_NISPlayer->LoadNISSound( NISSoundID, callback );
}

void SoundManager::PlayNISSound( radKey32 NISSoundID, rmt::Box3D* boundingBox, NISSoundPlaybackCompleteCallback* callback )
{ 
    if( m_isMuted )
    {
        return;
    }

    m_NISPlayer->PlayNISSound( NISSoundID, boundingBox, callback );
}

void SoundManager::StopAndDumpNISSound( radKey32 NISSoundID )
{ 
    if( m_isMuted )
    {
        return;
    }

    m_NISPlayer->StopAndDumpNISSound( NISSoundID ); 
}

//=============================================================================
// SoundManager::StopForMovie
//=============================================================================
// Description: To be called when we want to stop the action for an FMV
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::StopForMovie()
{
    if ( !m_stoppedForMovie )
    {
        if( m_isMuted )
        {
            return;
        }

        //
        // Shut the works down
        //
        m_musicPlayer->StopForMovie();
        GetInteriorManager()->UnloadGagSounds();
        m_pSoundRenderMgr->GetPlayerManager()->PausePlayers();
        m_stoppedForMovie = true;
    }
}

//=============================================================================
// SoundManager::ResumeAfterMovie
//=============================================================================
// Description: Call this to start everything up again after an FMV
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::ResumeAfterMovie()
{
    if ( m_stoppedForMovie )
    {
        if( m_isMuted )
        {
            return;
        }

        m_musicPlayer->ResumeAfterMovie();
        m_pSoundRenderMgr->GetPlayerManager()->ContinuePlayers();
        m_stoppedForMovie = false;
    }
}

//=============================================================================
// SoundManager::IsStoppedForMovie
//=============================================================================

bool SoundManager::IsStoppedForMovie()
{
    if( m_isMuted )
    {
        return true;
    }
    else
    {
        return ( m_stoppedForMovie 
                 && m_musicPlayer->IsStoppedForMovie() 
                 && m_pSoundRenderMgr->GetPlayerManager()->AreAllPlayersStopped() );
    }
}

//=============================================================================
// SoundManager::MuteNISPlayers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::MuteNISPlayers()
{
    m_pSoundRenderMgr->GetTuner()->MuteNIS();
}

//=============================================================================
// SoundManager::UnmuteNISPlayers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SoundManager::UnmuteNISPlayers()
{
    m_pSoundRenderMgr->GetTuner()->UnmuteNIS();
}

//=============================================================================
// SoundManager::RestartSupersprintMusic
//=============================================================================
// Description: Give the supersprint music a kick
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::RestartSupersprintMusic()
{
    m_musicPlayer->RestartSupersprintMusic();
}

//=============================================================================
// SoundManager::GetBeatValue
//=============================================================================
// Description: Pass on the beat from the music player
//
// Parameters:  None
//
// Return:      float from 0.0f to 4.0f
//
//=============================================================================
float SoundManager::GetBeatValue()
{
    rAssert( m_musicPlayer != NULL );

    return( m_musicPlayer->GetBeatValue() );
}

//=============================================================================
// SoundManager::IsFoodCharacter
//=============================================================================
// Description: Determine whether this character gets Askfood lines or
//              Idlereply
//
// Parameters:  theGuy - ambient character being talked to
//
// Return:      true for Askfood, false for Idlereply 
//
//=============================================================================
bool SoundManager::IsFoodCharacter( Character* theGuy )
{
    return( DialogLine::IsFoodCharacter( theGuy ) );
}

//=============================================================================
// SoundManager::SetDialogueLanguage
//=============================================================================
// Description: Switch the current cement file for dialogue to match the
//              given language.
//
// Parameters:  language - new language to use
//
// Return:      void 
//
//=============================================================================
void SoundManager::SetDialogueLanguage( Scrooby::XLLanguage language )
{
    // TODO: Commented out to get PAL working for alpha.  It's basically a no-op anyway until we get localized dialogue. --jdy
    m_pSoundRenderMgr->SetLanguage( language );
}

//=============================================================================
// SoundManager::LoadData
//=============================================================================
// Description: Load sound settings (from saved game file).
//
// Parameters:  
//
// Return:      void 
//
//=============================================================================
void SoundManager::LoadData( const GameDataByte* dataBuffer, unsigned int numBytes )
{
    SoundSettings soundSettings;
    SoundMode loadedSoundMode;
    float calculatedAmbienceVolume;

#ifdef RAD_WIN32 // temp
    return;
#endif

    memcpy( &soundSettings, dataBuffer, sizeof( soundSettings ) );

    this->SetMusicVolume( soundSettings.musicVolume );
    this->SetSfxVolume( soundSettings.sfxVolume );
    this->SetCarVolume( soundSettings.carVolume );

    calculatedAmbienceVolume = GetCalculatedAmbienceVolume();
    //
    // If the volume returned is less than zero, we haven't initialized
    // the scripts yet.  This will get taken care of in OnBootupComplete(),
    // then.
    //
    if( calculatedAmbienceVolume >= 0.0f )
    {
        SetAmbienceVolume( GetCalculatedAmbienceVolume() );
    }

    //
    // Hack!  Hack!  Hack!  Hack!
    //
    // This is horrible.  I'm not allowed to change the save structure, because we're
    // too close to final.  I can't cast that bool to an enumeration, because the PS2
    // changes its size in release.  The Xbox compiler won't let me set bit flags on
    // booleans.  So, I'm going to use the dialogue volume to signal stereo/mono by
    // adding 100 to the volume to signal mono.  I feel dirty.
    //
    if( soundSettings.dialogVolume >= 100.0f )
    {
        this->SetDialogueVolume( soundSettings.dialogVolume - 100.0f );
        loadedSoundMode = SOUND_MONO;
    }
    else
    {
        this->SetDialogueVolume( soundSettings.dialogVolume );
        if( soundSettings.isSurround )
        {
            loadedSoundMode = SOUND_SURROUND;
        }
        else
        {
            loadedSoundMode = SOUND_STEREO;
        }
    }

#ifdef RAD_GAMECUBE
    //
    // GameCube's IPL needs to override the loaded settings, sadly
    //
    u32 GCSoundMode = OSGetSoundMode();
    if( GCSoundMode == OS_SOUND_MODE_MONO )
    {
        //
        // IPL says mono, we go mono
        //
        loadedSoundMode = SOUND_MONO;
    }
    else
    {
        if( loadedSoundMode == SOUND_MONO )
        {
            //
            // IPL says stereo, we go stereo.  Since the saved game had said
            // mono, we need to choose a sub-sound mode.  We'll go with ProLogic.
            //
            loadedSoundMode = SOUND_SURROUND;
        }
    }

#endif

    this->SetSoundMode( loadedSoundMode );
}

//=============================================================================
// SoundManager::SaveData
//=============================================================================
// Description: Save sound settings (to saved game file).
//
// Parameters:  
//
// Return:      void 
//
//=============================================================================
void SoundManager::SaveData( GameDataByte* dataBuffer, unsigned int numBytes )
{
    SoundMode mode;
    SoundSettings soundSettings;

    soundSettings.musicVolume = this->GetMusicVolume();
    soundSettings.sfxVolume = this->GetSfxVolume();
    soundSettings.carVolume = this->GetCarVolume();
    soundSettings.dialogVolume = this->GetDialogueVolume();

    mode = this->GetSoundMode();
    if( mode == SOUND_MONO )
    {
        //
        // Horrible stinky hack explained in LoadData()
        //
        soundSettings.dialogVolume += 100.0f;
        soundSettings.isSurround = false;
    }
    else if( mode == SOUND_STEREO )
    {
        soundSettings.isSurround = false;
    }
    else
    {
        soundSettings.isSurround = true;
    }

    memcpy( dataBuffer, &soundSettings, sizeof( soundSettings ) );
}

//=============================================================================
// SoundManager::ResetData
//=============================================================================
// Description: Reset sound settings to defaults.
//
// Parameters:  
//
// Return:      void 
//
//=============================================================================
void SoundManager::ResetData()
{
    if( GetGameDataManager()->IsGameLoaded() )
    {
        // since sound settings can be changed in the FE, we should
        // never reset to defaults if a game is already loaded
        //
        return;
    }

    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    globalSettings* settings;

    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    nameSpaceObj = nameSpace->GetInstance( "tuner" );
    rAssert( nameSpaceObj != NULL );

    settings = static_cast<globalSettings*>( nameSpaceObj );

    SetSfxVolume( settings->GetSfxVolume() );
    SetMusicVolume( settings->GetMusicVolume() );
    SetAmbienceVolume( settings->GetAmbienceVolume() );
    SetDialogueVolume( settings->GetDialogueVolume() );
    SetCarVolume( settings->GetCarVolume() );

    //
    // Sound mode.  For GameCube, get it from the IPL (Initial Program
    // Loader, the thingy you get when you start up a GameCube without
    // a disc).  For PS2 and Xbox, default to stereo (RadSound ignores
    // this stuff for Xbox anyway, since these settings are supposed
    // to be changed from the dashboard).
    //
#ifdef RAD_GAMECUBE
    u32 GCSoundMode = OSGetSoundMode();
    if( GCSoundMode == OS_SOUND_MODE_MONO )
    {
        SetSoundMode( SOUND_MONO );
    }
    else
    {
        SetSoundMode( SOUND_SURROUND );
    }
#else
    SetSoundMode( SOUND_SURROUND );
#endif
}

#ifdef RAD_WIN32
//=============================================================================
// SoundManager::GetConfigName
//=============================================================================
// Description: Returns the name of the sound manager's config
//
// Parameters:  n/a
//
// Returns:     
//
//=============================================================================

const char* SoundManager::GetConfigName() const
{
    return "Sound";
}

//=============================================================================
// SoundManager::GetNumProperties
//=============================================================================
// Description: Returns the number of config properties
//
// Parameters:  n/a
//
// Returns:     
//
//=============================================================================

int SoundManager::GetNumProperties() const
{
    return 5;
}

//=============================================================================
// SoundManager::LoadDefaults
//=============================================================================
// Description: Loads the default configuration for the sound manager.
//
// Parameters:  n/a
//
// Returns:     
//
//=============================================================================

void SoundManager::LoadDefaults()
{
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    globalSettings* settings;

    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );
    nameSpaceObj = nameSpace->GetInstance( "tuner" );
    rAssert( nameSpaceObj != NULL );

    settings = static_cast<globalSettings*>( nameSpaceObj );

    SetSfxVolume( settings->GetSfxVolume() );
    SetMusicVolume( settings->GetMusicVolume() );
    SetAmbienceVolume( settings->GetAmbienceVolume() );
    SetDialogueVolume( settings->GetDialogueVolume() );
    SetCarVolume( settings->GetCarVolume() );
}

//=============================================================================
// SoundManager::LoadConfig
//=============================================================================
// Description: Loads the manager's configuration
//
// Parameters:  n/a
//
// Returns:     
//
//=============================================================================

void SoundManager::LoadConfig( ConfigString& config )
{
    char property[ ConfigString::MaxLength ];
    char value[ ConfigString::MaxLength ];

    while ( config.ReadProperty( property, value ) )
    {
        if( _stricmp( property, "sfx" ) == 0 )
        {
            float val = (float) atof( value );
            if( val >= 0 && val <= 1 )
            {
                SetSfxVolume( val );
            }
        }
        else if( _stricmp( property, "music" ) == 0 )
        {
            float val = (float) atof( value );
            if( val >= 0 && val <= 1 )
            {
                SetMusicVolume( val );
            }
        }
        else if( _stricmp( property, "ambience" ) == 0 )
        {
            float val = (float) atof( value );
            if( val >= 0 && val <= 1 )
            {
                SetAmbienceVolume( val );
            }
        }
        else if( _stricmp( property, "dialogue" ) == 0 )
        {
            float val = (float) atof( value );
            if( val >= 0 && val <= 1 )
            {
                SetDialogueVolume( val );
            }
        }
        else if( _stricmp( property, "car" ) == 0 )
        {
            float val = (float) atof( value );
            if( val >= 0 && val <= 1 )
            {
                SetCarVolume( val );
            }
        }
    }
}

//=============================================================================
// SoundManager::SaveConfig
//=============================================================================
// Description: Saves the manager's configuration to the config string.
//
// Parameters:  config string to save to
//
// Returns:     
//
//=============================================================================

void SoundManager::SaveConfig( ConfigString& config )
{
    char value[ 32 ];

    sprintf( value, "%f", GetSfxVolume() );
    config.WriteProperty( "sfx", value );
    sprintf( value, "%f", GetMusicVolume() );
    config.WriteProperty( "music", value );
    sprintf( value, "%f", GetAmbienceVolume() );
    config.WriteProperty( "ambience", value );
    sprintf( value, "%f", GetDialogueVolume() );
    config.WriteProperty( "dialogue", value );
    sprintf( value, "%f", GetCarVolume() );
    config.WriteProperty( "car", value );
}
#endif // RAD_WIN32

void SoundManager::SetSoundMode( SoundMode mode )
{
    radSoundOutputMode radSoundMode;

    m_soundMode = mode;

    if( m_soundMode == SOUND_MONO )
    {
        radSoundMode = radSoundOutputMode_Mono;
    }
    else if( m_soundMode == SOUND_STEREO )
    {
        radSoundMode = radSoundOutputMode_Stereo;
    }
    else
    {
        radSoundMode = radSoundOutputMode_Surround;
    }

    ::radSoundHalSystemGet()->SetOutputMode( radSoundMode );
}

SoundMode SoundManager::GetSoundMode()
{
    return( m_soundMode );
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//==============================================================================
// SoundManager::SoundManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  
//
// Return:      N/A
//
//==============================================================================
SoundManager::SoundManager( bool noSound, bool noMusic, 
                            bool noEffects, bool noDialogue ) :
    m_soundLoader( NULL ),
    m_musicPlayer( NULL ),
    m_soundFXPlayer( NULL ),
    m_NISPlayer( NULL ),
    m_movingSoundManager( NULL ),
    m_isMuted( noSound ),
    m_noMusic( noMusic ),
    m_noEffects( noEffects ),
    m_noDialogue( noDialogue ),
    m_stoppedForMovie( false ),
    m_selectSoundClip( NULL ),
    m_scrollSoundClip( NULL ),
    m_selectSoundClipPlayer( NULL ),
    m_scrollSoundClipPlayer( NULL ),
    m_soundMode( SOUND_STEREO )
{
    m_dialogCoordinator = NULL;
    
    Sound::daSoundRenderingManagerCreate( GMA_AUDIO_PERSISTENT );

    if( !m_isMuted )
    {
        m_pSoundRenderMgr = Sound::daSoundRenderingManagerGet();
        rAssert( m_pSoundRenderMgr != NULL );

        m_pSoundRenderMgr->Initialize();
    }

    GetGameDataManager()->RegisterGameData( this, sizeof( SoundSettings ), "Sound Manager" );

    prepareStartupSounds();
}

//==============================================================================
// SoundManager::~SoundManager
//==============================================================================
//
// Description: Destructor.  first attempt.
//
// Parameters:  N/A
//
// Return:      N/A
//
//==============================================================================
SoundManager::~SoundManager()
{
    GetEventManager()->RemoveAll( this );

    if( m_isMuted )
    {
        return;
    }

    delete( GMA_PERSISTENT, m_soundFXPlayer);
    delete( GMA_PERSISTENT, m_movingSoundManager);
    delete( GMA_PERSISTENT, m_NISPlayer);
    delete( GMA_PERSISTENT, m_dialogCoordinator);
    delete( GMA_PERSISTENT, m_musicPlayer);
    delete( GMA_PERSISTENT, m_soundLoader);

    Sound::daSoundRenderingManagerTerminate();

    delete( GMA_PERSISTENT, m_debugDisplay);
}

//=============================================================================
// SoundManager::initialize
//=============================================================================
// Description: Do some class member initialization tasks that we can't really 
//              do in the constructor.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundManager::initialize()
{
    if( m_isMuted )
    {
        //
        // We need to intercept dialog skip events, since the dialog coordinator
        // isn't going to do it if we're muted
        //
        GetEventManager()->AddListener( this, EVENT_CONVERSATION_SKIP );
        return;
    }
    else
    {
        //
        // Set up a few tuner-related events
        //
        GetEventManager()->AddListener( this, EVENT_CONVERSATION_START );
        GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE );
        GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_END );
        GetEventManager()->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
        GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_START );
        GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_END );
        GetEventManager()->AddListener( this, EVENT_EXIT_INTERIOR_START );
        GetEventManager()->AddListener( this, EVENT_EXIT_INTERIOR_END );
        GetEventManager()->AddListener( this, EVENT_MISSION_RESET );
        GetEventManager()->AddListener( this, EVENT_CHARACTER_POS_RESET );
        GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED_SYNC_SOUND );
    }
        
    m_debugDisplay = new( GMA_PERSISTENT ) SoundDebugDisplay( m_pSoundRenderMgr );
    m_soundLoader = new( GMA_PERSISTENT ) SoundLoader();
    m_musicPlayer = new( GMA_PERSISTENT ) MusicPlayer( *(m_pSoundRenderMgr->GetTuner()) );
    m_dialogCoordinator = new( GMA_PERSISTENT ) DialogCoordinator( m_pSoundRenderMgr->GetSoundNamespace() );
    m_NISPlayer = new( GMA_PERSISTENT ) NISSoundPlayer();
    m_movingSoundManager = new( GMA_PERSISTENT ) MovingSoundManager();
    m_soundFXPlayer = new( GMA_PERSISTENT ) SoundEffectPlayer();
    m_avatarSoundPlayer.Initialize();
    m_listener.Initialize( *m_pSoundRenderMgr );

    //
    // Apply the non-global mute options
    //
    if( m_noMusic )
    {
        m_pSoundRenderMgr->GetTuner()->SetMusicVolume( 0.0f );
    }
    if( m_noEffects )
    {
        m_pSoundRenderMgr->GetTuner()->SetSfxVolume( 0.0f );
    }
    if( m_noDialogue )
    {
        m_pSoundRenderMgr->GetTuner()->SetDialogueVolume( 0.0f );
    }
    
    //
    // Register a factory for creating the global settings object
    //
    ::radFactoryRegister( "globalSettings", (radFactoryOutParamProc*) ::GlobalSettingsObjCreate );
    ::radFactoryRegister( "reverbSettings", (radFactoryOutParamProc*) ::ReverbSettingsObjCreate );
    ::radFactoryRegister( "positionalSoundSettings", (radFactoryOutParamProc*) ::PositionalSettingsObjCreate );

#ifdef RAD_WIN32
    //
    // Register with the game config manager
    //
    GetGameConfigManager()->RegisterConfig( this );
#endif
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

void SoundManager::prepareStartupSounds()
{
    //
    // Go direct to RadSound to load two sounds that we can 
    // play for bootcheck screens
    //
    IRadSoundRsdFileDataSource* selectFileDataSource =
        radSoundRsdFileDataSourceCreate( GMA_DEFAULT );
    selectFileDataSource->AddRef();
    selectFileDataSource->InitializeFromFileName( "sound/accept.rsd",
                                                  false,
                                                  0,
                                                  IRadSoundHalAudioFormat::Frames,
                                                  Sound::SoundNucleusGetClipFileAudioFormat() );
    m_selectSoundClip = radSoundClipCreate( GMA_DEFAULT );
    m_selectSoundClip->AddRef();
    m_selectSoundClip->Initialize(
        selectFileDataSource,
        ::radSoundHalSystemGet()->GetRootMemoryRegion(),
        false,
        "sound/accept.rsd" );
    selectFileDataSource->Release( );

    m_selectSoundClipPlayer = radSoundClipPlayerCreate( GMA_DEFAULT );
    m_selectSoundClipPlayer->AddRef();

    IRadSoundRsdFileDataSource* scrollFileDataSource =
        radSoundRsdFileDataSourceCreate( GMA_DEFAULT );
    scrollFileDataSource->AddRef();
    scrollFileDataSource->InitializeFromFileName( "sound/scroll.rsd",
                                                  false,
                                                  0,
                                                  IRadSoundHalAudioFormat::Frames,
                                                  Sound::SoundNucleusGetClipFileAudioFormat() );
    m_scrollSoundClip = radSoundClipCreate( GMA_DEFAULT );
    m_scrollSoundClip->AddRef();
    m_scrollSoundClip->Initialize(
        scrollFileDataSource,
        ::radSoundHalSystemGet()->GetRootMemoryRegion(),
        false,
        "sound/scroll.rsd" );
    scrollFileDataSource->Release( );

    m_scrollSoundClipPlayer = radSoundClipPlayerCreate( GMA_DEFAULT );
    m_scrollSoundClipPlayer->AddRef();

    //
    // Starting listening for the select/scroll events
    //
    GetEventManager()->AddListener( this, EVENT_FE_MENU_SELECT );
    GetEventManager()->AddListener( this, EVENT_FE_MENU_UPORDOWN );
}

void SoundManager::dumpStartupSounds()
{
    m_selectSoundClip->Release();
    m_selectSoundClip = NULL;

    m_selectSoundClipPlayer->Release();
    m_selectSoundClipPlayer = NULL;

    m_scrollSoundClip->Release();
    m_scrollSoundClip = NULL;

    m_scrollSoundClipPlayer->Release();
    m_scrollSoundClipPlayer = NULL;

    GetEventManager()->RemoveListener( this, EVENT_FE_MENU_SELECT );
    GetEventManager()->RemoveListener( this, EVENT_FE_MENU_UPORDOWN );
}

void SoundManager::playStartupAcceptSound()
{
    if( m_selectSoundClip->GetState() == IRadSoundClip::Initialized )
    {
        m_selectSoundClipPlayer->SetClip( m_selectSoundClip );
        m_selectSoundClipPlayer->Play();
    }
}

void SoundManager::playStartupScrollSound()
{
    if( m_scrollSoundClip->GetState() == IRadSoundClip::Initialized )
    {
        m_scrollSoundClipPlayer->SetClip( m_scrollSoundClip );
        m_scrollSoundClipPlayer->Play();
    }
}