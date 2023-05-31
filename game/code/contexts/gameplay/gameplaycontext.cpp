//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement GameplayContext
//
// History:     21/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radtime.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================

#include <ai/vehicle/vehicleairender.h>

#include <camera/animatedcam.h>
#include <camera/debugcam.h>
#include <camera/followcam.h> 
#include <camera/relativeanimatedcam.h>
#include <camera/chasecam.h>
#include <camera/bumpercam.h>
#include <camera/kullcam.h>
#include <camera/trackercam.h>
#include <camera/walkercam.h>
#include <camera/railcam.h>
#include <camera/wrecklesscam.h>
#include <camera/supercammanager.h>
#include <camera/conversationcam.h>
#include <camera/reversecam.h>
#include <camera/snapshotcam.h>
#ifdef RAD_WIN32
#include <camera/pccam.h>
#endif

//#include <camera/firstpersoncam.h>

#include <contexts/gameplay/gameplaycontext.h>
#include <contexts/contextenum.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>

#include <data/gamedatamanager.h>

#include <interiors/interiormanager.h>
#include <memory/leakdetection.h>
#include <memory/srrmemory.h>

#include <meta/triggervolumetracker.h>

#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <presentation/presentation.h>
#include <presentation/gui/guisystem.h>

#include <render/rendermanager/rendermanager.h>
#include <render/RenderManager/RenderLayer.h>

#include <worldsim/character/footprint/footprintmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/skidmarks/skidmarkmanager.h>
#include <worldsim/coins/sparkle.h>
#include <worldsim/hitnrunmanager.h>
#include <ai/actionbuttonmanager.h>
#include <ai/actor/actormanager.h>


#include <worldsim/character/charactermanager.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/ped/pedestrianmanager.h>

#include <render/breakables/breakablesmanager.h>
#include <render/particles/particlemanager.h>
#include <render/animentitydsgmanager/animentitydsgmanager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <worldsim/skidmarks/skidmarkgenerator.h>

#include <sound/soundmanager.h>

#include <roads/roadmanager.h>

#include <input/inputmanager.h>
#include <p3d/light.hpp>
#include <p3d/view.hpp>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
GameplayContext* GameplayContext::spInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// GameplayContext::GetInstance
//==============================================================================
//
// Description: - Access point for the GameplayContext singleton.  
//              - Creates the GameplayContext if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the GameplayContext.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
GameplayContext* GameplayContext::GetInstance()
{
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) GameplayContext;
        rAssert( spInstance );
    }
    
    return spInstance;
}

//==============================================================================
// GameplayContext::GameplayContext
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GameplayContext::GameplayContext() :
    m_PausedAllButPresentation( false ),
    mSlowMoHack( false )
{
#ifdef DEBUGWATCH
    radDbgWatchAddUnsignedInt( &mDebugPhysTiming, "Debug Phys micros", "GameplayContext", NULL, NULL );
    radDbgWatchAddUnsignedInt( &mDebugTimeDelta, "Debug dT micros", "GameplayContext", NULL, NULL );
    radDbgWatchAddUnsignedInt( &mDebugOnUpdateDT, "Gameplay dT micros", "GameplayContext", NULL, NULL );
    radDbgWatchAddBoolean(&mSlowMoHack, "Slow Mo Hack", "GameplayContext", NULL, NULL);
#endif
}

//==============================================================================
// GameplayContext::~GameplayContext
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GameplayContext::~GameplayContext()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete(&mDebugPhysTiming);
    radDbgWatchDelete(&mDebugTimeDelta);
    radDbgWatchDelete(&mDebugOnUpdateDT);
    
    radDbgWatchDelete(&mSlowMoHack);
#endif
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// GameplayContext::OnStart
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum previousContext )
//
// Return:      void 
//
//=============================================================================
void GameplayContext::OnStart( ContextEnum previousContext )
{
    // Common to all playing contexts.
    //
    PlayingContext::OnStart( previousContext );

    MEMTRACK_PUSH_FLAG( "Gameplay" );
    // do gameplay initializations only if previous context was loading context
    if( previousContext == CONTEXT_LOADING_GAMEPLAY )
    {
        // RenderManager
        //
        RenderManager* rm = GetRenderManager();

        RenderLayer* rl = rm->mpLayer( RenderEnums::LevelSlot );
        rAssert( rl );

#ifdef DEBUGWATCH
        // bootstrap vehicleai renderer
        VehicleAIRender::GetVehicleAIRender();
#endif

        // Set up cameras
        //
        unsigned int iNumPlayers = GetGameplayManager()->GetNumPlayers();

        rl->SetNumViews( iNumPlayers );
        rl->SetUpViewCam();

		p3d::inventory->SelectSection("Default");
        tLightGroup* sun = p3d::find<tLightGroup>("sun");   
        rAssert( sun );
        rm->SetLevelLayerLights( sun );

        float aspect = p3d::display->IsWidescreen() ? (16.0f / 9.0f) : (4.0f / 3.0f);

        if( iNumPlayers == 2 )
        {
            aspect = 4.0f / 1.5f;
        }

        for( unsigned int view = 0; view < iNumPlayers; view++ )
        {

            tPointCamera* cam = static_cast<tPointCamera*>( rl->pCam( view ) );
            rAssert( dynamic_cast<tPointCamera*> ( cam ) != NULL );
            rAssert( cam );

            SuperCamCentral* scc = GetSuperCamManager()->GetSCC( view );
            rAssert( scc );
            scc->SetCamera( cam );
    
            FollowCam* fc = new FollowCam( FollowCam::FOLLOW_NEAR );
            fc->SetAspect( aspect );
            fc->CopyToData();
            scc->RegisterSuperCam( fc );

            fc = new FollowCam( FollowCam::FOLLOW_FAR );
            fc->SetAspect( aspect );
            fc->CopyToData();
            scc->RegisterSuperCam( fc );

            SuperCam* sc = new BumperCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new ChaseCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new KullCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new DebugCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new TrackerCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new WalkerCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new ComedyCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new WrecklessCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new ConversationCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new ReverseCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new AnimatedCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
            sc = new RelativeAnimatedCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
//            sc = new FirstPersonCam();
//            sc->SetAspect( aspect );
//            scc->RegisterSuperCam( sc );
#ifdef RAD_WIN32
            sc = new PCCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
#endif
#if !defined(FINAL) && defined(RAD_XBOX)
            sc = new SnapshotCam();
            sc->SetAspect( aspect );
            scc->RegisterSuperCam( sc );
#endif
        }

        // Boot strap the AvatarManager.
        //
        GetAvatarManager( )->EnterGame( );
        GetActionButtonManager( )->EnterGame( );

        TrafficManager::GetInstance()->Init();
        PedestrianManager::GetInstance()->Init();
        
        // Prepare the manager to start gameplay
        //
        //GetGameplayManager()->Reset();

#ifdef RAD_DEMO
        GetGameplayManager()->ResetIdleTime();
#endif

        // Start up GUI in-game manager
        GetGuiSystem()->HandleMessage( GUI_MSG_RUN_INGAME );

        // register GUI user input handler for active players only
        //
        int activeControllerIDs = 0;
        for( int i = 0; i < MAX_PLAYERS; i++ )
        {
            int controllerID = GetInputManager()->GetControllerIDforPlayer( i );
            if( controllerID != -1 )
            {
                activeControllerIDs |= (1 << controllerID);
            }
        }

        GetGuiSystem()->RegisterUserInputHandlers( activeControllerIDs );

        // Tell GameData Manager that game has been started
        //
        GetGameDataManager()->SetGameLoaded();

        //Set up the sorting of the intersections and stuff.
        RoadManager::GetInstance()->CreateRoadNetwork();

        //Find skid mark shaders in the inventory and set proper values
        SkidMarkGenerator::InitShaders();

        GetSkidmarkManager()->Init();

        //
        // Notify the sound system of gameplay start.  This has been moved after
        // the GUI startup above, since that leads to a mission reset, which causes
        // character position in or out of the car to be decided, which the sound
        // system uses to determine which sounds to start playing.
        //
        SoundManager::GetInstance()->OnGameplayStart();

        GetInteriorManager()->OnGameplayStart();

        GetHitnRunManager()->ResetState();

        //tick character manager once, to get state system and choreo started
        GetCharacterManager()->PreSimUpdate( 0.0001f );
        GetCharacterManager()->PreSubstepUpdate( 0.0001f );
        GetCharacterManager()->Update( 0.0001f );
        GetCharacterManager()->PostSubstepUpdate( 0.0001f );
        GetCharacterManager()->PostSimUpdate( 0.0001f );

    }

    GetPresentationManager()->OnGameplayStart();

    HeapMgr()->ResetArtStats();
    GetEventManager()->TriggerEvent( EVENT_LEVEL_START );

    if( previousContext != CONTEXT_PAUSE )
    {
        AnimatedCam::CheckPendingCameraSwitch();
    }

    for( int view = 0; view < GetGameplayManager()->GetNumPlayers(); view++ )
    {
        SuperCamCentral* scc = GetSuperCamManager()->GetSCC( view );
        scc->SetIsInitialCamera(true);
    }
    GetGameplayManager()->Update( 0 );
    GetTriggerVolumeTracker()->Update( 0 );
}

//=============================================================================
// GameplayContext::OnStop
//=============================================================================
// Description: Comment
//
// Parameters:  ( ContextEnum nextContext )
//
// Return:      void 
//
//=============================================================================
void GameplayContext::OnStop( ContextEnum nextContext )
{
    GetPresentationManager()->OnGameplayStop();

    // turn off controller rumble, to comply w/ TCR/TRC standards
    //
    GetInputManager()->ToggleRumble( false );


    // Common to all playing contexts.
    //
    PlayingContext::OnStop( nextContext );
}

//=============================================================================
// GameplayContext::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void GameplayContext::OnUpdate( unsigned int elapsedTime )
{
#ifdef DEBUGWATCH
    mDebugOnUpdateDT = radTimeGetMicroseconds();
#endif

#ifndef FINAL
    if( CommandLineOptions::Get( CLO_PRINT_FRAMERATE ) )
    {
        //
        // Merasure the time taken to render frames 100-500
        //
        const unsigned int startFrame   = 200;
        const unsigned int endFrame     = 600;
        static unsigned int count = 0;
        ++count;
        static unsigned int totalTime = 0;
        totalTime += elapsedTime;

        if( count == startFrame )
        {
            totalTime = 0;
        }
        if( count == endFrame )
        {
            const unsigned int frames = endFrame - startFrame;
            float msPerFrame = totalTime / static_cast< float >( frames );
            rReleasePrintf( "====================================\n" );
            rReleasePrintf( "%d frames rendered in %dms\n", frames, totalTime );
            rReleasePrintf( "%fms per frame\n", msPerFrame );
            rReleasePrintf( "====================================\n" );
        }
    }

    static int frameCount = 60;
    frameCount--;
    if( frameCount == 0 )
    {
        HeapMgr()->ResetArtStats();
    }
#endif

    // hack for yousuf
    if( mSlowMoHack )
    {
        //elapsedTime = 1;
        elapsedTime = 2;
    }

    if( !m_PausedAllButPresentation )
    {
        float timeins = (float)(elapsedTime) / 1000.0f;

        BEGIN_PROFILE("GameplayManager");
        if ( m_state != S_SUSPENDED )
        {
            GetGameplayManager()->Update( elapsedTime );
        }
        END_PROFILE("GameplayManager");


        GetAvatarManager()->Update( timeins );

        GetFootprintManager()->Update( elapsedTime );

	    GetCharacterManager()->GarbageCollect( );

	    BEGIN_PROFILE( "Update Particles" );

	    GetParticleManager()->Update( elapsedTime );
        GetBreakablesManager()->Update( elapsedTime );
	    GetAnimEntityDSGManager()->Update( elapsedTime );

	    END_PROFILE( "Update Particles" );

    #ifdef DEBUGWATCH
        unsigned int t0 = radTimeGetMicroseconds();
    #endif 

    BEGIN_PROFILE("WorldPhysics");
        GetWorldPhysicsManager()->Update(elapsedTime);
    END_PROFILE("WorldPhysics");

        // ordering is important. Unless other parts of code change, we must call
        // this before WorldPhysManager::Update() because PedestrianManager
        // sets the flags for all characters to be updated in WorldPhys Update 
        PedestrianManager::GetInstance()->Update( elapsedTime );

    #ifdef DEBUGWATCH
        mDebugTimeDelta = elapsedTime;
        mDebugPhysTiming = radTimeGetMicroseconds()-t0 ;
    #endif 

    BEGIN_PROFILE("TriggerVolumeTracker");
        GetTriggerVolumeTracker()->Update( elapsedTime );
    END_PROFILE("TriggerVolumeTracker");

    BEGIN_PROFILE("TrafficManager");
        TrafficManager::GetInstance()->Update( elapsedTime );
    END_PROFILE("TrafficManager");

    BEGIN_PROFILE("ActorManager");
        ActorManager::GetInstance()->Update( elapsedTime );
    END_PROFILE("ActorManager");

    BEGIN_PROFILE("InteriorManager");
        GetInteriorManager()->Update( elapsedTime );
    END_PROFILE("InteriorManager");

    BEGIN_PROFILE("SkidmarkManager");
        GetSkidmarkManager()->Update( elapsedTime );
    END_PROFILE("SkidmarkManager");

    BEGIN_PROFILE( "CoinManager" );
        GetCoinManager()->Update( elapsedTime );
        GetSparkleManager()->Update( elapsedTime );
    END_PROFILE( "CoinManager" );

    BEGIN_PROFILE( "HitnRunManager" );
        GetHitnRunManager()->Update( elapsedTime );
    END_PROFILE( "HitnRunManager" );
    }

#ifdef RAD_DEMO
    GetGameplayManager()->UpdateIdleTime( elapsedTime );
#endif

    BEGIN_PROFILE("PresentationManager");
       GetPresentationManager()->Update( elapsedTime );
    END_PROFILE("PresentationManager");

    // Common to all playing contexts.
    //
    PlayingContext::OnUpdate( elapsedTime );
#ifdef DEBUGWATCH
    mDebugOnUpdateDT = radTimeGetMicroseconds()-mDebugOnUpdateDT;
#endif
}

//=============================================================================
// GameplayContext::OnSuspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayContext::OnSuspend()
{
    // Common to all playing contexts.
    //
    PlayingContext::OnSuspend();
}

//=============================================================================
// GameplayContext::OnResume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayContext::OnResume()
{
    // Common to all playing contexts.
    //
    PlayingContext::OnResume();
}

//=============================================================================
// GameplayContext::PauseAllButPresentation
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameplayContext::PauseAllButPresentation( const bool pause )
{
    m_PausedAllButPresentation = pause;
}

