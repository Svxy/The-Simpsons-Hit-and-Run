//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        bootupcontext.cpp
//
// Description: Implementation of BootupContext.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifdef RAD_RELEASE
    #ifndef RAD_E3
        #define SHOW_MOVIES
    #endif
#endif

//========================================
// System Includes
//========================================
#include <raddebug.hpp>
#include <radtime.hpp>
#include <raddebugwatch.hpp>
#include <radmovie2.hpp>
#include <p3d/utility.hpp>
#include <p3d/context.hpp>
#include <pddi/pddi.hpp>

//========================================
// Project Includes
//========================================
#include <contexts/bootupcontext.h>

#include <atc/atcmanager.h>
#include <cards/cardgallery.h>
#include <cheats/cheatinputsystem.h>
#include <constants/movienames.h>
#include <contexts/contextenum.h>
#include <data/gamedatamanager.h>
#include <data/memcard/memorycardmanager.h>
#include <gameflow/gameflow.h>
#include <input/inputmanager.h>
#include <interiors/interiormanager.h>
#include <loading/loadingmanager.h>
#include <main/commandlineoptions.h>
#include <memory/leakdetection.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <presentation/presentation.h>
#include <presentation/presevents/presentationevent.h>
#include <presentation/gui/guisystem.h>
#include <presentation/tutorialmanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>
#include <sound/soundmanager.h>
#include <supersprint/supersprintmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/character/charactermanager.h>

#ifdef RAD_GAMECUBE
    #include <main/gamecube_extras/gcmanager.h>
#endif

#ifdef RAD_WIN32
    #include <main/win32platform.h>
    #include <data/config/gameconfigmanager.h>
#endif

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
BootupContext* BootupContext::spInstance = NULL;

#ifdef RAD_RELEASE
    #ifdef RAD_PS2
        // TC: Edwin (Singh) says that the PS2 TRC requires that the license screen
        //     be displayed for at least 5 seconds.
        //
        const int MINIMUM_LICENSE_SCREEN_DISPLAY_TIME = 5000; // in msec
    #else
        const int MINIMUM_LICENSE_SCREEN_DISPLAY_TIME = 1000; // in msec
    #endif
#else
    const int MINIMUM_LICENSE_SCREEN_DISPLAY_TIME = 1000; // in msec
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// BootupContext::GetInstance
//==============================================================================
//
// Description: - Access point for the BootupContext singleton.  
//              - Creates the BootupContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the BootupContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
BootupContext* BootupContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) BootupContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//=============================================================================
// BootupContext::StartMovies
//=============================================================================
// Description: Comment
//
// Parameters:  Game Mode (Frontend/In-Game)
//
// Return:      void 
//
//=============================================================================
void BootupContext::StartMovies()
{
#ifndef FINAL
    if( CommandLineOptions::Get( CLO_SKIP_FE ) )
    {
        // enable 'unlock missions' cheat for 'skipfe' commandline option
        //
        GetCheatInputSystem()->SetCheatEnabled( CHEAT_ID_UNLOCK_MISSIONS, true );

        short levelIndex = CommandLineOptions::GetDefaultLevel();

        if( levelIndex == -1 ) // L0 = minigame
        {
            SetGameplayManager( SuperSprintManager::GetInstance() );

            // skip FE and go to mini-game
            //
            GetGameFlow()->SetContext( CONTEXT_SUPERSPRINT_FE );
        }
        else
        {
            SetGameplayManager( MissionManager::GetInstance() );

            // register controller ID [0] for player [0], by default
            //
            GetInputManager()->RegisterControllerID( 0, 0 );

            short missionIndex = CommandLineOptions::GetDefaultMission();
            if( levelIndex == RenderEnums::L1 )
            {
                // special case for level 1 due to tutorial mission being mission 0
                //
                missionIndex++;
            }

            // set level and mission to load for normal gameplay
            //
            GetGameplayManager()->SetLevelIndex( static_cast< RenderEnums::LevelEnum >( levelIndex ) );
            GetGameplayManager()->SetMissionIndex( static_cast< RenderEnums::MissionEnum >( missionIndex ) );

            // skip FE and go to normal gameplay
            //
            GetGameFlow()->SetContext( CONTEXT_LOADING_GAMEPLAY );
        }
    }
    else
#endif // !FINAL
    {
#ifdef SHOW_MOVIES
        if( CommandLineOptions::Get( CLO_SKIP_MOVIE ) )
        {
            // Switch to frontend context.
            GetGameFlow()->SetContext( CONTEXT_FRONTEND );
        }
        else
        {
            FMVEvent* pEvent = 0;

            GetPresentationManager()->QueueFMV( &pEvent, this );
            strcpy( pEvent->fileName, MovieNames::VUGLOGO );
            pEvent->SetRenderLayer( RenderEnums::PresentationSlot );
            pEvent->SetAutoPlay( true );
            pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );
            pEvent->SetClearWhenDone( true );

            GetPresentationManager()->QueueFMV( &pEvent, this );
            strcpy( pEvent->fileName, MovieNames::FOXLOGO );
            pEvent->SetRenderLayer( RenderEnums::PresentationSlot );
            pEvent->SetAutoPlay( true );
            pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );
            pEvent->SetClearWhenDone( true );

			GetPresentationManager()->QueueFMV( &pEvent, this );
            strcpy( pEvent->fileName, MovieNames::GRACIELOGO );
            pEvent->SetRenderLayer( RenderEnums::PresentationSlot );
            pEvent->SetAutoPlay( true );
            pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );
            pEvent->SetClearWhenDone( true );

            GetPresentationManager()->QueueFMV( &pEvent, this );
            strcpy( pEvent->fileName, MovieNames::RADICALLOGO );
            pEvent->SetRenderLayer( RenderEnums::PresentationSlot );
            pEvent->SetAutoPlay( true );
            pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );
            pEvent->SetClearWhenDone( true );

            GetRenderManager()->mpLayer( RenderEnums::GUI )->Chill();
        }
#else
        // Switch to frontend context.
        GetGameFlow()->SetContext( CONTEXT_FRONTEND );
#endif
    }
}

void
BootupContext::StartLoadingSound()
{
    GetSoundManager()->OnBootupStart();

    GetLoadingManager()->AddCallback( this, (void*)GetSoundManager() );
}

#ifdef RAD_WIN32
void BootupContext::LoadConfig()
{
    // Load the config file for the game.
    GameConfigManager* gc = GetGameConfigManager();
    bool success = gc->LoadConfigFile();

    // If we couldn't load the config file, create a new one.
    if( !success )
    {
        Win32Platform::GetInstance()->LoadDefaults();
        gc->SaveConfigFile();
    }
}
#endif

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//==============================================================================
// BootupContext::OnStart
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void BootupContext::OnStart( ContextEnum previousContext )
{
    SetMemoryIdentification( "BootupContext" );
    HeapMgr()->PrepareHeapsFeCleanup();
    HeapMgr()->PrepareHeapsFeSetup();
    HeapMgr()->PushHeap (GMA_PERSISTENT);
#ifdef DEBUGINFO_ENABLED
    DebugInfo::InitializeStaticVariables();
#endif

    MEMTRACK_PUSH_FLAG( "Bootup" );
    GetGameDataManager()->Init();

#ifdef RAD_PS2
    // must load memory card info first, before anything else, since the
    // memory card boot-up check is done right at the beginning
    //
    if( !CommandLineOptions::Get( CLO_SKIP_MEMCHECK ) )
    {
        GetMemoryCardManager()->LoadMemcardInfo();
    }
#endif

    GetGuiSystem()->Init();
    GetGuiSystem()->RegisterUserInputHandlers();

    GetCardGallery()->Init();
    GetCheatInputSystem()->Init();
    GetTutorialManager()->Initialize();
    GetATCManager()->Init();

    GetCharacterSheetManager()->InitCharacterSheet();

    GetPresentationManager()->InitializePlayerDrawable();

#ifdef RAD_GAMECUBE
    //Initialize the GCManager's timers for testing reset and such.
    GCManager::GetInstance()->Init();
#endif

    GetWorldPhysicsManager()->Init();

    GetInteriorManager()->OnBootupStart();

    // TC: for PS2, we shouldn't start loading sound stuff until we get to the
    //     license screen to avoid any synchronous script parsing that could
    //     lock-up the CPU briefly on a GUI prompt screen
    //
//    GetSoundManager()->OnBootupStart();

    GetCharacterManager()->PreLoad();

    // load rewards script
    //
    GetRewardsManager()->LoadScript();

    // preload some data that is common across all levels 
    // MissionScriptLoader::LoadP3DFile hacked to supress their loads in mission scripts
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, "art\\cars\\common.p3d", GMA_DEFAULT, "Global" );
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, "art\\cars\\huskA.p3d", GMA_DEFAULT, "Global");
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, "art\\phonecamera.p3d", GMA_DEFAULT, "Global");
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, "art\\cards.p3d", GMA_DEFAULT, "Global");
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, "art\\wrench.p3d", GMA_DEFAULT, "Global");
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, "art\\missions\\generic\\missgen.p3d", GMA_DEFAULT, "Global");

    //
    // Address any loading requests that the managers have queued up
    //
    GetLoadingManager()->AddCallback( this );

#if defined( RAD_WIN32 ) && defined( SHOW_MOVIES )
    GetInputManager()->GetFEMouse()->SetInGameMode( true );
#endif
}


//==============================================================================
// BootupContext::OnStop
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void BootupContext::OnStop( ContextEnum nextContext )
{
    rTunePrintf("BootupContext::OnStop... ");

    GetGuiSystem()->UnregisterUserInputHandlers();

    // release GUI bootup
    GetGuiSystem()->HandleMessage( GUI_MSG_RELEASE_BOOTUP );

#if defined( RAD_WIN32 ) && defined( SHOW_MOVIES )
    GetInputManager()->GetFEMouse()->SetInGameMode( false );
#endif


    MEMTRACK_POP_FLAG( "" );

    HeapMgr()->PopHeap ( GMA_PERSISTENT );

    rTunePrintf("Finished\n");
    SetMemoryIdentification( "BootupContext Finished" );
}


//==============================================================================
// BootupContext::OnUpdate
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void BootupContext::OnUpdate( unsigned int elapsedTime )
{
    if( m_elapsedTime != -1 )
    {
        if( m_elapsedTime > MINIMUM_LICENSE_SCREEN_DISPLAY_TIME &&
            m_bootupLoadCompleted && m_soundLoadCompleted )
        {
            // Tell GUI system to quit out of the boot-up state
            GetGuiSystem()->HandleMessage( GUI_MSG_QUIT_BOOTUP );

            m_elapsedTime = -1;
        }
        else
        {
            m_elapsedTime += elapsedTime;
        }
    }

    // update game data manager
    GetGameDataManager()->Update( elapsedTime );

    GetPresentationManager()->Update( elapsedTime );

    // update GUI system
    GetGuiSystem()->Update( elapsedTime );
}


//==============================================================================
// BootupContext::OnSuspend
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void BootupContext::OnSuspend()
{
}


//==============================================================================
// BootupContext::OnResume
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void BootupContext::OnResume()
{
}


//==============================================================================
// BootupContext::OnHandleEvent
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void BootupContext::OnHandleEvent( EventEnum id, void* pEventData )
{
}

//=============================================================================
// BootupContext::OnProcessRequestsComplete
//=============================================================================
// Description: Called when startup loading is done
//
// Parameters:  pUserData - unused
//
// Return:      void 
//
//=============================================================================
void BootupContext::OnProcessRequestsComplete( void* pUserData )
{
    if( pUserData == GetSoundManager() )
    {
        // set flag indicating all sound loads have completed
        //
        m_soundLoadCompleted = true;
    }
    else
    {
        // set flag indicating all bootup loads (except for sound) have completed
        //
        m_bootupLoadCompleted = true;
    }

    //
    // Tell the sound manager to do some processing, now that the scripts
    // are sure to have been loaded.
    //
    // NOTE: I've moved this here since this call triggers a CPU-hogging
    // bit of dialog script postprocessing.  That processing should be pulled out
    // and done offline, but until then, do this somewhere where
    // it won't starve the completion of FMVs. -- Esan
    //
    if( m_bootupLoadCompleted && m_soundLoadCompleted )
    {
        GetSoundManager()->OnBootupComplete();

        GetInputManager()->ToggleRumble( false );
    }
}

//=============================================================================
// BootupContext::OnPresentationEventBegin
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* pEvent )
//
// Return:      void 
//
//=============================================================================
void BootupContext::OnPresentationEventBegin( PresentationEvent* pEvent )
{
}

//=============================================================================
// BootupContext::OnPresentationEventLoadComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* pEvent )
//
// Return:      void 
//
//=============================================================================
void BootupContext::OnPresentationEventLoadComplete( PresentationEvent* pEvent )
{
}


//=============================================================================
// BootupContext::OnPresentationEventEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* pEvent )
//
// Return:      void 
//
//=============================================================================
void BootupContext::OnPresentationEventEnd( PresentationEvent* pEvent )
{
    if( GetPresentationManager()->IsQueueEmpty() )
    {
        GetRenderManager()->mpLayer( RenderEnums::GUI )->Warm();

        // Switch to frontend context.
        GetGameFlow()->SetContext( CONTEXT_FRONTEND );
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// BootupContext::BootupContext
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================// 
BootupContext::BootupContext()
:   m_elapsedTime( -1 ),
    m_bootupLoadCompleted( false ),
    m_soundLoadCompleted( false ),
    m_pSharedShader( 0 )
{
    m_pSharedShader = p3d::device->NewShader("simple");
    rAssert( m_pSharedShader );
    m_pSharedShader->AddRef();
}


//==============================================================================
// BootupContext::~BootupContext
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================// 
BootupContext::~BootupContext()
{
    // Too bad we can't use tEntity::Release() since is
    //a pddi object.
    if( m_pSharedShader != 0 )
    {
        m_pSharedShader->Release();
        m_pSharedShader = 0;
    }
    spInstance = NULL;
}

